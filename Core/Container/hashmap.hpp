#include <initializer_list>
#include <type_traits>

#include "../Memory/memory.hpp"
#include "../Common/common.hpp"
#include "../Container/contiguous.hpp"
#include "../String/string.hpp"

#define HASHMAP_DEFAULT_LOAD_FACTOR 0.7f

// Original location:
// https://github.com/majek/csiphash/1

#if defined(_WIN32)
    #define _le64toh(x) ((u64)(x))
#elif defined(__APPLE__)
    #include <libkern/OSByteOrder.h>
    #define _le64toh(x) OSSwapLittleToHostInt64(x)
#else
    #if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
        #include <sys/endian.h>
    #else
        #include <endian.h>
    #endif

    #if defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && __BYTE_ORDER == __LITTLE_ENDIAN   
        #define _le64toh(x) ((u64)(x))
    #else
        #define _le64toh(x) le64toh(x)
    #endif
#endif

namespace ION::Container {
    typedef u64(HashFunction)(const void*, byte_t);
    typedef bool(EqualFunction)(const void*, byte_t, const void*, byte_t);

    #define ROTATE(x, b) (u64)( ((x) << (b)) | ( (x) >> (64 - (b))) )

    #define HALF_ROUND(a,b,c,d,s,t)	\
        a += b; c += d;			    \
        b = ROTATE(b, s) ^ a;		\
        d = ROTATE(d, t) ^ c;		\
        a = ROTATE(a, 32);          \

    #define DOUBLE_ROUND(v0,v1,v2,v3)  \
        HALF_ROUND(v0,v1,v2,v3,13,16); \
        HALF_ROUND(v2,v1,v0,v3,17,21); \
        HALF_ROUND(v0,v1,v2,v3,13,16); \
        HALF_ROUND(v2,v1,v0,v3,17,21)

    u64 siphash24(const void* source, u64 source_size) {
        const char key[16] = {
            0x00, 0x01, 0x02, 0x03,
            0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B,
            0x0C, 0x0D, 0x0E, 0x0F
        };

        const u64 *_key = (const u64*)key;
        u64 k0 = _le64toh(_key[0]);
        u64 k1 = _le64toh(_key[1]);
        u64 b = source_size << 56;
        const u64* in = (const u64*)source;

        u64 v0 = k0 ^ 0x736f6d6570736575ULL;
        u64 v1 = k1 ^ 0x646f72616e646f6dULL;
        u64 v2 = k0 ^ 0x6c7967656e657261ULL;
        u64 v3 = k1 ^ 0x7465646279746573ULL;

        while (source_size >= 8) {
            u64 mi = _le64toh(*in);
            in += 1; source_size -= 8;
            v3 ^= mi;
            DOUBLE_ROUND(v0,v1,v2,v3);
            v0 ^= mi;
        }

        u64 t = 0; 
        u8* pt = (u8*)&t; 
        const u8* m = (const u8*)in;
        switch (source_size) {
            case 7: pt[6] = m[6];
            case 6: pt[5] = m[5];
            case 5: pt[4] = m[4];
            case 4: *((u32*)&pt[0]) = *((u32*)&m[0]); break;
            case 3: pt[2] = m[2];
            case 2: pt[1] = m[1];
            case 1: pt[0] = m[0];
        }
        b |= _le64toh(t);

        v3 ^= b;
        DOUBLE_ROUND(v0,v1,v2,v3);
        v0 ^= b; v2 ^= 0xff;
        DOUBLE_ROUND(v0,v1,v2,v3);
        DOUBLE_ROUND(v0,v1,v2,v3);

        u64 ret = (v0 ^ v1) ^ (v2 ^ v3);
        return ret;
    }

    template <typename K, typename V>
    struct Hashmap {
        struct HashmapEntry {
            K key;
            V value;
            bool filled;
            bool dead;
        };

        Hashmap(u64 capacity = 1, ION::Memory::Allocator allocator = ION::Memory::Allocator::libc()) {
            constexpr bool supported_key_type = std::is_trivially_copyable_v<K> || std::is_same_v<K, char*> || std::is_same_v<K, const char*>;
            STATIC_ASSERT(supported_key_type);

            this->m_count = 0;
            this->m_capacity = capacity;

            this->m_entries = nullptr;
            this->m_allocator = allocator;
            this->m_hash_func = siphash24;
            this->m_equal_func = ION::Memory::equal;
        }

        Hashmap(HashFunction* hash_func, EqualFunction* equal_func, u64 capacity = 1, ION::Memory::Allocator allocator = ION::Memory::Allocator::libc()) {
            constexpr bool supported_key_type = std::is_trivially_copyable_v<K> || std::is_same_v<K, char*> || std::is_same_v<K, const char*>;
            STATIC_ASSERT(supported_key_type);

            this->m_count = 0;
            this->m_capacity = capacity;

            this->m_entries = nullptr;
            this->m_allocator = allocator;
            this->m_hash_func = hash_func;
            this->m_equal_func = equal_func;
        }

        void put(K key, V value) {
            if (this->load_factor() >= HASHMAP_DEFAULT_LOAD_FACTOR) {
                this->grow_and_rehash();
            }

            u64 hash = this->hash_func(key, sizeof(K));
            u64 index = this->resolve_collision(key, hash % this->m_entries.capacity());

            HashmapEntry* entry = &this->m_entries[index];
            if (!entry->filled || entry->dead) {
                this->m_count += 1;
            }

            entry->key = key;
            entry->value = value;
            entry->filled = true;
        }

        bool has(K key) {
            u64 hash = this->hash_func(&key, sizeof(K));
            u64 index = this->resolve_collision(key, hash % this->m_entries.capacity());
            HashmapEntry* entry = &this->m_entries[index];

            return entry->filled && !entry->dead;
        }

        K get(K key) {
            u64 hash = this->hash_func(&key, sizeof(K));
            u64 index = this->resolve_collision(key, hash % this->m_entries.capacity());
            HashmapEntry* entry = &this->m_entries[index];
            RUNTIME_ASSERT_MSG(this->has(key), "Key doesn't exist\n");

            return entry->value;
        }

        V remove(K key) {
            u64 hash = this->hash_func(&key, sizeof(K));
            u64 index = this->resolve_collision(key, hash % this->m_entries.capacity());
            HashmapEntry* entry = &this->m_entries[index];
            RUNTIME_ASSERT_MSG(this->has(key), "Key doesn't exist\n");

            entry->dead = true;

            return entry->value;
        }
    private:
        V* m_entries;
        u64 m_count = 0;
        u64 m_capacity = 0;
        u64 m_dead_count = 0;
        HashFunction* m_hash_func = nullptr;
        EqualFunction* m_equal_func = nullptr;
        ION::Memory::Allocator m_allocator = ION::Memory::Allocator::invalid();

        void grow_and_rehash() {
            byte_t old_allocation_size = (this->m_capacity * sizeof(V));
            this->m_capacity *= 2;
            byte_t new_allocation_size = (this->m_capacity * sizeof(V));

            this->m_data = (V*)this->m_allocator.realloc(this->m_data, old_allocation_size, new_allocation_size);

            /*
            u64 old_capacity = meta->capacity;
            meta->capacity *= 2;
            void* new_entries = ckg_alloc(meta->capacity * meta->entry_size);
            ckg_memory_copy((u8*)map + meta->entries_offset, sizeof(void*), &new_entries, sizeof(void*));

            this->m_entries = Vector<V>();

            // rehash
            for (u64 i = 0; i < old_capacity; i++) {
                HashmapEntry* entry = &this->m_entries[i];
                if (!entry->filled) {
                    continue;
                }

                u64 hash = meta->hash_fn(entry_key, meta->key_size);
                u64 index = this->resolve_collision((u8*)map, entry_key, hash % this->capacity);

                u8* new_entry = (u8*)new_entries + (real_index * meta->entry_size);
                HashmapEntry* new_entry = new_entries[real_index]
                ckg_memory_copy(new_entry, sizeof(HashmapEntry), entry, sizeof(HashmapEntry));
            }

            ckg_free(entries_base_address);
            */
        }

        u64 resolve_collision(K key, u64 inital_hash_index) {
            u64 cannonical_hash_index = inital_hash_index;

            while (true) {
                HashmapEntry* entry = &this->m_entries[cannonical_hash_index];
                if (!entry->filled) {
                    break;
                }

                bool equality_match = this->equal_func(key, sizeof(K), &entry->key, sizeof(K));
                if (equality_match) {
                    break;
                }

                cannonical_hash_index++;
                cannonical_hash_index = cannonical_hash_index % this->m_capacity;
            }

            return cannonical_hash_index;
        }

        float load_factor() {
            return (float)(this->dead_count + this->m_entries.count()) / (float)this->m_entries.capacity();
        }
    };
}