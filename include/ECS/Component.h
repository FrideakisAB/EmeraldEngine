#ifndef COMPONENT_H
#define COMPONENT_H

#include "ECS/API.h"
#include "ECS/IComponent.h"
#include <boost/type_index.hpp>
#include "ECS/Utils/FamilyTypeID.h"

namespace ECS {
    template<class T>
    class Component : public IComponent {
    public:
        static const ComponentTypeId STATIC_COMPONENT_TYPE_ID;

        ~Component() override = default;

        [[nodiscard]] json SerializeObj() const override
        {
            json ret;
            ret["cmpName"] = boost::typeindex::type_id<T>().pretty_name();
            return ret;
        }

        [[nodiscard]] inline ComponentTypeId GetStaticComponentTypeId() const
        {
            return STATIC_COMPONENT_TYPE_ID;
        }
    };

    template<class T>
    const ComponentTypeId Component<T>::STATIC_COMPONENT_TYPE_ID = Utils::Internal::FamilyTypeID<IComponent>::Get<T>();
}

#endif
