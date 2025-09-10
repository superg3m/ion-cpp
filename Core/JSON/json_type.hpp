#include "../DataStructure/ds.hpp"

enum JsonValeType {
    JSON_VALUE_INT,
    JSON_VALUE_FLOAT,
    JSON_VALUE_STRING,
    JSON_VALUE_OBJECT,
    JSON_VALUE_ARRAY,
    JSON_VALUE_NULL
};

typedef struct JSON JSON;

struct JsonValueObject {
    DS::Hashmap<DS::View key_value_map
}

struct JsonValueArray {
    DS::Vector<JSON> elements;
}

struct JsonValueArray {

}

struct JSON {
    JsonValeType type;

    union {
        int i;
        float j;
        bool b;
        DS::View<char> string;
        JsonObject object;
        JsonArray array;
        const char* null;
    } value;
};