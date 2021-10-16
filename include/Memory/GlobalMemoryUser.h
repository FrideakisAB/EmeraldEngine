#ifndef GLOBALMEMORYUSER_H
#define GLOBALMEMORYUSER_H

#include <cstdlib>

namespace Memory {
    class GlobalMemoryUser {
    public:
        virtual ~GlobalMemoryUser() = default;

        static const void *AllocateGlobal(size_t memSize, const char *user = nullptr);
        static void FreeGlobal(void *memory);
    };
}

#endif
