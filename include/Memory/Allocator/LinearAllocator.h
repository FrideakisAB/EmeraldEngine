#ifndef LINEARALLOCATOR_H
#define LINEARALLOCATOR_H

#include "IAllocator.h"

namespace Memory::Allocator {
    class LinearAllocator final : public IAllocator {
    public:
        LinearAllocator(size_t memSize, const void *memory);
        ~LinearAllocator() final;

        [[nodiscard]] void *Allocate(size_t size, u8 alignment) final;
        void Free(void *memory) final;
        void Clear() final;
    };
}

#endif
