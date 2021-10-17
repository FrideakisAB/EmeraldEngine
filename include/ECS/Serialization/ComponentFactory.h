#ifndef COMPONENTFACTORY_H
#define COMPONENTFACTORY_H

#include "ECS/IEntity.h"
#include <boost/type_index.hpp>
#include "ECS/ComponentManager.h"

namespace ECS {
    class ComponentFactory {
        class IComponentSet {
        public:
            virtual IComponent *Add(IEntity *ent) = 0;
            virtual ~IComponentSet() = default;
        };

        template<class T>
        class ComponentSet : public IComponentSet {
        public:
            IComponent *Add(IEntity *ent) override
            {
                return ent->AddComponent<T>();
            }
        };

        using CmpSetRegistry = std::unordered_map<std::string, IComponentSet *>;
        CmpSetRegistry cmpSetRegistry;

    public:
        ~ComponentFactory()
        {
            for (auto cr : cmpSetRegistry)
            {
                delete cr.second;
                cr.second = nullptr;
            }
        }

        [[nodiscard]] const CmpSetRegistry &GetRegistry() const noexcept
        {
            return cmpSetRegistry;
        }

        template<class T>
        std::string Register()
        {
            std::string CSID = boost::typeindex::type_id<T>().pretty_name();

            if (cmpSetRegistry.find(CSID) == cmpSetRegistry.end())
                cmpSetRegistry[CSID] = new ComponentSet<T>();

            return CSID;
        }

        IComponent *Add(const std::string &name, IEntity *ent)
        {
            return cmpSetRegistry[name]->Add(ent);
        }
    };
}

#endif
