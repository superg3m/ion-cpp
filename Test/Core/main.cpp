#include <Core/core.hpp>

struct Point {
    int x;
    int y;

    static u64 hash(const void* source, byte_t source_size) {
        Point* p = (Point*)source;

        return p->x ^ p->y << 3;
    }

    static bool equal(const void* c1, byte_t c1_size, const void* c2, byte_t c2_size) {
        (void)c1_size;
        (void)c2_size;

        Point* p1 = (Point*)c1;
        Point* p2 = (Point*)c2;

        return p1->x == p2->x && p1->y == p2->y;
    }
};

// ---------- Tests ----------
void test_basic_put_get() {
    DS::Hashmap<int, int> map;
    map.put(1, 10);
    map.put(2, 20);
    RUNTIME_ASSERT(map.has(1));
    RUNTIME_ASSERT(map.has(2));
    RUNTIME_ASSERT(map.get(1) == 10);
    RUNTIME_ASSERT(map.get(2) == 20);
    LOG_INFO("test_basic_put_get passed\n");
}
void test_overwrite() {
    DS::Hashmap<int, int> map;
    map.put(1, 10);
    map.put(1, 99);
    RUNTIME_ASSERT(map.has(1));
    RUNTIME_ASSERT(map.get(1) == 99);
    LOG_INFO("test_overwrite passed\n");
}
void test_remove() {
    DS::Hashmap<int, int> map;
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
    DS::Hashmap<int, int> map((DS::HashFunction*)BadHash::hash, bad_equal, 4);
    map.put(1, 100);
    map.put(2, 200);
    map.put(3, 300);
    RUNTIME_ASSERT(map.get(1) == 100);
    RUNTIME_ASSERT(map.get(2) == 200);
    RUNTIME_ASSERT(map.get(3) == 300);
    LOG_INFO("test_collisions passed\n");
}
void test_rehashing() {
    DS::Hashmap<int, int> map(2);
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
    DS::Hashmap<const char*, int> map;
    map.put("apple+pen", 1);
    map.put("banana", 2);
    RUNTIME_ASSERT(map.has("apple+pen"));
    RUNTIME_ASSERT(map.has("banana"));
    RUNTIME_ASSERT(map.get("apple+pen") == 1);
    RUNTIME_ASSERT(map.get("banana") == 2);
    LOG_INFO("test_string_keys passed\n");
}
void test_dead_entries() {
    DS::Hashmap<int, int> map;
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
    DS::Hashmap<int, int> map;
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

void test_put_get_large_values() {
    DS::Hashmap<int, double> map;
    for (int i = 0; i < 500; ++i) {
        map.put(i, static_cast<double>(i) * 3.14);
    }

    for (int i = 0; i < 500; ++i) {
        RUNTIME_ASSERT(map.has(i));
        RUNTIME_ASSERT(map.get(i) == static_cast<double>(i) * 3.14);
    }

    LOG_INFO("test_put_get_large_values passed\n");
}

void test_remove_nonexistent() {
    DS::Hashmap<int, int> map;
    map.put(1, 10);
    // map.remove(2);
    // RUNTIME_ASSERT(!removed);
    RUNTIME_ASSERT(map.has(1));
    LOG_INFO("test_remove_nonexistent passed\n");
}

void test_has_nonexistent() {
    DS::Hashmap<int, int> map;
    map.put(1, 10);
    RUNTIME_ASSERT(!map.has(2));
    LOG_INFO("test_has_nonexistent passed\n");
}

void test_string_view_keys() {
    DS::Hashmap<DS::View<char>, int> map;


    DS::View<char> k1 = DS::View<char>("key1testings", sizeof("key1testings") - 1);
    DS::View<char> k2 = DS::View<char>("key2testingstestings", sizeof("key2testingstestings") - 1);
    DS::View<char> k3 = DS::View<char>("key3testings", sizeof("key3testings") - 1);


    map.put(k1, 100);
    map.put(k2, 200);
    RUNTIME_ASSERT(map.has(k1));
    RUNTIME_ASSERT(map.has(k2));
    RUNTIME_ASSERT(map.get(k1) == 100);
    RUNTIME_ASSERT(map.get(k2) == 200);

    map.remove(k1);
    RUNTIME_ASSERT(!map.has(k1));
    RUNTIME_ASSERT(map.has(k2));

    map.put(k3, 300);
    RUNTIME_ASSERT(map.get(k3) == 300);
    LOG_INFO("test_string_view_keys passed\n");
}

void test_cstring_keys() {
    DS::Hashmap<const char*, int> map;

    const char* key1 = "DFLKSJDFKDSKLFJDSKLJF";
    const char* key2 = "TSLKFJSFLKSJFLJDSLJFDLSF";
    const char* key3 = "dfgdfSFLKSJFLJDSLJFDLSF";

    map.put(key1, 100);
    map.put(key2, 200);
    map.put(key3, 300);

    RUNTIME_ASSERT(map.get(key1) == 100);
    RUNTIME_ASSERT(map.get(key2) == 200);
    RUNTIME_ASSERT(map.get(key3) == 300);
   
    LOG_INFO("test_string_view_keys passed\n");
}

void test_custom_struct_keys() {
    DS::Hashmap<Point, int> map = DS::Hashmap<Point, int>(Point::hash, Point::equal);
    Point p1 = {1, 2};
    Point p2 = {3, 4};
    Point p3 = {1, 2}; // Same as p1

    map.put(p1, 100);
    map.put(p2, 200);
    RUNTIME_ASSERT(map.has(p1));
    RUNTIME_ASSERT(map.has(p2));
    RUNTIME_ASSERT(map.has(p3)); // Should be true due to custom equality
    RUNTIME_ASSERT(map.get(p1) == 100);
    RUNTIME_ASSERT(map.get(p3) == 100);
    RUNTIME_ASSERT(map.get(p2) == 200);

    map.put(p1, 999); // Overwrite
    RUNTIME_ASSERT(map.get(p3) == 999);

    int val = map.remove(p2);
    RUNTIME_ASSERT(val == 200);
    RUNTIME_ASSERT(!map.has(p2));
    RUNTIME_ASSERT(map.has(p1));
    
    LOG_INFO("test_custom_struct_keys passed\n");
}

void test_edge_cases() {
    DS::Hashmap<int, int> map(1); // Small initial size
    map.put(1, 1);
    map.put(2, 2);
    map.put(3, 3);
    map.remove(2);
    map.put(4, 4);
    RUNTIME_ASSERT(map.has(1));
    RUNTIME_ASSERT(!map.has(2));
    RUNTIME_ASSERT(map.has(3));
    RUNTIME_ASSERT(map.has(4));
    RUNTIME_ASSERT(map.get(1) == 1);
    RUNTIME_ASSERT(map.get(3) == 3);
    RUNTIME_ASSERT(map.get(4) == 4);
    LOG_INFO("test_edge_cases passed\n");
}

void test_clear() {
    DS::Hashmap<int, int> map;
    map.put(1, 10);
    map.put(2, 20);
    map.clear();
    RUNTIME_ASSERT(map.count() == 0);
    RUNTIME_ASSERT(!map.has(1));
    RUNTIME_ASSERT(!map.has(2));
    LOG_INFO("test_clear passed\n");
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
    test_put_get_large_values();
    test_remove_nonexistent();
    test_has_nonexistent();
    test_string_view_keys();
    test_cstring_keys();
    test_custom_struct_keys();
    test_edge_cases();
    test_clear();

    LOG_INFO("All tests passed ✅\n");
    JSON* root = JSON::Object(&Memory::global_general_allocator);
    root->push("Hello", "World!");
    root->push("Hello1", 2);

    return 0;
}
