#include "json.hpp"

JSON* JSON::Object(Memory::BaseAllocator* allocator) {
    JSON* ret = (JSON*)allocator->malloc(sizeof(JSON));
    ret->allocator = allocator;
    ret->type = JSON_VALUE_OBJECT;
    ret->object.key_value_map = DS::Hashmap<const char*, JSON*>(1, allocator);

    return ret;
}

JSON* JSON::Array(Memory::BaseAllocator* allocator) {
    JSON* ret = (JSON*)allocator->malloc(sizeof(JSON));
    ret->allocator = allocator;
    ret->type = JSON_VALUE_ARRAY;
    ret->array.elements = DS::Vector<JSON*>(1, allocator);

    return ret;
}

JSON* JSON::Null(Memory::BaseAllocator* allocator) {
    JSON* ret = (JSON*)allocator->malloc(sizeof(JSON));
    ret->allocator = allocator;
    ret->type = JSON_VALUE_NULL;
    ret->null = "null";

    return ret;
}

void JSON::push(const char* key, int value) {
    RUNTIME_ASSERT(this->type == JSON_VALUE_OBJECT);
    RUNTIME_ASSERT_MSG(this->object.key_value_map.has(key), "Duplicate key: %s\n", key);

    this->object.key_value_map.put(key, JSON::Integer(this->allocator, value));
}

void JSON::push(const char* key, float value) {
    RUNTIME_ASSERT(this->type == JSON_VALUE_OBJECT);
    RUNTIME_ASSERT_MSG(this->object.key_value_map.has(key), "Duplicate key: %s\n", key);

    this->object.key_value_map.put(key, JSON::Floating(this->allocator, value));
}

void JSON::push(const char* key, bool value) {
    RUNTIME_ASSERT(this->type == JSON_VALUE_OBJECT);
    RUNTIME_ASSERT_MSG(this->object.key_value_map.has(key), "Duplicate key: %s\n", key);

    this->object.key_value_map.put(key, JSON::Boolean(this->allocator, value));
}

void JSON::push(const char* key, char* value) {
    RUNTIME_ASSERT(this->type == JSON_VALUE_OBJECT);
    RUNTIME_ASSERT_MSG(this->object.key_value_map.has(key), "Duplicate key: %s\n", key);

    this->object.key_value_map.put(key, JSON::String(this->allocator, value));
}

void JSON::push(const char* key, const char* value) {
    RUNTIME_ASSERT(this->type == JSON_VALUE_OBJECT);
    RUNTIME_ASSERT_MSG(this->object.key_value_map.has(key), "Duplicate key: %s\n", key);

    this->object.key_value_map.put(key, JSON::String(this->allocator, value));
}

void JSON::push(const char* key, DS::View<char> value) {
    RUNTIME_ASSERT(this->type == JSON_VALUE_OBJECT);
    RUNTIME_ASSERT_MSG(this->object.key_value_map.has(key), "Duplicate key: %s\n", key);

    this->object.key_value_map.put(key, JSON::String(this->allocator, value));
}

// -------- Push into array --------
void JSON::array_push(int value) {
    RUNTIME_ASSERT(this->type == JSON_VALUE_ARRAY);
    
    this->array.elements.push(JSON::Integer(this->allocator, value));
}

void JSON::array_push(float value) {
    RUNTIME_ASSERT(this->type == JSON_VALUE_ARRAY);
    this->array.elements.push(JSON::Floating(this->allocator, value));
}

void JSON::array_push(bool value) {
    RUNTIME_ASSERT(this->type == JSON_VALUE_ARRAY);
    this->array.elements.push(JSON::Boolean(this->allocator, value));
}

void JSON::array_push(char* value) {
    RUNTIME_ASSERT(this->type == JSON_VALUE_ARRAY);
    this->array.elements.push(JSON::String(this->allocator, value));
}

void JSON::array_push(DS::View<char> value) {
    RUNTIME_ASSERT(this->type == JSON_VALUE_ARRAY);
    this->array.elements.push(JSON::String(this->allocator, value));
}

JSON* JSON::Integer(Memory::BaseAllocator* allocator, int value) {
    JSON* ret = (JSON*)allocator->malloc(sizeof(JSON));
    ret->allocator = allocator;
    ret->type = JSON_VALUE_INT;
    ret->integer = value;

    return ret;
}

JSON* JSON::Floating(Memory::BaseAllocator* allocator, float value) {
    JSON* ret = (JSON*)allocator->malloc(sizeof(JSON));
    ret->allocator = allocator;
    ret->type = JSON_VALUE_FLOAT;
    ret->floating = value;

    return ret;
}

JSON* JSON::Boolean(Memory::BaseAllocator* allocator, bool value) {
    JSON* ret = (JSON*)allocator->malloc(sizeof(JSON));
    ret->allocator = allocator;
    ret->type = JSON_VALUE_BOOL;
    ret->boolean = value;

    return ret;
}

JSON* JSON::String(Memory::BaseAllocator* allocator, DS::View<char> value) {
    JSON* ret = (JSON*)allocator->malloc(sizeof(JSON));
    ret->allocator = allocator;
    ret->type = JSON_VALUE_STRING;
    ret->string = value;

    return ret;
}

JSON* JSON::String(Memory::BaseAllocator* allocator, char* str) {
    u64 str_length = String::length(str);
    return JSON::String(allocator, DS::View<char>(str, str_length));
}

JSON* JSON::String(Memory::BaseAllocator* allocator, const char* str) {
    u64 str_length = String::length(str);
    return JSON::String(allocator, DS::View<char>(str, str_length));
}

void JSON::push(const char* key, JSON* value) {
    RUNTIME_ASSERT(this->type == JSON_VALUE_OBJECT);
    this->object.key_value_map.put(key, value);
}

void JSON::array_push(JSON* value) {
    RUNTIME_ASSERT(this->type == JSON_VALUE_ARRAY);
    this->array.elements.push(value);
}