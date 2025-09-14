

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
    Expression* parse_unary_expression(Parser* parser, Memory::BaseAllocator* allocator);
    Expression* parse_multiplicative_expression(Parser* parser, Memory::BaseAllocator* allocator);
    Expression* parse_additive_expression(Parser* parser, Memory::BaseAllocator* allocator);
    Expression* parse_logical_expression(Parser* parser, Memory::BaseAllocator* allocator);
    Expression* parse_expression(Parser* parser, Memory::BaseAllocator* allocator);

    // <primary> ::= INTEGER | FLOAT | TRUE | FALSE | STRING | IDENTIFIER | "(" <expression> ")"
    Expression* parse_primary_expression(Parser* parser, Memory::BaseAllocator* allocator) {
        Token current_token = parser->peek_nth_token();

        if (parser->consume_on_match(TKW_TRUE) || parser->consume_on_match(TKW_FALSE)) {
            return Expression::Boolean(allocator, current_token.b, current_token.line);
        } else if (parser->consume_on_match(TL_INTEGER)) {
            return Expression::Integer(allocator, current_token.i, current_token.line);
        } else if (parser->consume_on_match(TL_FLOAT)) {
            return Expression::Float(allocator, current_token.f, current_token.line);
        } else if (parser->consume_on_match(TL_STRING)) {
            return Expression::String(allocator, current_token.sv, current_token.line);
        } else if (parser->consume_on_match(TS_LEFT_PAREN)) {
            Expression* expression = parse_expression(parser, allocator);
            parser->expect(TS_RIGHT_PAREN);

            return Expression::Grouping(allocator, expression, parser->previous_token().line);
        }

        return nullptr;
    
}
    // <unary> ::= (("-" | "+") <unary>) | <primary>
    Expression* parse_unary_expression(Parser* parser, Memory::BaseAllocator* allocator) {
        while (parser->consume_on_match(TS_PLUS) || parser->consume_on_match(TS_MINUS)) {
            Token op = parser->previous_token();
            Expression* operand = parse_unary_expression(parser, allocator);

            return Expression::Unary(allocator, op, operand, op.line);
        }

        return parse_primary_expression(parser, allocator);
    }

    // <multiplicative> ::= <unary> (("*" | "/" | "%" | "<<" | ">>") <unary>)*
    Expression* parse_multiplicative_expression(Parser* parser, Memory::BaseAllocator* allocator) {
        Expression* expression = parse_unary_expression(parser, allocator);

        while (parser->consume_on_match(TS_STAR) || parser->consume_on_match(TS_DIVISION) || parser->consume_on_match(TS_MODULUS)) {
            Token op = parser->previous_token();
            Expression* right = parse_unary_expression(parser, allocator);

            expression = Expression::Binary(allocator, op, expression, right, op.line);
        }

        return expression;
    }

    // <additive> ::= <multiplicative> (("+" | "-") <multiplicative>)*
    Expression* parse_additive_expression(Parser* parser, Memory::BaseAllocator* allocator) {
        Expression* expression = parse_multiplicative_expression(parser, allocator);

        while (parser->consume_on_match(TS_PLUS) || parser->consume_on_match(TS_MINUS)) {

            Token op = parser->previous_token();
            Expression* right = parse_multiplicative_expression(parser, allocator);

            expression = Expression::Binary(allocator, op, expression, right, op.line);
        }

        return expression;
    }

    // <logcal> ::= <additive> (("||" | "&&") <additive>)*
    Expression* parse_logical_expression(Parser* parser, Memory::BaseAllocator* allocator) {
        Expression* expression = parse_additive_expression(parser, allocator);

        while (parser->consume_on_match(TSL_OR) || parser->consume_on_match(TSL_AND)) {
            Token op = parser->previous_token();
            Expression* right = parse_additive_expression(parser, allocator);

            expression = Expression::Logical(allocator, op, expression, right, op.line);
        }

        return expression;
    }

    Expression* parse_expression(Parser* parser, Memory::BaseAllocator* allocator) {
        return parse_logical_expression(parser, allocator);
    }

    ASTNode* generate_ast(Memory::BaseAllocator* allocator, const DS::Vector<Token>& tokens) {
        Parser parser = Parser(allocator, tokens);

        return ASTNode::Expression(allocator, parse_expression(&parser, allocator));
    }
}