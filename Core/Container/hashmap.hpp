#include <initializer_list>
#include <type_traits>

#include "../Memory/memory.hpp"
#include "../Common/common.hpp"
#include "../Container/contiguous.hpp"
#include "../String/string.hpp"
#include "../Hashing/hashing.hpp"

#define HASHMAP_DEFAULT_LOAD_FACTOR 0.7f

namespace ION::Container {
    typedef u64(HashFunction)(const void*, byte_t);
    typedef bool(EqualFunction)(const void*, byte_t, const void*, byte_t);

    template <typename K, typename V>
    struct Hashmap {
        struct HashmapEntry {
            K key;
            V value;
            bool filled;
            bool dead;
        };

        Hashmap(u64 capacity = 1, ION::Memory::Allocator allocator = ION::Memory::Allocator::libc()) {
            constexpr bool is_trivially_copyable_non_pointer_v = std::is_trivially_copyable_v<K> && !std::is_pointer_v<K>;
            constexpr bool cstring_key_type = std::is_same_v<K, char*> || std::is_same_v<K, const char*>;
            // constexpr bool string_view_key_type = std::is_same_v<K, ION::Container::View<char>>;
            STATIC_ASSERT(is_trivially_copyable_non_pointer_v || cstring_key_type);

            this->m_count = 0;
            this->m_capacity = capacity;

            this->m_allocator = allocator;
            this->m_entries = (HashmapEntry*)this->m_allocator.malloc(this->m_capacity * sizeof(HashmapEntry));

            if constexpr (is_trivially_copyable_non_pointer_v) {
                this->m_hash_func = ION::Hashing::siphash24;
                this->m_equal_func = ION::Memory::equal;
            } else if constexpr (cstring_key_type) {
                this->m_hash_func = ION::Hashing::cstring_hash;
                this->m_equal_func = ION::Hashing::cstring_equality;
            }

            /*
            else if constexpr (string_view_key_type) {
                this->m_hash_func = ION::Hashing::string_view_hash;
                this->m_equal_func = ION::Hashing::string_view_equality;
            }
            */
        }

        Hashmap(HashFunction* hash_func, EqualFunction* equal_func, u64 capacity = 1, ION::Memory::Allocator allocator = ION::Memory::Allocator::libc()) {
            constexpr bool supported_key_type = std::is_trivially_copyable_v<K> || std::is_same_v<K, char*> || std::is_same_v<K, const char*>;
            STATIC_ASSERT(supported_key_type);

            this->m_count = 0;
            this->m_capacity = capacity;

            this->m_allocator = allocator;
            this->m_entries = (HashmapEntry*)this->m_allocator.malloc(this->m_capacity * sizeof(HashmapEntry));

            this->m_hash_func = hash_func;
            this->m_equal_func = equal_func;
        }

        void put(K key, V value) {
            if (this->load_factor() >= HASHMAP_DEFAULT_LOAD_FACTOR) {
                this->grow_and_rehash();
            }

            u64 hash = this->m_hash_func(&key, sizeof(K));
            s64 index = this->resolve_collision(key, hash % this->m_capacity);
            RUNTIME_ASSERT(index != -1);

            HashmapEntry* entry = &this->m_entries[index];
            if (!entry->filled || entry->dead) {
                this->m_count += 1;
            }

            entry->key = key;
            entry->value = value;
            entry->filled = true;
        }

        bool has(K key) {
            u64 hash = this->m_hash_func(&key, sizeof(K));
            s64 index = this->resolve_collision(key, hash % this->m_capacity);
            if (index == -1) {
                return false;
            }

            HashmapEntry* entry = &this->m_entries[index];

            return entry->filled && !entry->dead;
        }

        V get(K key) {
            RUNTIME_ASSERT_MSG(this->has(key), "Key doesn't exist\n");

            u64 hash = this->m_hash_func(&key, sizeof(K));
            s64 index = this->resolve_collision(key, hash % this->m_capacity);
            RUNTIME_ASSERT(index != -1);
            
            HashmapEntry* entry = &this->m_entries[index];


            return entry->value;
        }

        V remove(K key) {
            RUNTIME_ASSERT_MSG(this->has(key), "Key doesn't exist\n");

            u64 hash = this->m_hash_func(&key, sizeof(K));
            s64 index = this->resolve_collision(key, hash % this->m_capacity);
            RUNTIME_ASSERT(index != -1);
            
            HashmapEntry* entry = &this->m_entries[index];

            this->m_count -= 1;
            this->m_dead_count += 1;
            entry->dead = true;

            return entry->value;
        }
    private:
        u64 m_count = 0;
        u64 m_capacity = 0;
        u64 m_dead_count = 0;
        HashmapEntry* m_entries = nullptr;
        HashFunction* m_hash_func = nullptr;
        EqualFunction* m_equal_func = nullptr;
        ION::Memory::Allocator m_allocator = ION::Memory::Allocator::invalid();
        bool is_key_pointer_type = std::is_pointer_v<K>;

        void grow_and_rehash() {
            u64 old_capacity = this->m_capacity;
            HashmapEntry* old_entries = this->m_entries;

            this->m_capacity *= 2;
            byte_t new_allocation_size = (this->m_capacity * sizeof(HashmapEntry));
            this->m_entries = (HashmapEntry*)this->m_allocator.malloc(new_allocation_size);

            // rehash
            for (u64 i = 0; i < old_capacity; i++) {
                HashmapEntry old_entry = old_entries[i];
                if (!old_entry.filled || old_entry.dead) {
                    continue;
                }

                u64 hash = this->m_hash_func(&old_entry.key, sizeof(K));
                s64 index = this->resolve_collision(old_entry.key, hash % this->m_capacity);
                RUNTIME_ASSERT(index != -1);


                this->m_entries[index] = old_entry;
            }

            this->m_allocator.free(old_entries);
        }

        s64 resolve_collision(K key, u64 inital_hash_index) {
            s64 cannonical_hash_index = inital_hash_index;

            u64 visited_count = 0;
            while (true) {
                HashmapEntry* entry = &this->m_entries[cannonical_hash_index];
                if (!entry->filled) {
                    break;
                }

                bool equality_match = this->m_equal_func(&key, sizeof(K), &entry->key, sizeof(K));
                if (equality_match) {
                    break;
                }

                if (visited_count > this->m_capacity) {
                    return -1;
                }

                visited_count += 1;
                cannonical_hash_index += 1;
                cannonical_hash_index = cannonical_hash_index % this->m_capacity;
            }

            return cannonical_hash_index;
        }

        float load_factor() {
            return (float)(this->m_dead_count + this->m_count) / (float)this->m_capacity;
        }
    };
}