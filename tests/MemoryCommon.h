#include "Memory/MemoryManager.h"
#include "Memory/GlobalMemoryUser.h"

#include "Log.h"

TEST_CASE("MemoryManager interface and work test", "[MemoryManager]")
{
    static bool isErrorGenerate;

    class CheckerLog final : public Log {
    protected:
        void Write(const std::string &msg) final
        {
            if (msg.find("No memory leaks") == std::string::npos)
                isErrorGenerate = true;
            else
                isErrorGenerate = false;
        }
    };

    logger = new CheckerLog();

    Memory::MemoryManager::Get().CheckMemoryLeaks();
    isErrorGenerate = false;

    SECTION("Interface test")
    {
        void *memory = Memory::MemoryManager::Get().Allocate(8, "Test");
        REQUIRE( memory != nullptr );

        Memory::MemoryManager::Get().Free(memory);

        Memory::MemoryManager::Get().CheckMemoryLeaks();
        REQUIRE( isErrorGenerate == false );
    }

    SECTION("Work test")
    {
        void *memory = Memory::MemoryManager::Get().Allocate(8, "Test");
        REQUIRE( memory != nullptr );

        Memory::MemoryManager::Get().CheckMemoryLeaks();
        REQUIRE( isErrorGenerate == true );

        Memory::MemoryManager::Get().CheckMemoryLeaks();
        REQUIRE( isErrorGenerate == false );
    }

    delete logger;
}

TEST_CASE("GlobalMemoryUser work test", "[GlobalMemoryUser]")
{
    static bool isErrorGenerate = false;

    logger = new Log();

    class NewUser : protected Memory::GlobalMemoryUser {
        const void *ptr{};
    public:
        bool testAlloc() { ptr = AllocateGlobal(8, "TestUser"); return ptr != nullptr; }
        void testFree() { FreeGlobal(const_cast<void*>(ptr)); }
    };

    SECTION("Work test")
    {
        NewUser newUser{};

        REQUIRE( newUser.testAlloc() == true );

        newUser.testFree();
    }

    delete logger;
}
