#include "Memory/Allocator/IAllocator.h"

namespace Memory::Allocator {
    IAllocator::IAllocator(size_t memSize, const void *memory) :
            memorySize(memSize), memoryFirstAddress(memory) {}
}
