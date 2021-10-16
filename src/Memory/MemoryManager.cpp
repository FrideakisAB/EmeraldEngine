#include "Memory/MemoryManager.h"

#include "Log.h"
#include "Platform.h"
#include <algorithm>

namespace Memory {
    MemoryManager::MemoryManager()
    {
        pendingMemory.clear();
    }

    void MemoryManager::CheckMemoryLeaks()
    {
        if (!pendingMemory.empty())
        {
            for (auto i: pendingMemory)
            {
                logger->Error("%s memory user didn't release allocated memory %llu", i.first, (u64)i.second);
                Free(i.second);
            }
        }
        else
            logger->Info("No memory leaks detected");
    }

    void *MemoryManager::Allocate(size_t memSize, const char *user)
    {
        void *memory = malloc(memSize);
        pendingMemory.emplace_back(user, memory);

        return memory;
    }

    void MemoryManager::Free(void *memory)
    {
        auto it = std::find_if(pendingMemory.begin(), pendingMemory.end(), [memory](std::pair<const char *, void *> val) {
            return (val.second == memory);
        });
        if (it != pendingMemory.end())
        {
            pendingMemory.erase(it);
            free(memory);
        }
    }
}
