

#include "parser.hpp"
#include "types.hpp"
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
    /*
        I need to do scopes on all these things
    */

    Expression* parse_primary_expression(Parser* parser);
    Expression* parse_unary_expression(Parser* parser);
    Expression* parse_multiplicative_expression(Parser* parser);
    Expression* parse_additive_expression(Parser* parser);
    Expression* parse_logical_expression(Parser* parser);
    Expression* parse_expression(Parser* parser);

    Decleration* parse_decleration(Parser* parser);
    Statement* parse_statement(Parser* parser);

    Expression* parse_function_call_expression(Parser* parser) {
        Token identifier = parser->expect(TOKEN_IDENTIFIER);
        parser->expect(TS_LEFT_PAREN);
        parser->expect(TS_RIGHT_PAREN);

        return Expression::FunctionCall(parser->allocator, identifier.sv, Type(), identifier.line);
    }

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
        } else if (current_token.type == TOKEN_IDENTIFIER) {
            Token next_token = parser->peek_nth_token(1);
            if (next_token.type == TS_LEFT_PAREN) {
                return parse_function_call_expression(parser);
            }
           
            RUNTIME_ASSERT(false); 
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

    // <type> ::= "int" | "float" | "string"
    Type parse_type(Parser* parser) {
        DS::Hashmap<TokenType, bool> primitive_type_map = {
            #define X(name, str) {name, true},
                X_PRIMITIVE_TYPES_TOKENS
            #undef X
        };

        while (parser->peek_nth_token().type != TOKEN_ILLEGAL_TOKEN) {
            Token t = parser->consume_next_token();

            if (primitive_type_map.has(t.type)) {
                break;
            }
        }

        Token type_token = parser->previous_token();
        return Type(type_token.sv, type_token.type, {});
    }

    // <variable_decleration> ::= "var" <identifier> ":" <type> "=" <expression> ";"
    Decleration* parse_variable_decleration(Parser* parser) {
        Token var = parser->expect(TKW_VAR);
        Token variable_name = parser->expect(TOKEN_IDENTIFIER);
        
        Type type = Type();
        Expression* rhs = nullptr;
        
        if (parser->consume_on_match(TSA_INFER_EQUALS)) {
            rhs = parse_expression(parser);
        } else {
            parser->expect(TS_COLON);
            type = parse_type(parser);

            if (parser->consume_on_match(TSA_EQUALS)) {
                rhs = parse_expression(parser);
            }
        }

        parser->expect(TS_SEMI_COLON);

        return Decleration::Variable(parser->allocator, variable_name.sv, type, rhs, var.line);
    }

    void parse_code_block(Parser* parser, DS::Vector<ASTNode*>& out_code_block) {
        parser->expect(TS_LEFT_CURLY);
        while(parser->peek_nth_token().type != TOKEN_ILLEGAL_TOKEN && !parser->consume_on_match(TS_RIGHT_CURLY)) {
            Decleration* decleration = parse_decleration(parser);
            if (decleration) {
                out_code_block.push(ASTNode::Decleration(parser->allocator, decleration));
                continue;
            }

            Statement* statement = parse_statement(parser);
            if (statement) {
                out_code_block.push(ASTNode::Statement(parser->allocator, statement));
                continue;
            }
        
            RUNTIME_ASSERT(false);
        }
    }

    // <function_decleration> ::= "func" <identifier> "(" ")" "->" <type> "{" <code_block> "}"
    Decleration* parse_function_decleration(Parser* parser) {
        Token func = parser->expect(TKW_FUNC);
        Token function_name = parser->expect(TOKEN_IDENTIFIER);
        parser->expect(TS_LEFT_PAREN);
        parser->expect(TS_RIGHT_PAREN);
        parser->expect(TS_RIGHT_ARROW);
        Type return_type = parse_type(parser);

        DS::Vector<ASTNode*> body = DS::Vector<ASTNode*>(parser->allocator, 1);
        parse_code_block(parser, body);

        return Decleration::Function(parser->allocator, function_name.sv, return_type, body, func.line);
    }

    Statement* parse_assignment_statement(Parser* parser) {
        Token identifer = parser->consume_next_token();
        // Type idenfier_type = variable_symbol_table.get(identifer.sv).type;
        parser->expect(TSA_EQUALS);
        Expression* rhs = parse_expression(parser);
        parser->expect(TS_SEMI_COLON);

        return Statement::Assignment(parser->allocator, identifer.sv, rhs, identifer.line);
    }

    Statement* parse_return_statement(Parser* parser) {
        Token return_token = parser->expect(TKW_RETURN);
        Expression* expression = parse_expression(parser);
        parser->expect(TS_SEMI_COLON);

        return Statement::Return(parser->allocator, expression, return_token.line);
    }

    Statement* parse_statement(Parser* parser) {
        Token current_token = parser->peek_nth_token();

        if (current_token.type == TOKEN_IDENTIFIER) {
            Token next_token = parser->peek_nth_token(1);
            if (next_token.type == TS_LEFT_BRACKET) {
                // return parse_function_call_statement(parser);
            } else if (next_token.type == TSA_EQUALS) {
                return parse_assignment_statement(parser);
            } else {
                RUNTIME_ASSERT(false);
            } 
        } else if (current_token.type == TKW_RETURN) {
            return parse_return_statement(parser);
        }

        return nullptr;
    }

    Decleration* parse_decleration(Parser* parser) {
        Token current_token = parser->peek_nth_token();

        if (current_token.type == TKW_VAR) {
            return parse_variable_decleration(parser);
        } else if (current_token.type == TKW_FUNC) {
            return parse_function_decleration(parser);
        }

        return nullptr;
    }

    Program* parse_program(Parser* parser) {
        Program* program = (Program*)parser->allocator->malloc(sizeof(Program));
        program->declerations = DS::Vector<Decleration*>(parser->allocator, 1);

        while (parser->peek_nth_token().type != TOKEN_ILLEGAL_TOKEN) {
            program->declerations.push(parse_decleration(parser));
        }

        return program;
    }

    ASTNode* generate_ast(Memory::BaseAllocator* allocator, const DS::Vector<Token>& tokens) {
        Parser parser = Parser(allocator, tokens);

        return ASTNode::Program(allocator, parse_program(&parser));
    }
}