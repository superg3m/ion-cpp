#pragma once

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
            
            case EXPRESSION_TYPE_STRING: {
                return DS::View<char>("string", sizeof("string") - 1); // (genuienly you could just use strings for the type checking)
            } break;

            case EXPRESSION_TYPE_BINARY_OPERATION: {
                DS::View<char> op = e->binary->operation.sv;
                DS::View<char> left_type = type_check_expression(e->binary->left);
                DS::View<char> right_type = type_check_expression(e->binary->right);
                if (!String::equal(left_type, right_type)) {
                    const char* fmt = "[TypeChecker BinaryOp Error]: %.*s and %.*s are incompatible types for op: %.*s\n";
                    LOG_ERROR(fmt, left_type.length, left_type.data, right_type.length, right_type.data, op.length, op.data);
                    LOG_ERROR("[TypeChecker BinaryOp Error]: Line: %d\n", e->binary->line);
                    // RUNTIME_ASSERT(false);
                }

                return left_type;
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        RUNTIME_ASSERT(false);
        
        return DS::View<char>();
    }

    DS::View<char> type_check_decleration(Decleration* decl) {
        switch (decl->type) {
            case DECLERATION_TYPE_VARIABLE: {
                DS::View<char> expression_type_name = type_check_expression(decl->variable->right);

                if (!String::equal(decl->variable->type_name, expression_type_name)) {
                    LOG_ERROR("[TypeChecker VarDecl Error]: Line: %d\n", decl->variable->line);
                }

                return decl->variable->type_name;
            } break;

            case DECLERATION_TYPE_FUNCTION: {
                return DS::View<char>("nothing", sizeof("nothing") - 1);
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        RUNTIME_ASSERT(false);
        
        return DS::View<char>();
    }

    void type_check_ast(ASTNode* node) {
        switch (node->type) {
            case AST_NODE_PROGRAM: {
                Program* p = node->program;
                for (Decleration* decl : p->declerations) {
                    type_check_decleration(decl);
                }
            } break;

            case AST_NODE_EXPRESSION: {
                Expression* e = node->expression;
                type_check_expression(e);
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }
    }
}