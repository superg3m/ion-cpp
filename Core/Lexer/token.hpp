#pragma once

#include "../Container/container.hpp"

enum TokenType {
    TOKEN_ILLEGAL_TOKEN,
    TOKEN_EOF,
    
    TOKEN_SYNTAX_LEFT_PAREN, 
    TOKEN_SYNTAX_RIGHT_PAREN,
    TOKEN_SYNTAX_STAR,       
    TOKEN_SYNTAX_PLUS,       
    TOKEN_SYNTAX_MINUS,      
    TOKEN_SYNTAX_DIVISION,   
    TOKEN_SYNTAX_MODULUS,    

    TOKEN_LITERAL_INTEGER,     
    TOKEN_LITERAL_FLOAT,    
    TOKEN_LITERAL_STRING,   
    TOKEN_LITERAL_CHARACTER,
    TOKEN_LITERAL_TRUE,        
    TOKEN_LITERAL_FALSE, 

    TOKEN_KEYWORD_IF,
    TOKEN_KEYWORD_ELSE, 
    TOKEN_KEYWORD_FOR,         
    TOKEN_KEYWORD_WHILE,            
    TOKEN_KEYWORD_FUNC,
    TOKEN_KEYWORD_VAR,
    TOKEN_KEYWORD_NULL,      
    TOKEN_KEYWORD_RETURN, 
    
    TOKEN_IDENTIFIER,

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

    static Token KeywordTokenFromSourceView(Container::View<char> sv, int line);
    static Token SyntaxTokenFromSourceView(Container::View<char> sv, int line);
    static Token LiteralTokenFromSourceView(Container::View<char> sv, int line);

    void print();
    const char* type_to_string() const;
};