#include <Core/core.hpp>

using namespace Container;

// ---------- Tests ----------
void test_basic_put_get() {
    Hashmap<int, int> map;
    map.put(1, 10);
    map.put(2, 20);

    RUNTIME_ASSERT(map.has(1));
    RUNTIME_ASSERT(map.has(2));
    RUNTIME_ASSERT(map.get(1) == 10);
    RUNTIME_ASSERT(map.get(2) == 20);
    LOG_INFO("test_basic_put_get passed\n");
}

void test_overwrite() {
    Hashmap<int, int> map;
    map.put(1, 10);
    map.put(1, 99);

    RUNTIME_ASSERT(map.has(1));
    RUNTIME_ASSERT(map.get(1) == 99);
    LOG_INFO("test_overwrite passed\n");
}

void test_remove() {
    Hashmap<int, int> map;
    map.put(42, 100);
    RUNTIME_ASSERT(map.has(42));

    int val = map.remove(42);
    RUNTIME_ASSERT(val == 100);
    RUNTIME_ASSERT(!map.has(42));
    LOG_INFO("test_remove passed\n");
}

struct BadHash {
    static u64 hash(const void* data, byte_t) {
        return 0; // force collisions
    }
};

bool bad_equal(const void* a, byte_t, const void* b, byte_t) {
    return *(const int*)a == *(const int*)b;
}

void test_collisions() {
    Hashmap<int, int> map((HashFunction*)BadHash::hash, bad_equal, 4);
    map.put(1, 100);
    map.put(2, 200);
    map.put(3, 300);

    RUNTIME_ASSERT(map.get(1) == 100);
    RUNTIME_ASSERT(map.get(2) == 200);
    RUNTIME_ASSERT(map.get(3) == 300);
    LOG_INFO("test_collisions passed\n");
}

void test_rehashing() {
    Hashmap<int, int> map(2);
    for (int i = 0; i < 4; i++) {
        map.put(i, i * 10);
    }
    for (int i = 0; i < 4; i++) {
        RUNTIME_ASSERT(map.has(i));
        RUNTIME_ASSERT(map.get(i) == i * 10);
    }

    LOG_INFO("test_rehashing passed\n");
}

void test_string_keys() {
    Hashmap<const char*, int> map;
    map.put("apple+pen", 1);
    map.put("banana", 2);

    RUNTIME_ASSERT(map.has("apple+pen"));
    RUNTIME_ASSERT(map.has("banana"));
    RUNTIME_ASSERT(map.get("apple+pen") == 1);
    RUNTIME_ASSERT(map.get("banana") == 2);
    LOG_INFO("test_string_keys passed\n");
}

void test_dead_entries() {
    Hashmap<int, int> map;
    map.put(1, 10);
    map.put(2, 20);
    map.remove(1);

    RUNTIME_ASSERT(!map.has(1));
    RUNTIME_ASSERT(map.has(2));
    RUNTIME_ASSERT(map.get(2) == 20);

    map.put(1, 99);
    RUNTIME_ASSERT(map.get(1) == 99);
    LOG_INFO("test_dead_entries passed\n");
}

void test_stress() {
    Hashmap<int, int> map;
    for (int i = 0; i < 1000; i++) {
        map.put(i, i);
    }
    for (int i = 0; i < 1000; i++) {
        RUNTIME_ASSERT(map.has(i));
        RUNTIME_ASSERT(map.get(i) == i);
    }
    for (int i = 0; i < 1000; i++) {
        map.remove(i);
    }
    for (int i = 0; i < 1000; i++) {
        RUNTIME_ASSERT(!map.has(i));
    }
    LOG_INFO("test_stress passed\n");
}

int main() {
    test_basic_put_get();
    test_overwrite();
    test_remove();
    test_collisions();
    test_rehashing();
    test_string_keys();
    test_dead_entries();
    test_stress();

    LOG_INFO("All tests passed ✅\n");
    return 0;
}
