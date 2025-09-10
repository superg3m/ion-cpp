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

struct Token {
    TokenType type = TOKEN_ILLEGAL_TOKEN;
    u32 line = 1;

    Container::View<char> source_view; // used for identifer names, string literals, the source_view

    Token() = default;
    Token(Container::View<char> sv) : source_view(sv) {}

    union {
        s32 i;
        float f;
        char c;
    };

    const char* to_string() const {
        return token_strings[type];
    }
};

TokenType token_type_from_syntax(const char* buffer, byte_t length);
Token token_from_string(TokenType token_type, Container::View<char> sv, int line);