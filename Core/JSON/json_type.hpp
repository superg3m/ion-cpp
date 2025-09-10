enum JsonType {
    JSON_INT,
    JSON_INT,
    JSON_INT,
    JSON_INT,
    JSON_INT,
    JSON_INT,
    JSON_INT,
};

struct JSON {
    JsonType type;

    union {
        int json_int;
        float json_float;
        bool json_bool;
        Container::View<char> json_string;
        JsonObject json_object;
        JsonArray json_array;
    }
};