#include "ast.hpp"

namespace Frontend {
    bool type_check_expression(Expression* e) {
        switch (e->type) {
            case EXPRESSION_TYPE_INTEGER: {
                return "integer"; // (genuienly you could just use strings for the type checking)
            } break;
        }


        return false;
    }

    bool type_check_ast(ASTNode* node) {
        switch (node->type) {
            case AST_NODE_EXPRESSION: {
                Expression* e = node->expression;

            } break;

        }


        return false;
    }
}