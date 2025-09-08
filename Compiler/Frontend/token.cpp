#include "token.hpp"
#include <stdlib.h>

Token token_from_string(TokenType token_type, Container::View<char> sv, int line) {
    char buffer[128];
    RUNTIME_ASSERT(sv.length < sizeof(buffer));

    Token token = Token(sv);
    token.type = token_type;
    token.line = line;

    Memory::copy(buffer, sv.length, sv.data, sv.length);
    buffer[sv.length] = '\0';
    
    char* endptr_int = NULL;
    long i = strtol(buffer, &endptr_int, 10);
    if ((size_t)(endptr_int - buffer) == sv.length) {
        token.type = TOKEN_INTEGER_LITERAL;
        token.i = (int)i;

        return token;
    }
    
    char* endptr = NULL;
    float f = strtof(buffer, &endptr);
    if ((size_t)(endptr - buffer) == sv.length) {
        token.type = TOKEN_FLOAT_LITERAL;
        token.f = f;

        return token;
    }
    
    return token;
}


TokenType token_type_from_syntax(const char* buffer, byte_t length) {
    static Container::Hashmap<Container::View<char>, TokenType> syntax_map = {
        { Container::View<char>("+", sizeof("+") - 1), TOKEN_PLUS },
        { Container::View<char>("-", sizeof("-") - 1), TOKEN_MINUS },
        { Container::View<char>("*", sizeof("*") - 1), TOKEN_STAR },
        { Container::View<char>("/", sizeof("/") - 1), TOKEN_DIVISION },
        { Container::View<char>("%", sizeof("%") - 1), TOKEN_MODULUS },
    };

    TokenType ret = TOKEN_ILLEGAL_TOKEN;
    Container::View<char> key = Container::View<char>((char*)buffer, length);
    if (syntax_map.has(key)) {
        ret = syntax_map.get(key);
    }

    return ret;
}