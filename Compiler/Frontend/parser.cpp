#include "parser.hpp"

struct T_Parser {
    const Container::Vector<Token>& m_tokens;
    Token tok;
    int current;

    T_Parser(const Container::Vector<Token>& tokens) : m_tokens(tokens) {}

    // <primary> ::= INTEGER | FLOAT | TRUE | FALSE | STRING | PRIMITIVE_TYPE | IDENTIFIER | "(" <expression> ")"
    ASTNode* parse_primary_expression() {
        if (parser_consume_on_match(parser, SPL_TOKEN_INTEGER_LITERAL)) {
            SPL_Token tok = parser_previous_token(parser);
            return integer_expression_create(parser->tok.i, tok.line);
        } else if (parser_consume_on_match(parser, SPL_TOKEN_FLOAT_LITERAL)) {
            SPL_Token tok = parser_previous_token(parser);
            return float_expression_create(parser->tok.f, tok.line);
        } else if (parser_consume_on_match(parser, SPL_TOKEN_TRUE)) {
            return bool_expression_create(true, parser_previous_token(parser).line);
        } else if (parser_consume_on_match(parser, SPL_TOKEN_FALSE)) {
            return bool_expression_create(false, parser_previous_token(parser).line);
        } else if (parser_consume_on_match(parser, SPL_TOKEN_STRING_LITERAL)) {
            SPL_Token tok = parser_previous_token(parser);
            return string_expression_create(tok.name, tok.line);
        } else if (parser_consume_on_match(parser, SPL_TOKEN_LEFT_PAREN)) {
            Expression* expression = parse_expression(parser);
            parser_expect(parser, SPL_TOKEN_RIGHT_PAREN);

            return grouping_expression_create(expression, parser_previous_token(parser).line);
        } else {
            SPL_Token tok = parser_consume_next_token(parser);
            return identifier_expression_create(tok.name, tok.line);
        }
    }

    ASTNode* parse_expression() {
        return nullptr;
    }
};

namespace Frontend::Parser {
    ASTNode* generate_ast(const Container::Vector<Token>& tokens) {
        T_Parser parser = T_Parser(tokens);

        return parser.parse_expression();
    }
}