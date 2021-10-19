#include "Memory/Allocator/IAllocator.h"
#include "Memory/Allocator/LinearAllocator.h"
#include "Memory/Allocator/StackAllocator.h"
#include "Memory/Allocator/PoolAllocator.h"
#include "Memory/Allocator/ListLinearAllocator.h"
#include "Memory/Allocator/MemoryChunkAllocator.h"

TEST_CASE("IAllocator test", "[IAllocator]")
{
    SECTION("Test alignment function")
    {
        REQUIRE( Memory::Allocator::AlignForward(reinterpret_cast<void *>(0x123), 0x4) == reinterpret_cast<void *>(0x124) );
        REQUIRE( Memory::Allocator::AlignForward(reinterpret_cast<void *>(0x123), 0x8) == reinterpret_cast<void *>(0x128) );

        REQUIRE( Memory::Allocator::GetAdjustment(reinterpret_cast<void *>(0x123), 0x4) == 1 );
        REQUIRE( Memory::Allocator::GetAdjustment(reinterpret_cast<void *>(0x123), 0x8) == 5 );

        REQUIRE( Memory::Allocator::GetAdjustment(reinterpret_cast<void *>(0x123), 0x4, 0x3) == 5 );
        REQUIRE( Memory::Allocator::GetAdjustment(reinterpret_cast<void *>(0x123), 0x8, 0x3) == 5 );
    }

    SECTION("IAllocator test")
    {
        class NoAlloc final : public Memory::Allocator::IAllocator {
        public:
            NoAlloc(): Memory::Allocator::IAllocator(32, nullptr) {}

            void *Allocate(size_t size, u8 alignment) final { return nullptr; }
            void Free(void *memory) final {}
            void Clear() final {}
        };

        Memory::Allocator::IAllocator *noAlloc = new NoAlloc();

        REQUIRE( noAlloc->Allocate(123, 65) == nullptr );

        noAlloc->Free(nullptr);
        noAlloc->Clear();

        REQUIRE( noAlloc->GetMemorySize() == 32 );
        REQUIRE( noAlloc->GetAllocationCount() == 0 );
        REQUIRE( noAlloc->GetMemoryAddress0() == nullptr );
        REQUIRE( noAlloc->GetUsedMemory() == 0 );

        delete noAlloc;
    }
}

TEST_CASE("LinearAllocator test", "[LinearAllocator]")
{
    SECTION("Linear allocator logical test")
    {
        Memory::Allocator::LinearAllocator alloc(256, reinterpret_cast<void *>(0x20));

        REQUIRE( alloc.Allocate(32, 1) == reinterpret_cast<void *>(0x20) );
        REQUIRE( alloc.Allocate(32, 1) == reinterpret_cast<void *>(0x40) );
        REQUIRE( alloc.Allocate(32, 1) == reinterpret_cast<void *>(0x60) );

        REQUIRE( alloc.GetAllocationCount() == 3 );

        alloc.Clear();

        REQUIRE( alloc.Allocate(32, 1) == reinterpret_cast<void *>(0x20) );
        REQUIRE( alloc.Allocate(32, 1) == reinterpret_cast<void *>(0x40) );
        REQUIRE( alloc.Allocate(32, 1) == reinterpret_cast<void *>(0x60) );

        REQUIRE( alloc.GetAllocationCount() == 3 );

        alloc.Free(alloc.Allocate(32, 1));
    }

    SECTION("Linear allocator overflow test")
    {
        Memory::Allocator::LinearAllocator alloc(128, reinterpret_cast<void *>(0x20));

        alloc.Allocate(101, 1);

        REQUIRE( alloc.Allocate(32, 8) == nullptr );

        alloc.Clear();

        REQUIRE( alloc.Allocate(32, 8) != nullptr );
    }
}

TEST_CASE("StackAllocator test", "[StackAllocator]")
{
    SECTION("Stack allocator logical test")
    {
        Memory::Allocator::StackAllocator alloc(256, malloc(256));

        REQUIRE( alloc.Allocate(32, 1) != nullptr );
        REQUIRE( alloc.Allocate(256, 1) == nullptr );

        REQUIRE( alloc.GetAllocationCount() == 1 );

        alloc.Clear();

        REQUIRE( alloc.Allocate(255, 1) != nullptr );

        REQUIRE( alloc.GetAllocationCount() == 1 );

        alloc.Clear();

        alloc.Free(alloc.Allocate(32, 1));

        REQUIRE( alloc.GetAllocationCount() == 0 );
    }
}

TEST_CASE("PoolAllocator test", "[PoolAllocator]")
{
    SECTION("Pool allocator logical test")
    {
        Memory::Allocator::PoolAllocator alloc(256, malloc(256), 32, 1);
        void *memory = alloc.Allocate(32, 1);
        REQUIRE( memory != nullptr );

        REQUIRE( alloc.GetAllocationCount() == 1 );

        alloc.Clear();

        REQUIRE( alloc.Allocate(32, 1) == memory );

        alloc.Free(memory);

        REQUIRE( alloc.GetAllocationCount() == 0 );

        free(const_cast<void*>(alloc.GetMemoryAddress0()));
    }

    SECTION("Pool allocator overflow test")
    {
        Memory::Allocator::PoolAllocator alloc(32, malloc(32), 32, 1);

        alloc.Allocate(32, 1);

        REQUIRE( alloc.Allocate(32, 1) == nullptr );

        free(const_cast<void*>(alloc.GetMemoryAddress0()));
    }
}

TEST_CASE("ListLinearAllocator test", "[ListLinearAllocator]")
{
    SECTION("Linear allocator logical test")
    {
        Memory::Allocator::ListLinearAllocator alloc(256);
        void *memory = alloc.Allocate(32, 1);
        REQUIRE( alloc.Allocate(32, 1) == reinterpret_cast<void *>((size_t)memory + 0x20) );
        REQUIRE( alloc.Allocate(32, 1) == reinterpret_cast<void *>((size_t)memory + 0x40) );
        REQUIRE( alloc.Allocate(32, 1) == reinterpret_cast<void *>((size_t)memory + 0x60) );

        REQUIRE( alloc.GetAllocationCount() == 4 );

        alloc.Clear();

        REQUIRE( alloc.Allocate(32, 1) == reinterpret_cast<void *>((size_t)memory + 0x0) );
        REQUIRE( alloc.Allocate(32, 1) == reinterpret_cast<void *>((size_t)memory + 0x20) );
        REQUIRE( alloc.Allocate(32, 1) == reinterpret_cast<void *>((size_t)memory + 0x40) );

        REQUIRE( alloc.GetAllocationCount() == 3 );

        alloc.Free(alloc.Allocate(32, 1));
    }

    SECTION("List allocator functional")
    {
        Memory::Allocator::ListLinearAllocator alloc(16);

        alloc.SetShrinkPolicy(2);

        alloc.Allocate(11, 1);
        alloc.Allocate(11, 1);
        alloc.Allocate(11, 1);
        alloc.Allocate(11, 1);
        alloc.Allocate(11, 1);

        alloc.Clear();

        alloc.Allocate(11, 1);

        alloc.Clear();
    }
}

TEST_CASE("MemoryChunkAllocator test", "[MemoryChunkAllocator]")
{
    logger = new Log();

    SECTION("Base test")
    {
        auto *alloc = new Memory::Allocator::MemoryChunkAllocator<double, 8>();

        for (auto obj : *alloc) {}

        (*(double*)alloc->CreateObject()) = 0;
        (*(double*)alloc->CreateObject()) = 0;
        (*(double*)alloc->CreateObject()) = 0;
        (*(double*)alloc->CreateObject()) = 0;
        (*(double*)alloc->CreateObject()) = 0;

        for (auto obj : *alloc)
            REQUIRE( obj == 0 );

        for (auto it = std::reverse_iterator((*alloc).end()); it != std::reverse_iterator((*alloc).begin()); ++it)
            REQUIRE( *it == 0 );

        auto *testValue = (double *)alloc->CreateObject();
        *testValue = 0;

        for (int i = 0; i < 10; ++i)
            (*(double*)alloc->CreateObject()) = 0;
        for (auto obj : *alloc)
            REQUIRE( obj == 0 );

        alloc->DestroyObject(testValue);
        alloc->DestroyObject(nullptr);

        delete alloc;
    }

    delete logger;
}
