#pragma once

#include "../Memory/allocator.hpp"
#include "../Lexer/token.hpp"

struct Parser {
    Parser(Memory::BaseAllocator* allocator, const DS::Vector<Token>& tokens) : allocator(allocator), tokens(tokens) {}

    Token peek_nth_token(int n = 0);
    Token previous_token();
    void report_error(const char* fmt, ...);
    Token consume_next_token();
    Token expect(TokenType expected_type);
    bool consume_on_match(TokenType expected_type);
private:
    Memory::BaseAllocator* allocator;
    const DS::Vector<Token>& tokens;
    int current = 0;
};