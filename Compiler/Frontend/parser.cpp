

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
    Expression* parse_primary_expression(Parser* parser, Memory::BaseAllocator* allocator);
    Expression* parse_multiplicative_expression(Parser* parser, Memory::BaseAllocator* allocator);
    Expression* parse_additive_expression(Parser* parser, Memory::BaseAllocator* allocator);
    Expression* parse_expression(Parser* parser, Memory::BaseAllocator* allocator);

    // <primary> ::= INTEGER | FLOAT | TRUE | FALSE | STRING | IDENTIFIER | "(" <expression> ")"
    Expression* parse_primary_expression(Parser* parser, Memory::BaseAllocator* allocator) {
        Token current_token = parser->peek_nth_token();

        if (parser->consume_on_match(TOKEN_KEYWORD_TRUE) || parser->consume_on_match(TOKEN_KEYWORD_FALSE)) {
            return Expression::Boolean(allocator, current_token.b, current_token.line);
        } else if (parser->consume_on_match(TOKEN_LITERAL_INTEGER)) {
            return Expression::Integer(allocator, current_token.i, current_token.line);
        } else if (parser->consume_on_match(TOKEN_LITERAL_FLOAT)) {
            return Expression::Float(allocator, current_token.f, current_token.line);
        } else if (parser->consume_on_match(TOKEN_LITERAL_STRING)) {
            return Expression::String(allocator, current_token.sv, current_token.line);
        } else if (parser->consume_on_match(TOKEN_SYNTAX_LEFT_PAREN)) {
            Expression* expression = parse_expression(parser, allocator);
            parser->expect(TOKEN_SYNTAX_RIGHT_PAREN);

            return Expression::Grouping(allocator, expression, parser->previous_token().line);
        }

        return nullptr;
    }

    // <multiplicative> ::= <primary> (("*" | "/" | "%" | "<<" | ">>") <primary>)*
    Expression* parse_multiplicative_expression(Parser* parser, Memory::BaseAllocator* allocator) {
        Expression* expression = parse_primary_expression(parser, allocator);

        while (parser->consume_on_match(TOKEN_SYNTAX_STAR) || parser->consume_on_match(TOKEN_SYNTAX_DIVISION) || parser->consume_on_match(TOKEN_SYNTAX_MODULUS)) {
            Token op = parser->previous_token();
            Expression* right = parse_primary_expression(parser, allocator);

            expression = Expression::Binary(allocator, op, expression, right, op.line);
        }

        return expression;
    }

    // <additive> ::= <multiplicative> (("+" | "-") <multiplicative>)*
    Expression* parse_additive_expression(Parser* parser, Memory::BaseAllocator* allocator) {
        Expression* expression = parse_multiplicative_expression(parser, allocator);
        while (parser->consume_on_match(TOKEN_SYNTAX_PLUS) || parser->consume_on_match(TOKEN_SYNTAX_MINUS)) {

            Token op = parser->previous_token();
            Expression* right = parse_multiplicative_expression(parser, allocator);

            expression = Expression::Binary(allocator, op, expression, right, op.line);
        }

        return expression;
    }

    // <logcal> ::= <additive> (("||" | "&&") <additive>)*
    Expression* parse_lgocal_expression(Parser* parser, Memory::BaseAllocator* allocator) {
        Expression* expression = parse_multiplicative_expression(parser, allocator);
        while (parser->consume_on_match(TOKEN_SYNTAX_LOGICAL_OR) || parser->consume_on_match(TOKEN_SYNTAX_LOGICAL_AND)) {

            Token op = parser->previous_token();
            Expression* right = parse_multiplicative_expression(parser, allocator);

            expression = Expression::Binary(allocator, op, expression, right, op.line);
        }

        return expression;
    }

    Expression* parse_expression(Parser* parser, Memory::BaseAllocator* allocator) {
        return parse_additive_expression(parser, allocator);
    }

    ASTNode* generate_ast(Memory::BaseAllocator* allocator, const DS::Vector<Token>& tokens) {
        Parser parser = Parser(allocator, tokens);

        return ASTNode::Expression(allocator, parse_expression(&parser, allocator));
    }
}