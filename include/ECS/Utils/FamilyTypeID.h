#ifndef FAMILYTYPEID_H
#define FAMILYTYPEID_H

#include "ECS/API.h"

namespace ECS::Utils::Internal {
    template<class T>
    class FamilyTypeID {
    private:
        inline static TypeID typeCount{};

    public:
        template<class U>
        [[nodiscard]] static TypeID Get() noexcept
        {
            static const TypeID STATIC_TYPE_ID{typeCount++};
            return STATIC_TYPE_ID;
        }

        [[nodiscard]] static TypeID GetCount() noexcept
        {
            return typeCount;
        }
    };
}

#endif 
