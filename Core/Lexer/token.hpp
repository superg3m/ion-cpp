#pragma once

#include <Core/core.hpp>

enum TokenType {
    TOKEN_ILLEGAL_TOKEN,
    TOKEN_EOF,                        // EOF
    
    TOKEN_LEFT_PAREN,                 // "("
    TOKEN_RIGHT_PAREN,                // ")"
    TOKEN_STAR,                       // "*"
    TOKEN_PLUS,                       // "+"
    TOKEN_MINUS,                      // "-"
    TOKEN_DIVISION,                   // "/"
    TOKEN_MODULUS,                    // "%"

    TOKEN_INTEGER_LITERAL,            // 32, -12
    TOKEN_FLOAT_LITERAL,              // 0.23423
    TOKEN_STRING_LITERAL,             // "sdfdsfdsf"
    TOKEN_CHARACTER_LITERAL,          // '\0'

    TOKEN_TRUE,                       // true
    TOKEN_FALSE,                      // false

    TOKEN_COUNT
};

struct Token {
    TokenType type = TOKEN_ILLEGAL_TOKEN;
    u32 line = 1;

    Container::View<char> sv; // used for identifer names, string literals, the source_view

    Token() = default;
    Token(TokenType token_type, Container::View<char> sv, int line) : sv(sv) {
        this->type = token_type;
        this->line = line;
    }

    static Token SyntaxTokenFromSourceView(Container::View<char> sv, int line) {
        static Container::Hashmap<Container::View<char>, TokenType> syntax_map = {
            { Container::View<char>("+", sizeof("+") - 1), TOKEN_PLUS },
            { Container::View<char>("-", sizeof("-") - 1), TOKEN_MINUS },
            { Container::View<char>("*", sizeof("*") - 1), TOKEN_STAR },
            { Container::View<char>("/", sizeof("/") - 1), TOKEN_DIVISION },
            { Container::View<char>("%", sizeof("%") - 1), TOKEN_MODULUS },
        };

        Token ret = Token(); // Invalid
        if (syntax_map.has(sv)) {
            ret.type = syntax_map.get(sv);
            ret.line = line;
            ret.sv = sv;
        }

        return ret;
    }

    static Token LiteralTokenFromSourceView(Container::View<char> sv, int line) {
        Token ret = Token();
        ret.sv = sv;
        ret.line = line;

        if (ret.sv.data[0] == '"') {
            ret.type = TOKEN_STRING_LITERAL;

            return ret;;
        } else if (ret.sv.data[0] == '\'') {
            ret.type = TOKEN_CHARACTER_LITERAL;
            ret.c = ret.sv.data[1];

            return ret;
        }

        if (String::equal(ret.sv.data, sv.length, "true", sizeof("true") - 1)) {
            ret.type = TOKEN_TRUE;
            ret.b = true;

            return ret;
        } else if (String::equal(ret.sv.data, sv.length, "false", sizeof("false") - 1)) {
            ret.type = TOKEN_FALSE;
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
            ret.type = TOKEN_INTEGER_LITERAL;
            ret.i = (int)i;

            return ret;
        }
        
        char* endptr = NULL;
        float f = strtof(buffer, &endptr);
        if ((byte_t)(endptr - buffer) == sv.length) {
            ret.type = TOKEN_FLOAT_LITERAL;
            ret.f = f;

            return ret;
        }
    }

    union {
        s32 i;
        float f;
        char c;
        bool b;
    };

    void print();

    const char* type_to_string() const {
        static const char* token_strings[] = {
            stringify(TOKEN_ILLEGAL_TOKEN),
            stringify(TOKEN_EOF),        

            stringify(TOKEN_LEFT_PAREN), 
            stringify(TOKEN_RIGHT_PAREN),
            stringify(TOKEN_STAR),       
            stringify(TOKEN_PLUS),       
            stringify(TOKEN_MINUS),      
            stringify(TOKEN_DIVISION),   
            stringify(TOKEN_MODULUS),    

            stringify(TOKEN_INTEGER_LITERAL),    
            stringify(TOKEN_FLOAT_LITERAL),    
            stringify(TOKEN_STRING_LITERAL),    
            stringify(TOKEN_CHARACTER_LITERAL),  

            stringify(TOKEN_TRUE),    
            stringify(TOKEN_FALSE),
        };

        STATIC_ASSERT(ArrayCount(token_strings) == TOKEN_COUNT);

        return token_strings[type];
    }
};