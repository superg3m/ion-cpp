#pragma once

#include "../DataStructure/ds.hpp"
#include <type_traits>

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

struct KeyJsonPair {
    const char* key;
    JSON* value;
};

struct JsonValueObject {
    DS::Hashmap<const char*, bool> keys;
    DS::Vector<KeyJsonPair> order;
};

struct JsonValueArray {
    DS::Vector<JSON*> elements;
};

template<typename T>
concept SupportedType = (
    std::is_same_v<T, int> ||
    std::is_same_v<T, float> || 
    std::is_same_v<T, double> || 
    std::is_same_v<T, bool> ||
    std::is_same_v<T, char*> ||
    std::is_same_v<T, const char*> || 
    std::is_same_v<T, DS::View<char>> ||
    std::is_same_v<T, JSON*>
);

struct JSON {
    JsonValueType type;
    Memory::BaseAllocator* allocator;

    union {
        int integer;
        float floating;
        bool boolean;
        DS::View<char> string;
        JsonValueObject object;
        JsonValueArray array;
    };

    static JSON* Object(Memory::BaseAllocator* allocator);
    static JSON* Array(Memory::BaseAllocator* allocator);
    static JSON* Null(Memory::BaseAllocator* allocator);

    template<SupportedType T>
    constexpr JSON* MAKE_JSON_VALUE(T value) {
        if constexpr (std::is_same_v<T, int>) {
            return JSON::Integer(this->allocator, value);
        } else if constexpr (std::is_same_v<T, float>) {
            return JSON::Floating(this->allocator, value);
        } else if constexpr (std::is_same_v<T, double>) {
            return JSON::Floating(this->allocator, value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return JSON::Boolean(this->allocator, value);
        } else if constexpr (std::is_same_v<T, char*> || std::is_same_v<T, const char*>) {
            return JSON::String(this->allocator, value);
        } else if constexpr (std::is_same_v<T, DS::View<char>>) {
            return JSON::String(this->allocator, value);
        } else if constexpr (std::is_same_v<T, JSON*>) {
            return value;
        }

        RUNTIME_ASSERT(false);

        return nullptr;
    }

    template<SupportedType T>
    void push(const char* key, T value) {
        RUNTIME_ASSERT(this->type == JSON_VALUE_OBJECT);
        RUNTIME_ASSERT_MSG(!this->object.keys.has(key), "Duplicate key: %s\n", key);

        this->object.keys.put(key, true);
        this->object.order.push((KeyJsonPair){key , MAKE_JSON_VALUE(value)});
    }

    template<SupportedType T>
    void array_push(T value) {
        RUNTIME_ASSERT(this->type == JSON_VALUE_ARRAY);
    
        this->array.elements.push(MAKE_JSON_VALUE(value));
    }

    static char* to_string(JSON* root, const char* indent = "    ");
    
private:
    static JSON* Integer(Memory::BaseAllocator* allocator, int value);
    static JSON* Floating(Memory::BaseAllocator* allocator, float value);
    static JSON* Boolean(Memory::BaseAllocator* allocator, bool value);
    static JSON* String(Memory::BaseAllocator* allocator, DS::View<char> value);
    static JSON* String(Memory::BaseAllocator* allocator, DS::View<const char> value);
    static JSON* String(Memory::BaseAllocator* allocator, char* str);
    static JSON* String(Memory::BaseAllocator* allocator, const char* str);
};
