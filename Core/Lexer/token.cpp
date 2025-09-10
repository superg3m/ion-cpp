#include "token.hpp"
#include <stdlib.h>

Token::Token(TokenType token_type, Container::View<char> sv, int line) : sv(sv) {
    this->type = token_type;
    this->line = line;
}

Token Token::KeywordTokenFromSourceView(Container::View<char> sv, int line) {
    static Container::Hashmap<Container::View<char>, TokenType> syntax_map = {
        { Container::View<char>("if",     sizeof("if") - 1),     TOKEN_KEYWORD_IF     },
        { Container::View<char>("else",   sizeof("else") - 1),   TOKEN_KEYWORD_ELSE   },
        { Container::View<char>("for",    sizeof("for") - 1),    TOKEN_KEYWORD_FOR    },
        { Container::View<char>("while",  sizeof("while") - 1),  TOKEN_KEYWORD_WHILE  },
        { Container::View<char>("func",   sizeof("func") - 1),   TOKEN_KEYWORD_FUNC   },
        { Container::View<char>("var",    sizeof("var") - 1),    TOKEN_KEYWORD_VAR    },
        { Container::View<char>("null",   sizeof("null") - 1),   TOKEN_KEYWORD_NULL   },
        { Container::View<char>("return", sizeof("return") - 1), TOKEN_KEYWORD_RETURN },
    };

    Token ret = Token(); // Invalid
    if (syntax_map.has(sv)) {
        ret.type = syntax_map.get(sv);
        ret.line = line;
        ret.sv = sv;
    }

    return ret;
}

Token Token::SyntaxTokenFromSourceView(Container::View<char> sv, int line) {
    static Container::Hashmap<Container::View<char>, TokenType> syntax_map = {
        { Container::View<char>("+", sizeof("+") - 1), TOKEN_SYNTAX_PLUS },
        { Container::View<char>("-", sizeof("-") - 1), TOKEN_SYNTAX_MINUS },
        { Container::View<char>("*", sizeof("*") - 1), TOKEN_SYNTAX_STAR },
        { Container::View<char>("/", sizeof("/") - 1), TOKEN_SYNTAX_DIVISION },
        { Container::View<char>("%", sizeof("%") - 1), TOKEN_SYNTAX_MODULUS },
    };

    Token ret = Token(); // Invalid
    if (syntax_map.has(sv)) {
        ret.type = syntax_map.get(sv);
        ret.line = line;
        ret.sv = sv;
    }

    return ret;
}

Token Token::LiteralTokenFromSourceView(Container::View<char> sv, int line) {
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
        ret.type = TOKEN_LITERAL_TRUE;
        ret.b = true;

        return ret;
    } else if (String::equal(ret.sv.data, sv.length, "false", sizeof("false") - 1)) {
        ret.type = TOKEN_LITERAL_FALSE;
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

        stringify(TOKEN_SYNTAX_LEFT_PAREN), 
        stringify(TOKEN_SYNTAX_RIGHT_PAREN),
        stringify(TOKEN_SYNTAX_STAR),       
        stringify(TOKEN_SYNTAX_PLUS),       
        stringify(TOKEN_SYNTAX_MINUS),      
        stringify(TOKEN_SYNTAX_DIVISION),   
        stringify(TOKEN_SYNTAX_MODULUS),    

        stringify(TOKEN_LITERAL_INTEGER),    
        stringify(TOKEN_LITERAL_FLOAT),    
        stringify(TOKEN_LITERAL_STRING),    
        stringify(TOKEN_LITERAL_CHARACTER),
        stringify(TOKEN_LITERAL_TRUE),    
        stringify(TOKEN_LITERAL_FALSE),

        stringify(TOKEN_KEYWORD_IF),
        stringify(TOKEN_KEYWORD_ELSE), 
        stringify(TOKEN_KEYWORD_FOR),         
        stringify(TOKEN_KEYWORD_WHILE),            
        stringify(TOKEN_KEYWORD_FUNC),
        stringify(TOKEN_KEYWORD_VAR),
        stringify(TOKEN_KEYWORD_NULL),      
        stringify(TOKEN_KEYWORD_RETURN),  
        
        stringify(TOKEN_IDENTIFIER),
    };

    STATIC_ASSERT(ArrayCount(token_strings) == TOKEN_COUNT);

    return token_strings[this->type];
}