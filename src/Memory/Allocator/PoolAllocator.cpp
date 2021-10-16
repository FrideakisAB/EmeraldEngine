#include "Memory/Allocator/PoolAllocator.h"

namespace Memory::Allocator {
    PoolAllocator::PoolAllocator(size_t memSize, const void *memory, size_t objectSize, u8 objectAlignment) :
            IAllocator(memSize, memory), objectSize(objectSize), objectAlignment(objectAlignment)
    {
        assert(objectSize < 8 && "PoolAllocator support object size only above 8 byte.");
        Clear();
    }

    PoolAllocator::~PoolAllocator()
    {
        freeList = nullptr;
    }

    void *PoolAllocator::Allocate(size_t size, u8 alignment)
    {
        assert(size > 0 && "allocate called with memSize = 0.");
        assert(size == objectSize && alignment == objectAlignment);

        if (freeList == nullptr)
            return nullptr;

        void *mem = freeList;

        freeList = (void **)(*freeList);

        memoryUsed += objectSize;
        memoryAllocations++;

        return mem;
    }

    void PoolAllocator::Free(void *memory)
    {
        *((void **)memory) = freeList;

        freeList = (void **)memory;

        memoryUsed -= objectSize;
        memoryAllocations--;
    }

    void PoolAllocator::Clear()
    {
        u8 adjustment = GetAdjustment(memoryFirstAddress, objectAlignment);

        auto numObjects = (size_t)floor((memorySize - adjustment) / objectSize);

        union {
            void *asVoidPtr;
            uptr asUptr;
        };

        asVoidPtr = (void *)memoryFirstAddress;
        asUptr += adjustment;

        freeList = (void **)asVoidPtr;
        void **mem = freeList;

        for (size_t i = 0; i < (numObjects - 1); ++i)
        {
            *mem = (void *)((uptr)mem + objectSize);
            mem = (void **)*mem;
        }

        *mem = nullptr;

        memoryUsed = 0;
        memoryAllocations = 0;
    }
}
