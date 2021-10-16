#ifndef IALLOCATOR_H
#define IALLOCATOR_H

#include "Platform.h"

namespace Memory::Allocator {
    static inline void *AlignForward(void *address, u8 alignment)
    {
        return (void *)((reinterpret_cast<uptr>(address) + static_cast<uptr>(alignment - 1)) & static_cast<uptr>(~(alignment - 1)));
    }

    static inline u8 GetAdjustment(const void *address, u8 alignment)
    {
        u8 adjustment = alignment - (reinterpret_cast<uptr>(address) & static_cast<uptr>(alignment - 1));

        return adjustment == alignment ? 0 : adjustment;
    }

    static inline u8 GetAdjustment(const void *address, u8 alignment, u8 extra)
    {
        u8 adjustment = GetAdjustment(address, alignment);
        u8 neededSpace = extra;

        if (adjustment < neededSpace)
        {
            neededSpace -= adjustment;

            adjustment += alignment * (neededSpace / alignment);

            if (neededSpace % alignment > 0)
                adjustment += alignment;
        }

        return adjustment;
    }

    class IAllocator {
    protected:
        const size_t memorySize;
        const void *memoryFirstAddress;

        size_t memoryUsed = 0;
        u64 memoryAllocations = 0;

    public:
        IAllocator(size_t memSize, const void *memory);
        virtual ~IAllocator() = default;

        [[nodiscard]] virtual void *Allocate(size_t size, u8 alignment) = 0;
        virtual void Free(void *memory) = 0;
        virtual void Clear() = 0;

        [[nodiscard]] inline size_t GetMemorySize() const
        {
            return memorySize;
        }

        [[nodiscard]] inline const void *GetMemoryAddress0() const
        {
            return memoryFirstAddress;
        }

        [[nodiscard]] inline size_t GetUsedMemory() const
        {
            return memoryUsed;
        }

        [[nodiscard]] inline u64 GetAllocationCount() const
        {
            return memoryAllocations;
        }
    };
}

#endif
