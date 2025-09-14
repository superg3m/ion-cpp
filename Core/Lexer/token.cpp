#include "token.hpp"
#include <stdlib.h>

Token::Token(TokenType token_type, DS::View<char> sv, int line) : sv(sv) {
    this->type = token_type;
    this->line = line;
}

Token Token::KeywordTokenFromSourceView(DS::View<char> sv, int line) {
    Memory::GeneralAllocator general_allocator = Memory::GeneralAllocator();

    static DS::Hashmap<DS::View<char>, TokenType> syntax_map = {
        #define X(name, str) { DS::View<char>(str, sizeof(str) - 1), name },
            X_KEYWORD_TOKENS
        #undef X
    };

    Token ret = Token(); // Invalid
    ret.sv = sv;
    if (syntax_map.has(sv)) {
        ret.type = syntax_map.get(sv);
        ret.line = line;
        ret.sv = sv;
    }

    return ret;
}

Token Token::SyntaxTokenFromSourceView(DS::View<char> sv, int line) {
    static DS::Hashmap<DS::View<char>, TokenType> syntax_map = {
        #define X(name, str) { DS::View<char>(str, sizeof(str) - 1), name },
            X_SYNTAX_TOKENS
        #undef X
    };

    Token ret = Token(); // Invalid
    if (syntax_map.has(sv)) {
        ret.type = syntax_map.get(sv);
        ret.line = line;
        ret.sv = sv;
    }

    return ret;
}

Token Token::LiteralTokenFromSourceView(DS::View<char> sv, int line) {
    Token ret = Token();
    ret.sv = sv;
    ret.line = line;

    if (ret.sv.data[0] == '"') {
        ret.type = TOKEN_LITERAL_STRING;

        return ret;;
    } else if (ret.sv.data[0] == '\'') {
        ret.type = TOKEN_LITERAL_CHARACTER;
        ret.c = ret.sv.data[1];

        return ret;
    }

    if (String::equal(ret.sv.data, sv.length, "true", sizeof("true") - 1)) {
        ret.type = TOKEN_KEYWORD_TRUE;
        ret.b = true;

        return ret;
    } else if (String::equal(ret.sv.data, sv.length, "false", sizeof("false") - 1)) {
        ret.type = TOKEN_KEYWORD_FALSE;
        ret.b = false;

        return ret;
    }

    char buffer[128];
    RUNTIME_ASSERT(sv.length < sizeof(buffer));

    Memory::copy(buffer, ret.sv.length, ret.sv.data, ret.sv.length);
    buffer[ret.sv.length] = '\0';
    
    char* endptr_int = NULL;
    long i = strtol(buffer, &endptr_int, 10);
    if ((byte_t)(endptr_int - buffer) == sv.length) {
        ret.type = TOKEN_LITERAL_INTEGER;
        ret.i = (int)i;

        return ret;
    }
    
    char* endptr = NULL;
    float f = strtof(buffer, &endptr);
    if ((byte_t)(endptr - buffer) == sv.length) {
        ret.type = TOKEN_LITERAL_FLOAT;
        ret.f = f;

        return ret;
    }

    return ret;
}

void Token::print() {
    LOG_TRACE("%s(%.*s) | line: %d\n", this->type_to_string(), (int)this->sv.length, this->sv.data, this->line);
}

const char* Token::type_to_string() const {
    static const char* token_strings[] = {
        stringify(TOKEN_ILLEGAL_TOKEN),
        stringify(TOKEN_EOF),
        
        #define X(name, str) stringify(name),
            X_SYNTAX_TOKENS
        #undef X   

        #define X(name) stringify(name),
            X_LITERAL_TOKENS
        #undef X

        #define X(name, str) stringify(name),
            X_KEYWORD_TOKENS
        #undef X
            
        stringify(TOKEN_IDENTIFIER),
    };

    STATIC_ASSERT(ArrayCount(token_strings) == TOKEN_COUNT);

    return token_strings[this->type];
}