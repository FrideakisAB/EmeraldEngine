#include "Memory/Allocator/ListLinearAllocator.h"

namespace Memory::Allocator {
    ListLinearAllocator::ListLinearAllocator(size_t allocatorMemSize, const char *user, u32 startCount) :
            allocatorMemSize(allocatorMemSize), IAllocator(0, nullptr), user(user)
    {
        for (u32 i = 0; i < startCount; ++i)
            allocators.push_back(new LinearAllocator(allocatorMemSize, AllocateGlobal(allocatorMemSize, user)));

        lastUsed = allocators.begin();
    }

    ListLinearAllocator::~ListLinearAllocator()
    {
        for (auto &&allocator : allocators)
        {
            FreeGlobal((void *)allocator->GetMemoryAddress0());
            delete allocator;
            allocator = nullptr;
        }

        allocators.clear();
    }

    void *ListLinearAllocator::Allocate(size_t size, u8 alignment)
    {
        void *mem = (*lastUsed)->Allocate(size, alignment);

        if (mem != nullptr)
        {
            memoryUsed += size;
            ++memoryAllocations;
            return mem;
        }

        memoryUsed += allocatorMemSize - (*lastUsed)->GetUsedMemory();

        ++lastUsed;
        if (lastUsed == allocators.end())
        {
            allocators.push_back(new LinearAllocator(allocatorMemSize, AllocateGlobal(allocatorMemSize, user)));
            lastUsed = --allocators.end();
        }

        ++usedAllocators;

        mem = (*lastUsed)->Allocate(size, alignment);

        memoryUsed += size;
        ++memoryAllocations;

        return mem;
    }

    void ListLinearAllocator::Free(void *memory)
    {
        assert(true && "Linear allocators do not support free operations. Use clear instead.");
    }

    void ListLinearAllocator::Clear()
    {
        lastUsed = allocators.begin();
        for (u32 i = 0; i <= usedAllocators; ++i)
        {
            (*lastUsed)->Clear();
            ++lastUsed;
        }

        lastUsed = allocators.begin();

        if (triggerAllocatorsCount != 0 && (allocators.size() - (usedAllocators + 1)) >= triggerAllocatorsCount)
        {
            for (u32 i = 0; i < triggerAllocatorsCount; ++i)
            {
                auto It = --allocators.end();
                GlobalMemoryUser::FreeGlobal((void *)(*It)->GetMemoryAddress0());
                delete *It;
                allocators.erase(It);
            }
        }

        usedAllocators = 0;
        memoryUsed = 0;
        memoryAllocations = 0;
    }

    void ListLinearAllocator::SetShrinkPolicy(u32 triggerAllocatorsCount) const noexcept
    {
        this->triggerAllocatorsCount = triggerAllocatorsCount;
    }
}
