#include <initializer_list>
#include <type_traits>

#include "../Memory/memory.hpp"
#include "../Common/common.hpp"
#include "../DataStructure/view.hpp"
#include "../String/string.hpp"
#include "../Hashing/hashing.hpp"

#define HASHMAP_DEFAULT_LOAD_FACTOR 0.7f

namespace DS {
    typedef u64(HashFunction)(const void*, byte_t);
    typedef bool(EqualFunction)(const void*, byte_t, const void*, byte_t);

    template <typename K, typename V>
    struct Hashmap {
        struct HashmapEntry {
            K key;
            V value;
            bool filled = false;
            bool dead = false;
        };

        struct InitPair {
            K key;
            V value;
        };

        Hashmap() = default;

        Hashmap(Memory::BaseAllocator* allocator, u64 capacity = 1) : m_allocator(allocator) {
            constexpr bool key_is_trivial = std::is_trivially_copyable_v<K>;
            constexpr bool key_is_pointer = std::is_pointer_v<K>;
            constexpr bool key_is_cstring = std::is_same_v<K, char*> || std::is_same_v<K, const char*>;
            constexpr bool key_is_string_view = std::is_same_v<K, DS::View<char>> || std::is_same_v<K, DS::View<const char>>;
            
            STATIC_ASSERT((key_is_trivial && !key_is_pointer) || key_is_cstring || key_is_string_view);

            this->m_count = 0;
            this->m_capacity = capacity;
            this->m_entries = (HashmapEntry*)this->m_allocator->malloc(this->m_capacity * sizeof(HashmapEntry));

            if constexpr (key_is_trivial && !key_is_pointer) {
                this->m_hash_func = Hashing::siphash24;
                this->m_equal_func = Memory::equal;
            } else if constexpr (key_is_cstring) {
                this->m_hash_func = Hashing::cstring_hash;
                this->m_equal_func = Hashing::cstring_equality;
            } else if constexpr (key_is_string_view) {
                this->m_hash_func = Hashing::string_view_hash;
                this->m_equal_func = Hashing::string_view_equality;
            }
        }

        Hashmap(std::initializer_list<InitPair> list, Memory::BaseAllocator* allocator = &Memory::global_general_allocator) : m_allocator(allocator) {
            constexpr bool key_is_trivial = std::is_trivially_copyable_v<K>;
            constexpr bool key_is_pointer = std::is_pointer_v<K>;
            constexpr bool key_is_cstring = std::is_same_v<K, char*> || std::is_same_v<K, const char*>;
            constexpr bool key_is_string_view = std::is_same_v<K, DS::View<char>> || std::is_same_v<K, DS::View<const char>>;

            STATIC_ASSERT((key_is_trivial && !key_is_pointer) || key_is_cstring || key_is_string_view);

            this->m_count = list.size();
            this->m_capacity = this->m_count * 2;
            this->m_entries = (HashmapEntry*)this->m_allocator->malloc(this->m_capacity * sizeof(HashmapEntry));

            if constexpr (key_is_trivial && !key_is_pointer) {
                this->m_hash_func = Hashing::siphash24;
                this->m_equal_func = Memory::equal;
            } else if constexpr (key_is_cstring) {
                this->m_hash_func = Hashing::cstring_hash;
                this->m_equal_func = Hashing::cstring_equality;
            } else if constexpr (key_is_string_view) {
                this->m_hash_func = Hashing::string_view_hash;
                this->m_equal_func = Hashing::string_view_equality;
            }

            for (InitPair pair : list) {
                this->put(pair.key, pair.value);
            }
        }

        Hashmap(Memory::BaseAllocator* allocator, HashFunction* hash_func, EqualFunction* equal_func, u64 capacity = 1) : m_allocator(allocator) {
            constexpr bool key_is_trivial = std::is_trivially_copyable_v<K>;
            constexpr bool key_is_pointer = std::is_pointer_v<K>;
            STATIC_ASSERT(key_is_trivial && !key_is_pointer);

            this->m_count = 0;
            this->m_capacity = capacity;

            this->m_allocator = allocator;
            this->m_entries = (HashmapEntry*)this->m_allocator->malloc(this->m_capacity * sizeof(HashmapEntry));

            this->m_hash_func = hash_func;
            this->m_equal_func = equal_func;
        }

        Hashmap& operator=(const Hashmap& other) {
            if (this != &other) {
                if (!this->m_allocator) {
                    this->m_allocator = other.m_allocator;
                }
                
                if (this->m_entries) {
                    this->m_allocator->free(m_entries);
                }

                this->m_count = other.m_count;
                this->m_capacity = other.m_capacity;
                this->m_dead_count = other.m_dead_count;
                this->m_hash_func = other.m_hash_func;
                this->m_equal_func = other.m_equal_func;

                this->m_entries = (HashmapEntry*)m_allocator->malloc(m_capacity * sizeof(HashmapEntry));
            }

            return *this;
        }

        void put(K key, V value) {
            if (this->load_factor() >= HASHMAP_DEFAULT_LOAD_FACTOR) {
                this->grow_and_rehash();
            }

            u64 hash = this->safe_hash(key);
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
            u64 hash = this->safe_hash(key);
            s64 index = this->resolve_collision(key, hash % this->m_capacity);
            if (index == -1) {
                return false;
            }

            HashmapEntry* entry = &this->m_entries[index];

            return entry->filled && !entry->dead;
        }

        V get(K key) {
            RUNTIME_ASSERT_MSG(this->has(key), "Key doesn't exist\n");

            u64 hash = this->safe_hash(key);
            s64 index = this->resolve_collision(key, hash % this->m_capacity);
            RUNTIME_ASSERT(index != -1);
            
            HashmapEntry* entry = &this->m_entries[index];


            return entry->value;
        }

        V remove(K key) {
            RUNTIME_ASSERT_MSG(this->has(key), "Key doesn't exist\n");

            u64 hash = this->safe_hash(key);
            s64 index = this->resolve_collision(key, hash % this->m_capacity);
            RUNTIME_ASSERT(index != -1);
            
            HashmapEntry* entry = &this->m_entries[index];

            this->m_count -= 1;
            this->m_dead_count += 1;
            entry->dead = true;

            return entry->value;
        }

        void clear() {
            this->m_count = 0;
            this->m_dead_count = 0;
            Memory::zero(this->m_entries, sizeof(HashmapEntry) * this->m_capacity);
        }

        u64 count() {
            return this->m_count;
        }
    private:
        u64 m_count = 0;
        u64 m_capacity = 0;
        u64 m_dead_count = 0;
        HashmapEntry* m_entries = nullptr;
        HashFunction* m_hash_func = nullptr;
        EqualFunction* m_equal_func = nullptr;
        Memory::BaseAllocator* m_allocator = nullptr;

        void grow_and_rehash() {
            u64 old_capacity = this->m_capacity;
            HashmapEntry* old_entries = this->m_entries;

            this->m_capacity *= 2;
            byte_t new_allocation_size = (this->m_capacity * sizeof(HashmapEntry));
            this->m_entries = (HashmapEntry*)this->m_allocator->malloc(new_allocation_size);
            Memory::zero(this->m_entries, new_allocation_size);

            // rehash
            for (u64 i = 0; i < old_capacity; i++) {
                HashmapEntry old_entry = old_entries[i];
                if (!old_entry.filled || old_entry.dead) {
                    continue;
                }

                u64 hash = this->safe_hash(old_entry.key);
                s64 index = this->resolve_collision(old_entry.key, hash % this->m_capacity);
                RUNTIME_ASSERT(index != -1);

                this->m_entries[index] = old_entry;
            }

            this->m_allocator->free(old_entries);
        }

        s64 resolve_collision(K key, u64 inital_hash_index) {
            s64 cannonical_hash_index = inital_hash_index;

            u64 visited_count = 0;
            while (true) {
                HashmapEntry* entry = &this->m_entries[cannonical_hash_index];
                if (!entry->filled) {
                    break;
                }

                bool equality_match = this->safe_equality(key, entry->key);
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
        
        #define NOT_USED 0

        u64 safe_hash(K key) {
            constexpr bool key_is_trivial = std::is_trivially_copyable_v<K>;
            constexpr bool key_is_pointer = std::is_pointer_v<K>;
            constexpr bool key_is_cstring = std::is_same_v<K, char*> || std::is_same_v<K, const char*>;
            constexpr bool key_is_string_view = std::is_same_v<K, DS::View<char>> || std::is_same_v<K, DS::View<const char>>;

            if constexpr (key_is_trivial && !key_is_pointer) {
                return this->m_hash_func(&key, sizeof(K));
            } else if constexpr (key_is_cstring) {
                return this->m_hash_func((void*)key, NOT_USED);
            } else if constexpr (key_is_string_view) {
                return this->m_hash_func(&key, NOT_USED);
            } else {
                RUNTIME_ASSERT("WE ARE IN HELL?\n");
            }
        }

        bool safe_equality(K k1, K k2) {
            constexpr bool key_is_trivial = std::is_trivially_copyable_v<K>;
            constexpr bool key_is_pointer = std::is_pointer_v<K>;
            constexpr bool key_is_cstring = std::is_same_v<K, char*> || std::is_same_v<K, const char*>;
            constexpr bool key_is_string_view = std::is_same_v<K, DS::View<char>> || std::is_same_v<K, DS::View<const char>>;

            if constexpr (key_is_trivial && !key_is_pointer) {
                return this->m_equal_func(&k1, sizeof(K), &k2, sizeof(K));
            } else if constexpr (key_is_cstring) {
                return this->m_equal_func(k1, NOT_USED, k2, NOT_USED);
            } else if constexpr (key_is_string_view) {
                return this->m_equal_func(&k1, NOT_USED, &k2, NOT_USED);
            } else {
                RUNTIME_ASSERT("WE ARE IN HELL?\n");
            }
        }

        #undef NOT_USED
    };
}