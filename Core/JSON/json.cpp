#include "json.hpp"

#include "../Lexer/lexer.hpp"
#include "../Parser/parser.hpp"


static const char* indent_from_depth(Memory::BaseAllocator* allocator, int depth, const char* indent) {
    if (depth == 0) {
        return "";
    }

    byte_t allocation_size = String::length(indent) * depth;
    char* ret = (char*)allocator->malloc(allocation_size);
    for (int i = 0; i < allocation_size; i++) {
        ret[i] = ' ';
    }

    return ret;
}

static const char* to_string_helper(JSON* root, const char* indent, int depth) {
    switch (root->type) {
        case JSON_VALUE_BOOL: {
            return String::sprintf(root->allocator, nullptr, "%s", root->boolean ? "true" : "false");
        } break;

        case JSON_VALUE_INT: {
            return String::sprintf(root->allocator, nullptr, "%d", root->integer);
        } break;

        case JSON_VALUE_FLOAT: {
            return String::sprintf(root->allocator, nullptr, "%f", root->floating);
        } break;

        case JSON_VALUE_STRING: {
            if (root->string.data[0] == '\"') {
                return String::sprintf(root->allocator, nullptr, "\"%.*s\"", root->string.length - 2, root->string.data + 1);
            } else {
                return String::sprintf(root->allocator, nullptr, "\"%.*s\"", root->string.length, root->string.data);
            }
        } break;

        case JSON_VALUE_NULL: {
            return "null";
        } break;

        case JSON_VALUE_ARRAY: {
            int array_count = root->array.elements.count();
            DS::Vector<byte_t> buffer_sizes = DS::Vector<byte_t>(array_count, root->allocator);
            const char** buffers = (const char**)root->allocator->malloc(sizeof(char*) * array_count);

            byte_t total_allocation_size = 1; // 1 for the null terminator
            const char* member_indent = indent_from_depth(root->allocator, depth + 1, indent);
            for (int i = 0; i < array_count; i++) {
                byte_t temp_size = 0;
                JSON* element = root->array.elements[i];
                const char* value = to_string_helper(element, indent, depth + 1);
                if (i == array_count - 1) {
                    buffers[i] = String::sprintf(root->allocator, &temp_size, "%s%s", member_indent, value);
                } else {
                    buffers[i] = String::sprintf(root->allocator, &temp_size, "%s%s,\n", member_indent, value);
                }

                buffer_sizes.push(temp_size);
                total_allocation_size += temp_size;
            }
            
            byte_t string_length = 0;
            char* buffer = (char*)root->allocator->malloc(total_allocation_size);
            for (int i = 0; i < array_count; i++) {
                String::append(buffer, string_length, total_allocation_size, buffers[i], buffer_sizes[i]);
                string_length += buffer_sizes[i];
            }

            return String::sprintf(root->allocator, nullptr, "[\n%s\n%s]", buffer, indent_from_depth(root->allocator, depth, indent));
        } break;

        case JSON_VALUE_OBJECT: {
            int object_member_count = root->object.pairs.count();
            DS::Vector<byte_t> buffer_sizes = DS::Vector<byte_t>(object_member_count, root->allocator);
            const char** buffers = (const char**)root->allocator->malloc(sizeof(char*) * object_member_count);

            byte_t total_allocation_size = 1; // 1 for the null terminator
            const char* member_indent = indent_from_depth(root->allocator, depth + 1, indent);
            for (int i = 0; i < object_member_count; i++) {
                KeyJsonPair pair = root->object.pairs[i];
                byte_t temp_size = 0;

                const char* value = to_string_helper(pair.value, indent, depth + 1);
                if (i == object_member_count - 1) {
                    buffers[i] = String::sprintf(root->allocator, &temp_size, "%s\"%s\": %s", member_indent, pair.key, value);
                } else {
                    buffers[i] = String::sprintf(root->allocator, &temp_size, "%s\"%s\": %s,\n", member_indent, pair.key, value);
                }

                buffer_sizes.push(temp_size);
                total_allocation_size += temp_size;
            }
            
            byte_t string_length = 0;
            char* buffer = (char*)root->allocator->malloc(total_allocation_size);
            for (int i = 0; i < object_member_count; i++) {
                String::append(buffer, string_length, total_allocation_size, buffers[i], buffer_sizes[i]);
                string_length += buffer_sizes[i];
            }

            return String::sprintf(root->allocator, nullptr, "{\n%s\n%s}", buffer, indent_from_depth(root->allocator, depth, indent));
        } break;
    }

    return nullptr;
}

const char* JSON::to_string(JSON* root, const char* indent) {
    if (root == nullptr) {
        return "JSON* root = null";
    }

    return to_string_helper(root, indent, 0);
}


static JSON* parse_helper(JSON* root, Parser* parser) {
    RUNTIME_ASSERT(root);

    Token token = parser->consume_next_token();

    switch (token.type) {
        case TOKEN_KEYWORD_FALSE:
        case TOKEN_KEYWORD_TRUE: {
            return JSON::Boolean(root->allocator, token.b);
        } break;

        case TOKEN_LITERAL_INTEGER: {
            return JSON::Integer(root->allocator, token.i);
        } break;

        case TOKEN_LITERAL_FLOAT: {
            return JSON::Floating(root->allocator, token.f);
        } break;

        case TOKEN_LITERAL_STRING: {
            return JSON::String(root->allocator, token.sv);
        } break;

        case TOKEN_KEYWORD_NULL: {
            return JSON::Null(root->allocator);
        } break;

        case TOKEN_SYNTAX_LEFT_BRACKET: {
            JSON* ret = JSON::Array(root->allocator);
            while (!parser->consume_on_match(TOKEN_SYNTAX_RIGHT_BRACKET)) {
                if (parser->peek_nth_token().type == TOKEN_ILLEGAL_TOKEN) {
                    return nullptr;
                }
                
                JSON* value = parse_helper(ret, parser);
                if (value == nullptr) {
                    return nullptr;
                }

                ret->array.elements.push(value);
                parser->consume_on_match(TOKEN_SYNTAX_COMMA);
            }

            return ret;
        } break;

        case TOKEN_SYNTAX_LEFT_CURLY: {
            JSON* ret = JSON::Object(root->allocator);
            while (!parser->consume_on_match(TOKEN_SYNTAX_RIGHT_CURLY)) {
                if (parser->peek_nth_token().type == TOKEN_ILLEGAL_TOKEN) {
                    return nullptr;
                }

                Token key_token = parser->consume_next_token();
                if (key_token.type != TOKEN_LITERAL_STRING) {
                    return nullptr;
                }

                const char* key = "";
                if (key_token.sv.data[0] == '\"') {
                    key = String::allocate(ret->allocator, key_token.sv.data + 1, key_token.sv.length - 2);
                } else {
                    key = String::allocate(ret->allocator, key_token.sv.data, key_token.sv.length);
                }
                
                Token color_token = parser->consume_next_token();
                if (color_token.type != TOKEN_SYNTAX_COLON) {
                    return nullptr;
                }

                JSON* value = parse_helper(ret, parser);
                if (value == nullptr) {
                    return nullptr;
                }

                ret->object.keys.put(key, true);
                ret->object.pairs.push((KeyJsonPair){key, value});
                parser->consume_on_match(TOKEN_SYNTAX_COMMA);
            }
            
            return ret;
        } break;

        default: {
            return nullptr;
        } break;        
    }

    return root;
}

JSON* JSON::parse(Memory::BaseAllocator* allocator, const char* json_string, u64 json_string_length) {
    DS::Vector<Token> tokens = DS::Vector<Token>(40, allocator);
    Lexer::generate_tokens((u8*)json_string, json_string_length, tokens);
    for (const Token& token : tokens) {
        const char* token_type_string = token.type_to_string();
        LOG_DEBUG("%s(%.*s)\n", token_type_string, token.sv.length, token.sv.data);
    }

    if (tokens[0].type != TOKEN_SYNTAX_LEFT_CURLY) {
        return nullptr;
    }

    Parser parser = Parser(allocator, tokens);

    JSON* root = JSON::Object(allocator);
    return parse_helper(root, &parser);
}

JSON* JSON::Object(Memory::BaseAllocator* allocator) {
    JSON* ret = (JSON*)allocator->malloc(sizeof(JSON));
    ret->allocator = allocator;
    ret->type = JSON_VALUE_OBJECT;
    ret->object.keys = DS::Hashmap<const char*, bool>(1, allocator);
    ret->object.pairs = DS::Vector<KeyJsonPair>(1, allocator);

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
