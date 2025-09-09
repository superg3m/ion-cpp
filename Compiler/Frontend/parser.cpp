

#include "parser.hpp"
#include <cstdarg>
#include <cstdio>

// Date: May 13, 2025
// NOTE(Jovanni): Try to keep this updated
// Expression Precedence (highest at bottom)
//  └── Logical (||, &&)                       -- control flow style decisions
//       └── Equality (==, !=)                 -- same/different checks
//            └── Comparison (<, >, <=, >=).   -- ordering comparisons
//                 └── Bitwise (|, ^, &)       -- low-level operations
//                      └── Additive (+, -)     -- sequence-style math
//                           └── Multiplicative (*, /, %) -- scaling-style math
//                                └── Unary (!, ~, -, +, *, &) -- single-value ops
//                                     └── Primary (identifiers, literals, calls, grouping)

namespace Frontend {
    Parser::Parser(Memory::Allocator& allocator, const Container::Vector<Token>& tokens) : allocator(allocator), tokens(tokens) {}

    ASTNode* Parser::generate_ast(Memory::Allocator& allocator, const Container::Vector<Token>& tokens) {
        Parser parser = Parser(allocator, tokens);

        return ASTNode::Expression(allocator, parser.parse_expression());
    }

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
        LOG_ERROR("String: %s\n", token_strings[token.type]);
        LOG_ERROR("Error Line: %d | %s", token.line, String::sprintf(this->allocator, nullptr, fmt, args));
        va_end(args);

        RUNTIME_ASSERT(false);
    }

    Token Parser::consume_next_token() {
        return this->tokens[this->current++];
    }

    void Parser::expect(TokenType expected_type) {
        if (expected_type && peek_nth_token().type != expected_type) {
            this->report_error("Expected: %s | Got: %s\n", token_strings[expected_type], token_strings[peek_nth_token().type]);
        }

        this->consume_next_token();
    }

    bool Parser::consume_on_match(TokenType expected_type) {
        if (this->peek_nth_token().type == expected_type) {
            this->consume_next_token();
            return true;
        }

        return false;
    }

    // <primary> ::= INTEGER | FLOAT | TRUE | FALSE | STRING | PRIMITIVE_TYPE | IDENTIFIER | "(" <expression> ")"
    Expression* Parser::parse_primary_expression() {
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

        return nullptr;
    }

    // <multiplicative> ::= <primary> (("*" | "/" | "%" | "<<" | ">>") <primary>)*
    Expression* Parser::parse_multiplicative_expression() {
        Expression* expression = this->parse_primary_expression();

        while (this->consume_on_match(TOKEN_STAR) || this->consume_on_match(TOKEN_DIVISION) || this->consume_on_match(TOKEN_MODULUS)) {
            Token op = this->previous_token();
            Expression* right = this->parse_primary_expression();

            expression = Expression::Binary(this->allocator, op, expression, right, op.line);
        }

        return expression;
    }

    // <additive> ::= <multiplicative> (("+" | "-") <multiplicative>)*
    Expression* Parser::parse_additive_expression() {
        Expression* expression = this->parse_multiplicative_expression();
        while (this->consume_on_match(TOKEN_PLUS) || this->consume_on_match(TOKEN_MINUS)) {

            Token op = this->previous_token();
            Expression* right = this->parse_multiplicative_expression();

            expression = Expression::Binary(this->allocator, op, expression, right, op.line);
        }

        return expression;
    }

    Expression* Parser::parse_expression() {
        return this->parse_additive_expression();
    }
}