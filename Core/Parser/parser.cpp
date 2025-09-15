

#include "parser.hpp"
#include <cstdarg>
#include <cstdio>

Token Parser::peek_nth_token(int n) {
    if (this->current + n >= this->tokens.count()) {
        return Token(); // invalid
    }

    return this->tokens[this->current + n];
}

Token Parser::previous_token() {
    return this->tokens[this->current - 1];
}

void Parser::report_error(const char* fmt, ...) {
    Token token = this->peek_nth_token();

    va_list args;
    va_start(args, fmt);
    LOG_ERROR("String: %s\n", token.type_to_string());
    LOG_ERROR("Error Line: %d | %s", token.line, String::sprintf(this->allocator, nullptr, fmt, args));
    va_end(args);

    RUNTIME_ASSERT(false);
}

Token Parser::consume_next_token() {
    return this->tokens[this->current++];
}

Token Parser::expect(TokenType expected_type) {
    Token expected_token = Token();
    expected_token.type = expected_type;

    Token got_token = peek_nth_token();

    const char* expected_str = expected_token.type_to_string();
    const char* got_str = got_token.type_to_string();

    if (got_token.type != expected_token.type) {
        this->report_error("Expected: %s | Got: %s\n", expected_str, got_str);
    }

    return this->consume_next_token();
}

bool Parser::consume_on_match(TokenType expected_type) {
    if (this->peek_nth_token().type == expected_type) {
        this->consume_next_token();
        return true;
    }

    return false;
}