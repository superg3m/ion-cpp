#include "ast.hpp"

namespace Frontend {
    DS::View<char> type_check_expression(Expression* e) {
        switch (e->type) {
            case EXPRESSION_TYPE_INTEGER: {
                return DS::View<char>("int", sizeof("int") - 1); // (genuienly you could just use strings for the type checking)
            } break;

            case EXPRESSION_TYPE_FLOAT: {
                return DS::View<char>("float", sizeof("float") - 1); // (genuienly you could just use strings for the type checking)
            } break;

            case EXPRESSION_TYPE_BINARY_OPERATION: {
                DS::View<char> op = e->binary->operation.sv;
                DS::View<char> left_type = type_check_expression(e->binary->left);
                DS::View<char> right_type = type_check_expression(e->binary->right);
                RUNTIME_ASSERT_MSG(String::equal(left_type, right_type), "Left and Right are not the same type!\n");
            } break;
        }
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