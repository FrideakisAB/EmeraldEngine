#ifndef POOLALLOCATOR_H
#define POOLALLOCATOR_H

#include "IAllocator.h"

namespace Memory::Allocator {
    class PoolAllocator final : public IAllocator {
    private:
        const size_t objectSize;
        const u8 objectAlignment;
        void **freeList = nullptr;

    public:
        PoolAllocator(size_t memSize, const void *memory, size_t objectSize, u8 objectAlignment);
        ~PoolAllocator() final;

        [[nodiscard]] void *Allocate(size_t size, u8 alignment) final;
        void Free(void *memory) final;
        void Clear() final;
    };
}

#endif
