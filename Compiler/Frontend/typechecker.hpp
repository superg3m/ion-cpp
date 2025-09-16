#pragma once

#include "ast.hpp"

namespace Frontend {
    Type type_check_expression(Expression* e) {
        switch (e->type) {
            case EXPRESSION_TYPE_INTEGER: {
                return Type({}, DS::View<char>("int", sizeof("int") - 1));
            } break;

            case EXPRESSION_TYPE_FLOAT: {
                return Type({}, DS::View<char>("float", sizeof("float") - 1));
            } break;
            
            case EXPRESSION_TYPE_STRING: {
                return Type({}, DS::View<char>("string", sizeof("string") - 1));
            } break;

            case EXPRESSION_TYPE_BINARY_OPERATION: {
                DS::View<char> op = e->binary->operation.sv;
                Type left_type = type_check_expression(e->binary->left);
                Type right_type = type_check_expression(e->binary->right);

                if (left_type != right_type) {
                    const char* fmt = "[TypeChecker BinaryOp Error]: %.*s and %.*s are incompatible types for op: %.*s\n";
                    LOG_ERROR(fmt, left_type.name.length, left_type.name.data, right_type.name.length, right_type.name.data, op.length, op.data);
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
        
        return Type();
    }

    Type type_check_decleration(Decleration* decl) {
        switch (decl->type) {
            case DECLERATION_TYPE_VARIABLE: {
                if (decl->variable->rhs) {
                    Type expression_type = type_check_expression(decl->variable->rhs);

                    if (decl->variable->type.name.data == nullptr) {
                        decl->variable->type = expression_type;
                        return expression_type;
                    }

                    if (decl->variable->type != expression_type) {
                        LOG_ERROR("[TypeChecker VarDecl Error]: Line: %d\n", decl->variable->line);
                    }
                }

                return decl->variable->type;
            } break;

            case DECLERATION_TYPE_FUNCTION: {
                return Type({}, DS::View<char>("nothing", sizeof("nothing") - 1));
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        RUNTIME_ASSERT(false);
        
        return Type();
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