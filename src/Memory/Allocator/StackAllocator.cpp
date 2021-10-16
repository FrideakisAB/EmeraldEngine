#include "Memory/Allocator/StackAllocator.h"

namespace Memory::Allocator {
    StackAllocator::StackAllocator(size_t memSize, const void *memory) :
            IAllocator(memSize, memory) {}

    StackAllocator::~StackAllocator()
    {
        Clear();
    }

    void *StackAllocator::Allocate(size_t size, u8 alignment)
    {
        assert(size > 0 && "allocate called with memSize = 0.");

        union {
            void *asVoidPtr;
            uptr asUptr;
            AllocMetaInfo *asMeta;
        };

        asVoidPtr = (void *)memoryFirstAddress;

        asUptr += memoryUsed;

        u8 adjustment = GetAdjustment(asVoidPtr, alignment, sizeof(AllocMetaInfo));

        if (memoryUsed + size + adjustment > memorySize)
            return nullptr;

        asMeta->adjustment = adjustment;

        asUptr += adjustment;

        memoryUsed += size + adjustment;
        memoryAllocations++;

        return asVoidPtr;
    }

    void StackAllocator::Free(void *memory)
    {
        union {
            void *asVoidPtr;
            uptr asUptr;
            AllocMetaInfo *asMeta;
        };

        asVoidPtr = memory;

        asUptr -= sizeof(AllocMetaInfo);

        memoryAllocations--;
    }

    void StackAllocator::Clear()
    {
        memoryUsed = 0;
        memoryAllocations = 0;
    }
}
