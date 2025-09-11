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
