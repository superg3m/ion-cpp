#pragma once

#include "ast.hpp"

namespace Frontend {
    struct TypeEnvironment {
        TypeEnvironment* parent = nullptr;

        TypeEnvironment(TypeEnvironment* parent) {
            this->parent = parent;
        }

        bool has_var(DS::View<char> key) {
            TypeEnvironment* current = this;
            while (current != nullptr) {
                if (current->variables.has(key)) {
                    return true;
                }

                current = current->parent;
            }

            return false;
        }

        void put_var(DS::View<char> key, VariableDecleration* value) {
            RUNTIME_ASSERT(!this->has_var(key));
            
            this->variables.put(key, value);
        }

        VariableDecleration* get_var(DS::View<char> key) {
            RUNTIME_ASSERT(this->has_var(key));
            
            TypeEnvironment* current = this;
            while (current != nullptr) {
                if (current->variables.has(key)) {
                    return current->variables.get(key);
                }

                current = current->parent;
            }

            return nullptr;
        }

        // ----------------------------------------

        bool has_func(DS::View<char> key) {
            TypeEnvironment* current = this;
            while (current != nullptr) {
                if (current->functions.has(key)) {
                    return true;
                }

                current = current->parent;
            }

            return false;
        }

        void put_func(DS::View<char> key, FunctionDecleration* value) {
            RUNTIME_ASSERT(!this->has_var(key));
            
            this->functions.put(key, value);
        }

        FunctionDecleration* get_func(DS::View<char> key) {
            RUNTIME_ASSERT(this->has_func(key));
            
            TypeEnvironment* current = this;
            while (current != nullptr) {
                if (current->functions.has(key)) {
                    return current->functions.get(key);
                }

                current = current->parent;
            }

            return nullptr;
        }

    private:
        DS::Hashmap<DS::View<char>, VariableDecleration*> variables = DS::Hashmap<DS::View<char>, VariableDecleration*>(&Memory::global_general_allocator);
        DS::Hashmap<DS::View<char>, FunctionDecleration*> functions = DS::Hashmap<DS::View<char>, FunctionDecleration*>(&Memory::global_general_allocator);
    };

    void type_check_ast_helper(ASTNode* node, TypeEnvironment* env);

    Type type_check_expression(Expression* e, TypeEnvironment* env) {
        switch (e->type) {
            case EXPRESSION_TYPE_INTEGER: {
                return Type(DS::View<char>("int", sizeof("int") - 1), TPT_INT, {});
            } break;

            case EXPRESSION_TYPE_FLOAT: {
                return Type(DS::View<char>("float", sizeof("float") - 1), TPT_FLOAT, {});
            } break;
            
            case EXPRESSION_TYPE_STRING: {
                return Type(DS::View<char>("string", sizeof("string") - 1), TPT_STRING, {});
            } break;

            case EXPRESSION_TYPE_BINARY_OPERATION: {
                DS::View<char> op = e->binary->operation.sv;
                Type left_type = type_check_expression(e->binary->left, env);
                Type right_type = type_check_expression(e->binary->right, env);

                if (left_type != right_type) {
                    const char* fmt = "[TypeChecker BinaryOp Error]: %.*s and %.*s are incompatible types for op: %.*s\n";
                    LOG_ERROR(fmt, left_type.name.length, left_type.name.data, right_type.name.length, right_type.name.data, op.length, op.data);
                    LOG_ERROR("[TypeChecker BinaryOp Error]: Line: %d\n", e->binary->line);
                    RUNTIME_ASSERT(false);
                }

                return left_type;
            } break;

            case EXPRESSION_TYPE_FUNCTION_CALL: {
                FunctionDecleration* func_decl = env->get_func(e->function_call->function_name);

                int param_count = func_decl->parameters.count();
                int arg_count = e->function_call->arguments.count();

                RUNTIME_ASSERT_MSG(param_count == arg_count, "Expected: %d argument(s), got: %d\n", param_count, arg_count);

                for (int i = 0; i < arg_count; i++) {
                    Expression* argument = e->function_call->arguments[i];
                    Parameter param = func_decl->parameters[i];

                    if (type_check_expression(argument, env) != param.type) {
                        RUNTIME_ASSERT_MSG(false, "Argument types don't match function decleration parameter types\n");
                    }
                }

                e->function_call->return_type = func_decl->return_type;
                return e->function_call->return_type;
            } break;

            case EXPRESSION_TYPE_IDENTIFIER: {
                RUNTIME_ASSERT_MSG(env->has_var(e->identifier->name), "Undeclared identifier: %.*s\n", e->identifier->name.length, e->identifier->name.data);
                VariableDecleration* var_decl = env->get_var(e->identifier->name);
                e->identifier->type = var_decl->type;

                return e->identifier->type;
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        RUNTIME_ASSERT(false);
        
        return Type();
    }

    Type type_check_decleration(Decleration* decl, TypeEnvironment* env) {
        switch (decl->type) {
            case DECLERATION_TYPE_VARIABLE: {
                if (env->has_var(decl->variable->variable_name)) {
                    RUNTIME_ASSERT_MSG(false, "Duplicate variable decleration: %.*s\n", decl->variable->variable_name.length, decl->variable->variable_name.data);
                }

                env->put_var(decl->variable->variable_name, decl->variable);

                if (decl->variable->rhs) {
                    Type expression_type = type_check_expression(decl->variable->rhs, env);

                    if (expression_type.type == TPT_VOID) {
                        RUNTIME_ASSERT_MSG(false, "Attempting to assign void to varible: %.*s\n", decl->variable->variable_name.length, decl->variable->variable_name.data);
                    }

                    if (decl->variable->type.name.data == nullptr) {
                        decl->variable->type = expression_type;
                        return expression_type;
                    }

                    if (decl->variable->type != expression_type) {
                        RUNTIME_ASSERT_MSG(false, "[TypeChecker VarDecl Error]: Line: %d\n", decl->variable->line);
                    }
                }

                return decl->variable->type;
            } break;

            case DECLERATION_TYPE_FUNCTION: {
                if (env->parent != nullptr) {
                    RUNTIME_ASSERT_MSG(false, "Not allowed to declare function: %.*s outside of global scope\n", decl->function->function_name.length, decl->function->function_name.data);
                }

                if (env->has_func(decl->function->function_name)) {
                    RUNTIME_ASSERT_MSG(false, "Duplicate function decleration: %.*s\n", decl->function->function_name.length, decl->function->function_name.data);
                }

                env->put_func(decl->function->function_name, decl->function);

                TypeEnvironment function_env = TypeEnvironment(env);
                for (Parameter p : decl->function->parameters) {
                    VariableDecleration* var_decl = (VariableDecleration*)Memory::global_general_allocator.malloc(sizeof(VariableDecleration));
                    var_decl->variable_name = p.variable_name;
                    var_decl->type = p.type;
                    var_decl->rhs = nullptr;
                    var_decl->line = decl->function->line;

                    function_env.put_var(var_decl->variable_name, var_decl);
                }
                
                if (decl->function->body.count() > 0 && decl->function->return_type.type != TPT_VOID &&
                    decl->function->body[decl->function->body.count() - 1]->statement &&
                    decl->function->body[decl->function->body.count() - 1]->statement->type != STATEMENT_TYPE_RETURN) {
                    RUNTIME_ASSERT_MSG(false, "return statement required if not returning void on function decleration\n");
                } 

                for (ASTNode* node : decl->function->body) {
                    if (node->type == AST_NODE_STATEMENT) {
                        Statement* s = node->statement;
                        if (s->type == STATEMENT_TYPE_RETURN) {
                            if (decl->function->return_type.type == TPT_VOID) {
                                RUNTIME_ASSERT_MSG(false, "return statement is not allowed in a function that returns void\n");
                            } 

                            Type actual_return_type = type_check_expression(s->ret->expression, &function_env);
                            if (decl->function->return_type != actual_return_type) {
                                RUNTIME_ASSERT_MSG(false, "returning invalid type: %.*s from function: %.*s\n", 
                                    actual_return_type.name.length, actual_return_type.name.data,
                                    decl->function->function_name.length, decl->function->function_name.data
                                );
                            }
                        }

                    }

                    type_check_ast_helper(node, &function_env);
                }

                return decl->function->return_type;
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        RUNTIME_ASSERT(false);
        
        return Type();
    }

    Type type_check_statement(Statement* s, TypeEnvironment* env) {
        switch (s->type) {
            case STATEMENT_TYPE_ASSIGNMENT: {
                if (!env->has_var(s->assignment->variable_name)) {
                    RUNTIME_ASSERT_MSG(
                        false, "Error Line: %d | Undeclared identifier '%.*s'\n", 
                        s->assignment->line, s->assignment->variable_name.length, s->assignment->variable_name.data
                    );
                }

                VariableDecleration* var_decl = env->get_var(s->assignment->variable_name);
                Type expression_type = type_check_expression(s->assignment->rhs, env);
                if (var_decl->type != expression_type) {
                    RUNTIME_ASSERT_MSG(
                        false, "Error Line: %d | Assignment of %.*s to %.*s is not allowed!\n",
                        s->assignment->line, var_decl->type.name.length, var_decl->type.name.data, 
                        expression_type.name.length, expression_type.name.data
                    );
                }

                return var_decl->type;
            } break;
            
            case STATEMENT_TYPE_RETURN: {
                Type expression_type = type_check_expression(s->ret->expression, env);
                return expression_type;
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        RUNTIME_ASSERT(false);
        
        return Type();
    }

    void type_check_ast_helper(ASTNode* node, TypeEnvironment* env) {
        switch (node->type) {
            case AST_NODE_PROGRAM: {
                Program* p = node->program;
                for (Decleration* decl : p->declerations) {
                    type_check_decleration(decl, env);
                }
            } break;

            case AST_NODE_EXPRESSION: {
                Expression* e = node->expression;
                type_check_expression(e, env);
            } break;

            case AST_NODE_STATEMENT: {
                Statement* s = node->statement;
                type_check_statement(s, env);
            } break;

            case AST_NODE_DECLERATION: {
                Decleration* decl = node->decleration;
                type_check_decleration(decl, env);
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }
    }

    void type_check_ast(ASTNode* node) {
        TypeEnvironment global_environment(nullptr);

        type_check_ast_helper(node, &global_environment);
    }
        
}