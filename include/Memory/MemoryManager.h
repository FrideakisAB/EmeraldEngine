#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <vector>
#include "Utils/Singleton.h"

namespace Memory {
    class MemoryManager : public Singleton<MemoryManager> {
    private:
        std::vector<std::pair<const char *, void *>> pendingMemory;

    public:
        MemoryManager();

        void *Allocate(size_t memSize, const char *user = nullptr);
        void Free(void *memory);
        void CheckMemoryLeaks();
    };
}

#endif
