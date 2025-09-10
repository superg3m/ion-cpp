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

typedef struct JSON JSON;

struct JsonValueObject {
    DS::Hashmap<char*, JSON*> key_value_map;
};

struct JsonValueArray {
    DS::Vector<JSON*> elements;
};

struct JSON {
    JsonValueType type;
    Memory::Allocator& allocator;

    union {
        int integer;
        float floating;
        bool boolean;
        const char* null;
        DS::View<char> string;
        JsonValueObject object;
        JsonValueArray array;
    };

    void push(char* key, int value) {
        RUNTIME_ASSERT(this->type == JSON_VALUE_OBJECT);
        this->object.key_value_map.put(key, JSON::Integer(this->allocator, value));
    }

    void push(char* key, float value) {
        RUNTIME_ASSERT(this->type == JSON_VALUE_OBJECT);
        this->object.key_value_map.put(key, JSON::Floating(this->allocator, value));
    }

    void push(char* key, bool value) {
        RUNTIME_ASSERT(this->type == JSON_VALUE_OBJECT);
        this->object.key_value_map.put(key, JSON::Boolean(this->allocator, value));
    }

    void push(char* key, char* value) {
        RUNTIME_ASSERT(this->type == JSON_VALUE_OBJECT);
        this->object.key_value_map.put(key, JSON::String(this->allocator, value));
    }

    void push(char* key, DS::View<char> value) {
        RUNTIME_ASSERT(this->type == JSON_VALUE_OBJECT);
        this->object.key_value_map.put(key, JSON::String(this->allocator, value));
    }

    void array_push(int value) {
        RUNTIME_ASSERT(this->type == JSON_VALUE_ARRAY);
        this->array.elements.push(JSON::Integer(this->allocator, value));
    }

    void array_push(float value) {
        RUNTIME_ASSERT(this->type == JSON_VALUE_ARRAY);
        this->array.elements.push(JSON::Floating(this->allocator, value));
    }

    void array_push(bool value) {
        RUNTIME_ASSERT(this->type == JSON_VALUE_ARRAY);
        this->array.elements.push(JSON::Boolean(this->allocator, value));
    }

    void array_push(char* value) {
        RUNTIME_ASSERT(this->type == JSON_VALUE_ARRAY);
        this->array.elements.push(JSON::String(this->allocator, value));
    }

    void array_push(DS::View<char> value) {
        RUNTIME_ASSERT(this->type == JSON_VALUE_ARRAY);
        this->array.elements.push(JSON::String(this->allocator, value));
    }
private:
    static JSON* Object(Memory::Allocator& allocator) {
        JSON* ret = (JSON*)allocator.malloc(sizeof(JSON));
        ret->allocator = allocator;
        ret->type = JSON_VALUE_OBJECT;
        ret->object.key_value_map = DS::Hashmap<char*, JSON*>(1, allocator);
        return ret;
    }

    static JSON* Array(Memory::Allocator& allocator) {
        JSON* ret = (JSON*)allocator.malloc(sizeof(JSON));
        ret->allocator = allocator;
        ret->type = JSON_VALUE_ARRAY;
        ret->array.elements = DS::Vector<JSON*>(1, allocator);
        return ret;
    }

    static JSON* Integer(Memory::Allocator& allocator, int value) {
        JSON* ret = (JSON*)allocator.malloc(sizeof(JSON));
        ret->allocator = allocator;
        ret->type = JSON_VALUE_INT;
        ret->integer = value;
        return ret;
    }

    static JSON* Floating(Memory::Allocator& allocator, float value) {
        JSON* ret = (JSON*)allocator.malloc(sizeof(JSON));
        ret->allocator = allocator;
        ret->type = JSON_VALUE_FLOAT;
        ret->floating = value;
        return ret;
    }

    static JSON* Boolean(Memory::Allocator& allocator, bool value) {
        JSON* ret = (JSON*)allocator.malloc(sizeof(JSON));
        ret->allocator = allocator;
        ret->type = JSON_VALUE_BOOL;
        ret->boolean = value;
        return ret;
    }

    static JSON* String(Memory::Allocator& allocator, DS::View<char> value) {
        JSON* ret = (JSON*)allocator.malloc(sizeof(JSON));
        ret->allocator = allocator;
        ret->type = JSON_VALUE_STRING;
        ret->string = value;
        return ret;
    }

    static JSON* String(Memory::Allocator& allocator, char* str) {
        u64 str_length = String::length(str);
        return JSON::String(allocator, DS::View<char>(str, str_length));
    }

    static JSON* Null(Memory::Allocator& allocator) {
        JSON* ret = (JSON*)allocator.malloc(sizeof(JSON));
        ret->allocator = allocator;
        ret->type = JSON_VALUE_NULL;
        ret->null = "null";

        return ret;
    }

    void push(char* key, JSON* value) {
        RUNTIME_ASSERT(this->type == JSON_VALUE_OBJECT);
        this->object.key_value_map.put(key, value);
    }

    void array_push(JSON* value) {
        RUNTIME_ASSERT(this->type == JSON_VALUE_ARRAY);
        this->array.elements.push(value);
    }
};