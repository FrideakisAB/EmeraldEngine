#include "Memory/GlobalMemoryUser.h"

#include "Memory/MemoryManager.h"

namespace Memory {
    const void *GlobalMemoryUser::AllocateGlobal(size_t memSize, const char *user)
    {
        return MemoryManager::Get().Allocate(memSize, user);
    }

    void GlobalMemoryUser::FreeGlobal(void *memory)
    {
        MemoryManager::Get().Free(memory);
    }
}
