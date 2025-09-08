#include "parser.hpp"
#include <cstdarg>
#include <cstdio>

struct T_Parser {
    Memory::Allocator& allocator;
    const Container::Vector<Token>& tokens;
    int current;

    T_Parser(Memory::Allocator& allocator, const Container::Vector<Token>& tokens) : allocator(allocator), tokens(tokens) {}

    Token peek_nth_token(int n = 0) {
        RUNTIME_ASSERT(this->current + n < this->tokens.count());

        return this->tokens[this->current + n];
    }

    Token previous_token() {
        return this->tokens[this->current - 1];
    }

    void report_error(char* fmt, ...) {
        Token token = this->peek_nth_token();

        va_list args;
        va_start(args, fmt);
        LOG_ERROR("String: %s\n", token_strings[token.type]);
        LOG_ERROR("Error Line: %d | %s", token.line, String::sprintf(this->allocator, nullptr, fmt, args));
        va_end(args);

        RUNTIME_ASSERT(false);
    }

    Token consume_next_token() {
        return this->tokens[this->current++];
    }

    void expect(TokenType expected_type) {
        if (expected_type && peek_nth_token().type != expected_type) {
            this->report_error("Expected: %s | Got: %s\n", (char*)token_strings[expected_type], token_strings[peek_nth_token().type]);
        }

        this->consume_next_token();
    }

    bool consume_on_match(TokenType expected_type) {
        if (this->peek_nth_token().type == expected_type) {
            this->consume_next_token();
            return true;
        }

        return false;
    }

    // <primary> ::= INTEGER | FLOAT | TRUE | FALSE | STRING | PRIMITIVE_TYPE | IDENTIFIER | "(" <expression> ")"
    Expression* parse_primary_expression() {
        Token current_token = this->peek_nth_token();

        if (this->consume_on_match(TOKEN_INTEGER_LITERAL)) {
            return Expression::Integer(this->allocator, current_token.i, current_token.line);
        } else if (this->consume_on_match(TOKEN_FLOAT_LITERAL)) {
            return Expression::Float(this->allocator, current_token.f, current_token.line);
        } else if (this->consume_on_match(TOKEN_LEFT_PAREN)) {
            Expression* expression = this->parse_expression();
            expect(TOKEN_RIGHT_PAREN);

            return Expression::Grouping(this->allocator, expression, previous_token().line);
        }
    }

    Expression* parse_expression() {
        return parse_primary_expression();
    }
};

namespace Frontend::Parser {
    ASTNode* generate_ast(Memory::Allocator& allocator, const Container::Vector<Token>& tokens) {
        T_Parser parser = T_Parser(allocator, tokens);

        return ASTNode::Expression(allocator, parser.parse_expression());
    }
}