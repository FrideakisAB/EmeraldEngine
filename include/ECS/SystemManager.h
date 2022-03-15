#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include <map>
#include <vector>
#include "ECS/API.h"
#include <unordered_map>
#include "ECS/ISystem.h"
#include <boost/noncopyable.hpp>
#include "ECS/Utils/FamilyTypeID.h"
#include "Memory/GlobalMemoryUser.h"
#include "Memory/Allocator/LinearAllocator.h"

class Engine;

namespace ECS {
    using SystemWorkStateMask = std::vector<bool>;

    class SystemManager final : ::Memory::GlobalMemoryUser, boost::noncopyable {
        friend ::Engine;

        using SystemDependencyMatrix = std::vector<std::vector<bool>>;
        using SystemRegistry = std::unordered_map<u64, ISystem *>;
        using SystemAllocator = Memory::Allocator::LinearAllocator;
        using SystemWorkOrder = std::vector<ISystem *>;
        using SystemGroups = std::multimap<SystemPriority, std::vector<SystemTypeId>, std::greater<>>;

    private:
        SystemAllocator *systemAllocator;
        SystemRegistry systems;
        SystemDependencyMatrix systemDependencyMatrix;
        SystemWorkOrder systemWorkOrder;

        void Update();

        static void deepFirstSearch(SystemTypeId vertex, std::vector<int> &vertexState, const std::vector<std::vector<bool>> &edges, std::vector<SystemTypeId> &output);
        void groupByDependency(std::vector<std::vector<SystemTypeId>> &vertexGroups, std::vector<SystemPriority> &groupsPriority);
        SystemGroups sortGroupsByPriority(const std::vector<std::vector<SystemTypeId>> &vertexGroups, const std::vector<SystemPriority> &groupsPriority);

    public:
        SystemManager();
        ~SystemManager() final;

        template<class T, class... ARGS>
        T *AddSystem(ARGS &&...systemArgs)
        {
            static const u64 STID = T::STATIC_SYSTEM_TYPE_ID;

            auto it = systems.find(STID);
            if ((systems.find(STID) != systems.end()) && (it->second != nullptr))
                return (T*)it->second;

            T *system = nullptr;
            void *memory = systemAllocator->Allocate(sizeof(T), alignof(T));
            if (memory != nullptr)
            {
                system = new(memory)T(std::forward<ARGS>(systemArgs)...);
                ((T*)memory)->systemManagerInstance = this;
                systems[STID] = system;
            }
            else
            {
                logger->Error("Memory for System is out!");
                return system;
            }

            if (STID + 1 > systemDependencyMatrix.size())
            {
                systemDependencyMatrix.resize(STID + 1);
                for (auto &sys : systemDependencyMatrix)
                    sys.resize(STID + 1);
            }

            systemWorkOrder.push_back(system);

            return system;
        }

        template<class System, class Dependency>
        void AddSystemDependency(System target, Dependency dependency)
        {
            const u64 TARGET_ID = target->GetStaticSystemTypeId();
            const u64 DEPEND_ID = dependency->GetStaticSystemTypeId();

            systemDependencyMatrix[TARGET_ID][DEPEND_ID] = true;
        }

        template<class Target, class Dependency, class... Dependencies>
        void AddSystemDependency(Target target, Dependency dependency, Dependencies &&...dependencies)
        {
            const u64 TARGET_ID = target->GetStaticSystemTypeId();
            const u64 DEPEND_ID = dependency->GetStaticSystemTypeId();

            systemDependencyMatrix[TARGET_ID][DEPEND_ID] = true;

            AddSystemDependency(target, std::forward<Dependencies>(dependencies)...);
        }

        void UpdateSystemWorkOrder();

        template<class T>
        inline T *GetSystem() const
        {
            auto it = systems.find(T::STATIC_SYSTEM_TYPE_ID);
            return it != systems.end()? (T *)it->second : nullptr;
        }

        template<class T>
        void EnableSystem()
        {
            static const SystemTypeId STID = T::STATIC_SYSTEM_TYPE_ID;

            auto it = systems.find(STID);
            if (it != systems.end())
                it->second->enabled = true;
            else
                logger->Warning("System not added: %s", typeid(T).name());
        }

        template<class T>
        void DisableSystem()
        {
            static const SystemTypeId STID = T::STATIC_SYSTEM_TYPE_ID;

            auto it = systems.find(STID);
            if (it != systems.end())
                it->second->enabled = false;
            else
                logger->Warning("System not added: %s", typeid(T).name());
        }

        template<class T>
        void SetSystemUpdateInterval(f32 interval_ms)
        {
            static const SystemTypeId STID = T::STATIC_SYSTEM_TYPE_ID;

            auto it = systems.find(STID);
            if (it != systems.end())
                it->second->updateInterval = interval_ms;
            else
                logger->Warning("System not added: %s", typeid(T).name());
        }

        template<class T>
        void SetSystemPriority(SystemPriority newPriority)
        {
            static const SystemTypeId STID = T::STATIC_SYSTEM_TYPE_ID;

            auto it = systems.find(STID);
            if (it != systems.end())
            {
                it->second->priority = newPriority;
                UpdateSystemWorkOrder();
            }
            else
                logger->Warning("System not added: %s", typeid(T).name());
        }

        [[nodiscard]] SystemWorkStateMask GetSystemWorkState() const;
        void SetSystemWorkState(SystemWorkStateMask mask);

        template<class... ActiveSystems>
        SystemWorkStateMask GenerateActiveSystemWorkState(ActiveSystems &&...activeSystems)
        {
            SystemWorkStateMask mask(systemWorkOrder.size(), false);

            std::list<ISystem*> AS = {activeSystems...};
            for (auto system : AS)
            {
                for (size_t i = 0; i < systemWorkOrder.size(); ++i)
                {
                    if (systemWorkOrder[i]->GetStaticSystemTypeId() == system->GetStaticSystemTypeId())
                    {
                        mask[i] = true;
                        break;
                    }
                }
            }

            return mask;
        }
    };
}

#endif
