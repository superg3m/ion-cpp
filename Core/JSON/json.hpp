#pragma once

#include "../DataStructure/ds.hpp"

enum JsonValueType {
    JSON_VALUE_INT,
    JSON_VALUE_FLOAT,
    JSON_VALUE_STRING,
    JSON_VALUE_OBJECT,
    JSON_VALUE_ARRAY,
    JSON_VALUE_BOOL,
    JSON_VALUE_NULL
};

struct JSON;

struct JsonValueObject {
    DS::Hashmap<const char*, JSON*> key_value_map;
};

struct JsonValueArray {
    DS::Vector<JSON*> elements;
};

struct JSON {
    JsonValueType type;
    Memory::BaseAllocator* allocator;

    union {
        int integer;
        float floating;
        bool boolean;
        const char* null;
        DS::View<char> string;
        JsonValueObject object;
        JsonValueArray array;
    };

    static JSON* Object(Memory::BaseAllocator* allocator);
    static JSON* Array(Memory::BaseAllocator* allocator);
    static JSON* Null(Memory::BaseAllocator* allocator);

    void push(const char* key, int value);
    void push(const char* key, float value);
    void push(const char* key, bool value);
    void push(const char* key, char* value);
    void push(const char* key, const char* value);
    void push(const char* key, DS::View<char> value);
    void push(const char* key, JSON* value);

    void array_push(int value);
    void array_push(float value);
    void array_push(bool value);
    void array_push(char* value);
    void array_push(const char* value);
    void array_push(DS::View<char> value);
    void array_push(JSON* value);
private:
    static JSON* Integer(Memory::BaseAllocator* allocator, int value);
    static JSON* Floating(Memory::BaseAllocator* allocator, float value);
    static JSON* Boolean(Memory::BaseAllocator* allocator, bool value);
    static JSON* String(Memory::BaseAllocator* allocator, DS::View<char> value);
    static JSON* String(Memory::BaseAllocator* allocator, char* str);
    static JSON* String(Memory::BaseAllocator* allocator, const char* str);
};
