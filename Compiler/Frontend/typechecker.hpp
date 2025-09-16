#pragma once

#include "ast.hpp"

namespace Frontend {
    void type_check_ast(ASTNode* node);

    struct VariableSymbol {
        DS::View<char> name;
        Type type;
        // Score scope; // This is actually perfect because you can just scope chain to the global scope to find it!
    };

    /*
        Only allowed to have Function decls in
        global scope
    */
    struct FunctionSymbol {
        // DS::View<char> name;
        // Type return_type;
    };

    DS::Hashmap<DS::View<char>, Type> variable_symbol_table;

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
                    RUNTIME_ASSERT(false);
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
                        variable_symbol_table.put(decl->variable->variable_name, decl->variable->type);
                        return expression_type;
                    }

                    if (decl->variable->type != expression_type) {
                        LOG_ERROR("[TypeChecker VarDecl Error]: Line: %d\n", decl->variable->line);
                        RUNTIME_ASSERT(false);
                    }
                }

                variable_symbol_table.put(decl->variable->variable_name, decl->variable->type);

                return decl->variable->type;
            } break;

            case DECLERATION_TYPE_FUNCTION: {
                for (ASTNode* node : decl->function->body) {
                    type_check_ast(node);
                }

                return Type({}, DS::View<char>("nothing", sizeof("nothing") - 1));
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        RUNTIME_ASSERT(false);
        
        return Type();
    }

    Type type_check_statement(Statement* s) {
        switch (s->type) {
            case STATEMENT_TYPE_ASSIGNMENT: {
                if (!variable_symbol_table.has(s->assignment->variable_name)) {
                    RUNTIME_ASSERT_MSG(
                        false, "Error Line: %d | Undeclared identifier '%.*s'\n", 
                        s->assignment->line, s->assignment->variable_name.length, s->assignment->variable_name.data
                    );
                }

                Type variable_type = variable_symbol_table.get(s->assignment->variable_name);
                Type expression_type = type_check_expression(s->assignment->rhs);
                if (variable_type != expression_type) {
                    RUNTIME_ASSERT_MSG(
                        false, "Error Line: %d | Assignment of %.*s to %.*s is not allowed!\n",
                        s->assignment->line, variable_type.name.length, variable_type.name.data, 
                        expression_type.name.length, expression_type.name.data
                    );
                }

                return variable_type;
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
                variable_symbol_table = DS::Hashmap<DS::View<char>, Type>(&Memory::global_general_allocator);
                for (Decleration* decl : p->declerations) {
                    type_check_decleration(decl);
                }
            } break;

            case AST_NODE_EXPRESSION: {
                Expression* e = node->expression;
                type_check_expression(e);
            } break;

            case AST_NODE_STATEMENT: {
                Statement* s = node->statement;
                type_check_statement(s);
            } break;

            case AST_NODE_DECLERATION: {
                Decleration* decl = node->decleration;
                type_check_decleration(decl);
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }
    }
}