#pragma once

#include <Core/core.hpp>
#include <stdlib.h>

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

    union {
        s32 i;
        float f;
        char c;
        bool b;
    };

    Token() = default;
    Token(TokenType token_type, Container::View<char> sv, int line);

    static Token SyntaxTokenFromSourceView(Container::View<char> sv, int line);
    static Token LiteralTokenFromSourceView(Container::View<char> sv, int line);

    void print();
    const char* type_to_string() const;
};