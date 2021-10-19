#ifndef MEMORYCHUNKALLOCATOR_H
#define MEMORYCHUNKALLOCATOR_H

#include <vector>
#include <list>
#include "PoolAllocator.h"
#include "Memory/GlobalMemoryUser.h"

namespace Memory::Allocator {
    template<class OBJECT_TYPE, size_t MAX_CHUNK_OBJECTS>
    class MemoryChunkAllocator : protected ::Memory::GlobalMemoryUser {
    private:
        static const size_t maxObjects = MAX_CHUNK_OBJECTS;
        static const size_t allocSize = (sizeof(OBJECT_TYPE) + alignof(OBJECT_TYPE)) * maxObjects;
        const char *allocatorTag;

    public:
        using Allocator = Memory::Allocator::PoolAllocator;
        using ObjectList = std::list<OBJECT_TYPE *>;

        class MemoryChunk {
        public:
            Allocator *allocator;
            ObjectList objects;

            uptr chunkStart;
            uptr chunkEnd;

            explicit MemoryChunk(Allocator *allocator) :
                    allocator(allocator)
            {
                chunkStart = reinterpret_cast<uptr>(allocator->GetMemoryAddress0());
                chunkEnd = chunkStart + allocSize;
                objects.clear();
            }
        };

        using MemoryChunks = std::list<MemoryChunk *>;

        class iterator {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = OBJECT_TYPE;
            using difference_type = std::ptrdiff_t;
            using pointer = OBJECT_TYPE*;
            using reference = OBJECT_TYPE&;

        private:
            typename MemoryChunks::iterator currentChunk;
            typename MemoryChunks::iterator anBegin;
            typename MemoryChunks::iterator anEnd;
            typename ObjectList::iterator currentObject;

        public:
            iterator(typename MemoryChunks::iterator begin, typename MemoryChunks::iterator end, bool isEnd) :
                    currentChunk(begin), anBegin(begin), anEnd(end)
            {
                if (begin != end && !isEnd)
                {
                    assert((*currentChunk) != nullptr);
                    currentObject = (*currentChunk)->objects.begin();

                    if (currentObject == (*currentChunk)->objects.end())
                        currentChunk = anEnd;
                }
                else
                {
                    currentChunk = end;
                    currentObject = (*std::prev(anEnd))->objects.end();
                }
            }

            inline iterator &operator++()
            {
                ++currentObject;

                if (currentObject == (*currentChunk)->objects.end())
                {
                    ++currentChunk;

                    if (currentChunk != anEnd)
                    {
                        assert((*currentChunk) != nullptr);
                        currentObject = (*currentChunk)->objects.begin();
                    }
                }

                return *this;
            }

            inline iterator &operator--()
            {
                if (currentChunk == anEnd || currentObject == (*currentChunk)->objects.begin())
                {
                    while (currentChunk != anBegin)
                    {
                        --currentChunk;

                        assert((*currentChunk) != nullptr);
                        if ((*currentChunk)->objects.begin() != (*currentChunk)->objects.end())
                        {
                            currentObject = (*currentChunk)->objects.end();
                            break;
                        }
                    }
                }

                --currentObject;

                return *this;
            }

            [[nodiscard]] inline reference operator*() const { return *(*currentObject); }
            [[nodiscard]] inline pointer operator->() const { return *currentObject; }

            [[nodiscard]] inline bool operator==(const iterator &other) const
            {
                return ((currentChunk == other.currentChunk) && (currentObject == other.currentObject));
            }

            [[nodiscard]] inline bool operator!=(const iterator &other) const
            {
                return (!((currentChunk == other.currentChunk) && (currentObject == other.currentObject)));
            }
        };

    protected:
        MemoryChunks chunks;

    public:
        explicit MemoryChunkAllocator(const char *allocatorTag = nullptr) :
                allocatorTag(allocatorTag)
        {
            auto *allocator = new Allocator(allocSize, AllocateGlobal(allocSize, allocatorTag), sizeof(OBJECT_TYPE), alignof(OBJECT_TYPE));
            chunks.push_back(new MemoryChunk(allocator));
        }

        ~MemoryChunkAllocator() override
        {
            for (auto chunk : chunks)
            {
                for (auto obj : chunk->objects)
                    ((OBJECT_TYPE *) obj)->~OBJECT_TYPE();

                chunk->objects.clear();

                FreeGlobal((void *)chunk->allocator->GetMemoryAddress0());
                delete chunk->allocator;
                chunk->allocator = nullptr;

                delete chunk;
                chunk = nullptr;
            }
        }

        [[nodiscard]] void *CreateObject()
        {
            void *slot = nullptr;

            for (auto chunk : chunks)
            {
                if (chunk->objects.size() > maxObjects)
                    continue;

                slot = chunk->allocator->Allocate(sizeof(OBJECT_TYPE), alignof(OBJECT_TYPE));
                if (slot != nullptr)
                {
                    chunk->objects.push_back((OBJECT_TYPE *)slot);
                    break;
                }
            }

            if (slot == nullptr)
            {
                auto *allocator = new Allocator(allocSize, AllocateGlobal(allocSize, allocatorTag), sizeof(OBJECT_TYPE), alignof(OBJECT_TYPE));
                auto *newChunk = new MemoryChunk(allocator);

                chunks.push_front(newChunk);

                slot = newChunk->allocator->Allocate(sizeof(OBJECT_TYPE), alignof(OBJECT_TYPE));

                assert(slot != nullptr && "Unable to create new object. Out of memory?!");
                newChunk->objects.clear();
                newChunk->objects.push_back((OBJECT_TYPE *)slot);
            }

            return slot;
        }

        void DestroyObject(void *object)
        {
            uptr adr = reinterpret_cast<uptr>(object);

            auto chunk = std::find_if(chunks.begin(), chunks.end(), [adr](MemoryChunk *chunk) {
                return (chunk->chunkStart <= adr && adr < chunk->chunkEnd);
            });

            if (chunk == chunks.end())
            {
                logger->Error("Failed to delete object. Memory corruption!");
                return;
            }

            (*chunk)->objects.remove((OBJECT_TYPE *) object);
            (*chunk)->allocator->Free(object);
        }

        [[nodiscard]] inline iterator begin() { return iterator(chunks.begin(), chunks.end(), false); }
        [[nodiscard]] inline iterator end() { return iterator(chunks.begin(), chunks.end(), true); }
    };
}

#endif
