#include "Memory/Allocator/LinearAllocator.h"

namespace Memory::Allocator {
    LinearAllocator::LinearAllocator(size_t memSize, const void *memory) :
            IAllocator(memSize, memory) {}

    LinearAllocator::~LinearAllocator()
    {
        memoryUsed = 0;
        memoryAllocations = 0;
    }

    void *LinearAllocator::Allocate(size_t size, u8 alignment)
    {
        assert(size > 0 && "AllocateGlobal called with memSize = 0");

        union {
            void *asVoidPtr;
            uptr asUptr;
        };

        asVoidPtr = (void *)memoryFirstAddress;
        asUptr += memoryUsed;

        u8 adjustment = GetAdjustment(asVoidPtr, alignment);

        if (memoryUsed + size + adjustment > memorySize)
            return nullptr;

        asUptr += adjustment;

        memoryUsed += size + adjustment;
        ++memoryAllocations;

        return asVoidPtr;
    }

    void LinearAllocator::Free(void *memory)
    {
        assert(false && "Linear allocators do not support free operations. Use clear instead.");
    }

    void LinearAllocator::Clear()
    {
        memoryUsed = 0;
        memoryAllocations = 0;
    }
}
