#pragma once

#include "../DataStructure/ds.hpp"

#define X_SYNTAX_TOKENS          \
    X(TS_PLUS, "+")              \
    X(TS_MINUS, "-")             \
    X(TS_STAR, "*")              \
    X(TS_DIVISION, "/")          \
    X(TS_MODULUS, "%")           \
    X(TS_LEFT_CURLY, "{")        \
    X(TS_RIGHT_CURLY, "}")       \
    X(TS_LEFT_PAREN, "(")        \
    X(TS_RIGHT_PAREN, ")")       \
    X(TS_LEFT_BRACKET, "[")      \
    X(TS_RIGHT_BRACKET, "]")     \
    X(TS_COLON, ":")             \
    X(TS_COMMA, ",")             \
    X(TS_SEMI_COLON, ";")        \
    X(TS_RIGHT_ARROW, "->")      \
                                 \
    X(TSA_EQUALS, "=")           \
    X(TSA_INFER_EQUALS, ":=")    \
    X(TSA_PLUS_EQUALS, "+=")     \
    X(TSA_MINUS_EQUALS, "-=")    \
    X(TSA_STAR_EQUALS, "*=")     \
    X(TSA_DIVISION_EQUALS, "/=") \
                                 \
    X(TSL_OR, "||")              \
    X(TSL_AND, "&&")             \
                                 \
    X(TSC_EQUAL, "==")           \
    X(TSC_NOT_EQUAL, "!=")       \
    X(TSC_GT, ">")               \
    X(TSC_GT_OR_EQUAL, ">=")     \
    X(TSC_LT_OR_EQUAL, "<=")     \
    X(TSC_EQUALS_EQUALS, "==")   \

#define X_LITERAL_TOKENS \
    X(TL_INTEGER)        \
    X(TL_FLOAT)          \
    X(TL_STRING)         \
    X(TL_CHARACTER)      \

#define X_KEYWORD_TOKENS    \
    X(TKW_IF, "if")         \
    X(TKW_ELSE, "else")     \
    X(TKW_FOR, "for")       \
    X(TKW_WHILE, "while")   \
    X(TKW_TRUE, "true")     \
    X(TKW_FALSE, "false")   \
    X(TKW_FUNC, "func")     \
    X(TKW_VAR, "var")       \
    X(TKW_NULL, "null")     \
    X(TKW_RETURN, "return") \

#define X_PRIMITIVE_TYPES_TOKENS  \
    X(TPT_VOID, "void")           \
    X(TPT_INT, "int")             \
    X(TPT_FLOAT, "float")         \
    X(TPT_STRING, "string")       \
    

enum TokenType {
    TOKEN_ILLEGAL_TOKEN,
    TOKEN_EOF,
    
    #define X(name, str) name,
        X_SYNTAX_TOKENS
    #undef X

    #define X(name) name,
        X_LITERAL_TOKENS
    #undef X

    #define X(name, str) name,
        X_KEYWORD_TOKENS
    #undef X

    #define X(name, str) name,
        X_PRIMITIVE_TYPES_TOKENS
    #undef X

    TOKEN_IDENTIFIER,

    TOKEN_COUNT
};

struct Token {
    TokenType type = TOKEN_ILLEGAL_TOKEN;
    u32 line = 404;

    DS::View<char> sv; // used for identifer names, string literals, the source_view

    union {
        int i;
        float f;
        char c;
        bool b;
    };

    Token() = default;
    Token(TokenType token_type, DS::View<char> sv, int line);

    static Token KeywordTokenFromSourceView(DS::View<char> sv, int line);
    static Token SyntaxTokenFromSourceView(DS::View<char> sv, int line);
    static Token LiteralTokenFromSourceView(DS::View<char> sv, int line);
    static Token PrimiveTypeTokenFromSourceView(DS::View<char> sv, int line);

    void print();
    const char* type_to_string() const;
};