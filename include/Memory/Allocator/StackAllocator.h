#ifndef STACKALLOC_H
#define STACKALLOC_H

#include "IAllocator.h"

namespace Memory::Allocator {
    class StackAllocator final : public IAllocator {
        struct AllocMetaInfo {
            u8 adjustment;
        };

    public:
        StackAllocator(size_t memSize, const void *memory);
        ~StackAllocator() final;

        [[nodiscard]] void *Allocate(size_t size, u8 alignment) final;
        void Free(void *memory) final;
        void Clear() final;
    };
}

#endif
