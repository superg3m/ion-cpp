#pragma once

#include "../DataStructure/ds.hpp"

#define X_SYNTAX_TOKENS                \
    X(TOKEN_SYNTAX_PLUS, "+")          \
    X(TOKEN_SYNTAX_MINUS, "-")         \
    X(TOKEN_SYNTAX_STAR, "*")          \
    X(TOKEN_SYNTAX_DIVISION, "/")      \
    X(TOKEN_SYNTAX_MODULUS, "%")       \
    X(TOKEN_SYNTAX_LEFT_CURLY, "{")    \
    X(TOKEN_SYNTAX_RIGHT_CURLY, "}")   \
    X(TOKEN_SYNTAX_LEFT_PAREN, "(")    \
    X(TOKEN_SYNTAX_RIGHT_PAREN, ")")   \
    X(TOKEN_SYNTAX_LEFT_BRACKET, "[")  \
    X(TOKEN_SYNTAX_RIGHT_BRACKET, "]") \
    X(TOKEN_SYNTAX_COLON, ":")         \
    X(TOKEN_SYNTAX_COMMA, ",")         \
    X(TOKEN_SYNTAX_LOGICAL_OR, "||")   \
    X(TOKEN_SYNTAX_LOGICAL_AND, "&&")  \

#define X_LITERAL_TOKENS       \
    X(TOKEN_LITERAL_INTEGER)   \
    X(TOKEN_LITERAL_FLOAT)     \
    X(TOKEN_LITERAL_STRING)    \
    X(TOKEN_LITERAL_CHARACTER) \

#define X_KEYWORD_TOKENS              \
    X(TOKEN_KEYWORD_IF, "if")         \
    X(TOKEN_KEYWORD_ELSE, "else")     \
    X(TOKEN_KEYWORD_FOR, "for")       \
    X(TOKEN_KEYWORD_WHILE, "while")   \
    X(TOKEN_KEYWORD_TRUE, "true")     \
    X(TOKEN_KEYWORD_FALSE, "false")   \
    X(TOKEN_KEYWORD_FUNC, "func")     \
    X(TOKEN_KEYWORD_VAR, "var")       \
    X(TOKEN_KEYWORD_NULL, "null")     \
    X(TOKEN_KEYWORD_RETURN, "return") \

enum TokenType {
    TOKEN_ILLEGAL_TOKEN,
    TOKEN_EOF,
    
    // Single Character Syntax
    #define X(name, str) name,
        X_SYNTAX_TOKENS
    #undef X

    #define X(name) name,
        X_LITERAL_TOKENS
    #undef X

    #define X(name, str) name,
        X_KEYWORD_TOKENS
    #undef X

    TOKEN_IDENTIFIER,

    TOKEN_COUNT
};

struct Token {
    TokenType type = TOKEN_ILLEGAL_TOKEN;
    u32 line = 1;

    DS::View<char> sv; // used for identifer names, string literals, the source_view

    union {
        s32 i;
        float f;
        char c;
        bool b;
    };

    Token() = default;
    Token(TokenType token_type, DS::View<char> sv, int line);

    static Token KeywordTokenFromSourceView(DS::View<char> sv, int line);
    static Token SyntaxTokenFromSourceView(DS::View<char> sv, int line);
    static Token LiteralTokenFromSourceView(DS::View<char> sv, int line);

    void print();
    const char* type_to_string() const;
};