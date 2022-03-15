#include "ECS/SystemManager.h"

#include <map>
#include <functional>
#include "Engine/Engine.h"
#include "ECS/Utils/Timer.h"

namespace ECS {
    SystemManager::SystemManager()
    {
        systemAllocator = new SystemAllocator(ECS_SYSTEM_MEMORY_BUFFER_SIZE, AllocateGlobal(ECS_SYSTEM_MEMORY_BUFFER_SIZE, "SystemManager"));
    }

    SystemManager::~SystemManager()
    {
        for (auto it = systemWorkOrder.rbegin(); it != systemWorkOrder.rend(); ++it)
        {
            (*it)->~ISystem();
            *it = nullptr;
        }

        systemWorkOrder.clear();
        systems.clear();

        FreeGlobal((void *)systemAllocator->GetMemoryAddress0());
        delete systemAllocator;
        systemAllocator = nullptr;
    }

    void SystemManager::Update()
    {
        f32 dtMs = Engine::Get().GetTimer().GetDeltaTime();
        for (ISystem *system : systemWorkOrder)
        {
            system->timeSinceLastUpdate += dtMs;

            system->needsUpdate = (system->updateInterval < 0.0f) || ((system->updateInterval > 0.0f) && (system->timeSinceLastUpdate > system->updateInterval));

            if (system->enabled == true && system->needsUpdate == true)
                system->PreUpdate();
        }

        for (ISystem *system : systemWorkOrder)
        {
            if (system->enabled == true && system->needsUpdate == true)
            {
                system->Update();
                system->timeSinceLastUpdate = 0.0f;
            }
        }

        for (ISystem *system : systemWorkOrder)
        {
            if (system->enabled == true && system->needsUpdate == true)
                system->PostUpdate();
        }
    }

    void SystemManager::UpdateSystemWorkOrder()
    {
        std::vector<std::vector<SystemTypeId>> vertexGroups;
        std::vector<SystemPriority> groupsPriority;

        groupByDependency(vertexGroups, groupsPriority);

        SystemGroups sortedGroups = std::move(sortGroupsByPriority(vertexGroups, groupsPriority));

        systemWorkOrder.clear();
        for (const auto &group : sortedGroups)
        {
            for (auto typeId : group.second)
            {
                ISystem *system = systems[typeId];
                if (system != nullptr)
                    systemWorkOrder.push_back(system);
            }
        }
    }

    SystemManager::SystemGroups SystemManager::sortGroupsByPriority(const std::vector<std::vector<SystemTypeId>> &vertexGroups, const std::vector<SystemPriority> &groupsPriority)
    {
        SystemGroups sortedGroups;

        std::vector<int> vertexStates(systemDependencyMatrix.size(), 0);

        for (size_t i = 0; i < vertexGroups.size(); ++i)
        {
            std::vector<SystemTypeId> order;

            for (u64 typeId : vertexGroups[i])
            {
                if (vertexStates[typeId] == 0)
                    deepFirstSearch(typeId, vertexStates, systemDependencyMatrix, order);
            }

            std::reverse(order.begin(), order.end());

            sortedGroups.insert({groupsPriority[i], order});
        }

        return sortedGroups;
    }

    void SystemManager::groupByDependency(std::vector<std::vector<SystemTypeId>> &vertexGroups, std::vector<SystemPriority> &groupsPriority)
    {
        const size_t systemCount = systemDependencyMatrix.size();

        std::vector<int> systemIndices(systemCount);
        for (int i = 0; i < systemCount; ++i)
            systemIndices[i] = i;

        while (!systemIndices.empty())
        {
            SystemTypeId index = systemIndices.back();
            systemIndices.pop_back();

            if (index == -1)
                continue;

            std::vector<SystemTypeId> group;
            std::vector<SystemTypeId> member;

            SystemPriority groupPriority = LOWEST_SYSTEM_PRIORITY;
            member.push_back(index);

            while (!member.empty())
            {
                index = member.back();
                member.pop_back();

                for (size_t i = 0; i < systemIndices.size(); ++i)
                {
                    if (systemIndices[i] != -1 && (systemDependencyMatrix[i][index] || systemDependencyMatrix[index][i]))
                    {
                        member.push_back(i);
                        systemIndices[i] = -1;
                    }
                }

                group.push_back(index);

                ISystem *system = systems[index];
                groupPriority = std::max((system != nullptr ? system->priority : NORMAL_SYSTEM_PRIORITY), groupPriority);
            }

            vertexGroups.push_back(group);
            groupsPriority.push_back(groupPriority);
        }
    }

    void SystemManager::deepFirstSearch(SystemTypeId vertex, std::vector<int> &vertexState, const std::vector<std::vector<bool>> &edges, std::vector<SystemTypeId> &output)
    {
        vertexState[vertex] = 1;

        for (size_t i = 0; i < vertexState.size(); ++i)
        {
            if (edges[i][vertex] && vertexState[i] == 0)
                deepFirstSearch(i, vertexState, edges, output);
        }

        output.push_back(vertex);
    }

    SystemWorkStateMask SystemManager::GetSystemWorkState() const
    {
        SystemWorkStateMask mask(systemWorkOrder.size());

        for (size_t i = 0; i < systemWorkOrder.size(); ++i)
            mask[i] = systemWorkOrder[i]->enabled;

        return mask;
    }

    void SystemManager::SetSystemWorkState(SystemWorkStateMask mask)
    {
        assert(mask.size() == systemWorkOrder.size() && "Provided mask does not match size of current system array.");

        for (size_t i = 0; i < systemWorkOrder.size(); ++i)
            systemWorkOrder[i]->enabled = mask[i];
    }
}
