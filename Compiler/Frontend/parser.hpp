#pragma once

#include <Core/core.hpp>
#include "ast.hpp"

namespace Frontend {
    struct Parser {
        static ASTNode* generate_ast(Memory::BaseAllocator* allocator, const DS::Vector<Token>& tokens);

    private:
        Memory::BaseAllocator* allocator;
        const DS::Vector<Token>& tokens;
        int current = 0;

        Parser(Memory::BaseAllocator* allocator, const DS::Vector<Token>& tokens);
        Token peek_nth_token(int n = 0);
        Token previous_token();
        void report_error(const char* fmt, ...);
        Token consume_next_token();
        void expect(TokenType expected_type);
        bool consume_on_match(TokenType expected_type);
        Expression* parse_primary_expression();
        Expression* parse_multiplicative_expression();
        Expression* parse_additive_expression();
        Expression* parse_expression();
    };
}