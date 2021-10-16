#ifndef LISTLINEARALLOC_H
#define LISTLINEARALLOC_H

#include <list>
#include "LinearAllocator.h"
#include "Memory/GlobalMemoryUser.h"

namespace Memory::Allocator {
    class ListLinearAllocator final : public IAllocator, private ::Memory::GlobalMemoryUser {
        using AllocatorsList = std::list<LinearAllocator *>;

    private:
        AllocatorsList allocators;
        AllocatorsList::iterator lastUsed;

        mutable u32 triggerAllocatorsCount = 0;

        u32 usedAllocators = 0;
        size_t allocatorMemSize;
        const char *user;

    public:
        explicit ListLinearAllocator(size_t allocatorMemSize, const char *user = nullptr, u32 startCount = 2);
        ~ListLinearAllocator() final;

        [[nodiscard]] void *Allocate(size_t size, u8 alignment) final;
        void Free(void *memory) final;
        void Clear() final;

        void SetShrinkPolicy(u32 triggerAllocatorsCount) const noexcept;
    };
}

#endif
