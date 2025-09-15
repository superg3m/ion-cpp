

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
    Expression* parse_primary_expression(Parser* parser);
    Expression* parse_unary_expression(Parser* parser);
    Expression* parse_multiplicative_expression(Parser* parser);
    Expression* parse_additive_expression(Parser* parser);
    Expression* parse_logical_expression(Parser* parser);
    Expression* parse_expression(Parser* parser);

    // <primary> ::= INTEGER | FLOAT | TRUE | FALSE | STRING | IDENTIFIER | "(" <expression> ")"
    Expression* parse_primary_expression(Parser* parser) {
        Token current_token = parser->peek_nth_token();

        if (parser->consume_on_match(TKW_TRUE) || parser->consume_on_match(TKW_FALSE)) {
            return Expression::Boolean(parser->allocator, current_token.b, current_token.line);
        } else if (parser->consume_on_match(TL_INTEGER)) {
            return Expression::Integer(parser->allocator, current_token.i, current_token.line);
        } else if (parser->consume_on_match(TL_FLOAT)) {
            return Expression::Float(parser->allocator, current_token.f, current_token.line);
        } else if (parser->consume_on_match(TL_STRING)) {
            return Expression::String(parser->allocator, current_token.sv, current_token.line);
        } else if (parser->consume_on_match(TS_LEFT_PAREN)) {
            Expression* expression = parse_expression(parser);
            parser->expect(TS_RIGHT_PAREN);

            return Expression::Grouping(parser->allocator, expression, parser->previous_token().line);
        }

        return nullptr;
    
}
    // <unary> ::= (("-" | "+") <unary>) | <primary>
    Expression* parse_unary_expression(Parser* parser) {
        if (parser->consume_on_match(TS_PLUS) || parser->consume_on_match(TS_MINUS)) {
            Token op = parser->previous_token();
            Expression* operand = parse_unary_expression(parser);

            return Expression::Unary(parser->allocator, op, operand, op.line);
        }

        return parse_primary_expression(parser);
    }

    // <multiplicative> ::= <unary> (("*" | "/" | "%" | "<<" | ">>") <unary>)*
    Expression* parse_multiplicative_expression(Parser* parser) {
        Expression* expression = parse_unary_expression(parser);

        while (parser->consume_on_match(TS_STAR) || parser->consume_on_match(TS_DIVISION) || parser->consume_on_match(TS_MODULUS)) {
            Token op = parser->previous_token();
            Expression* right = parse_unary_expression(parser);

            expression = Expression::Binary(parser->allocator, op, expression, right, op.line);
        }

        return expression;
    }

    // <additive> ::= <multiplicative> (("+" | "-") <multiplicative>)*
    Expression* parse_additive_expression(Parser* parser) {
        Expression* expression = parse_multiplicative_expression(parser);

        while (parser->consume_on_match(TS_PLUS) || parser->consume_on_match(TS_MINUS)) {

            Token op = parser->previous_token();
            Expression* right = parse_multiplicative_expression(parser);

            expression = Expression::Binary(parser->allocator, op, expression, right, op.line);
        }

        return expression;
    }

    // <logcal> ::= <additive> (("||" | "&&") <additive>)*
    Expression* parse_logical_expression(Parser* parser) {
        Expression* expression = parse_additive_expression(parser);

        while (parser->consume_on_match(TSL_OR) || parser->consume_on_match(TSL_AND)) {
            Token op = parser->previous_token();
            Expression* right = parse_additive_expression(parser);

            expression = Expression::Logical(parser->allocator, op, expression, right, op.line);
        }

        return expression;
    }

    Expression* parse_expression(Parser* parser) {
        return parse_logical_expression(parser);
    }

    // <variable_decleration> ::= "var" <identifier> ":" <identifer> "=" <expression> ";"
    Decleration* parse_variable_decleration(Parser* parser) {
        Token var = parser->expect(TKW_VAR);
        Token name = parser->expect(TOKEN_IDENTIFIER);
        parser->expect(TS_COLON);
        Token type_name = parser->expect(TOKEN_IDENTIFIER);
        parser->expect(TSA_EQUALS);

        Expression* e = parse_expression(parser);

        parser->expect(TS_COMMA);

        return Decleration::Variable(parser->allocator, name.sv,type_name.sv, e, var.line);
    }

    Decleration* parse_decleration(Parser* parser) {
        Token current_token = parser->peek_nth_token();

        if (current_token.type == TKW_VAR) {
            return parse_variable_decleration(parser);
        } else if (current_token.type == TKW_FUNC) {

        }

        RUNTIME_ASSERT(false);
        return nullptr;
    }

    Program* parse_program(Parser* parser) {
        Program* program = (Program*)parser->allocator->malloc(sizeof(Program));
        program->declerations = DS::Vector<Decleration*>(1, parser->allocator);

        while (parser->peek_nth_token().type != TOKEN_ILLEGAL_TOKEN) {
            program->declerations.push(parse_decleration(parser));
        }

        return program;
    }

    ASTNode* generate_ast(Memory::BaseAllocator* allocator, const DS::Vector<Token>& tokens) {
        Parser parser = Parser(allocator, tokens);

        return ASTNode::Program(allocator, parse_program(&parser));
        // return ASTNode::Expression(parser.allocator, parse_expression(&parser));
    }
}