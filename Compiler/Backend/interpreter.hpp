#pragma once

#include "../Frontend/ast.hpp"

using namespace Frontend;

namespace Backend {
    enum InterpreterValueType {
        VOID,
        INTEGER,
        FLOAT,
        BOOLEAN,
    };

    struct InterpreterValue {
        InterpreterValueType type = VOID;
        union {
            int i;
            float f;
            bool b;
        };
    };


    struct Scope {
        Scope* parent;

        Scope(Scope* parent) {
            this->parent = parent;
            this->variables = DS::Hashmap<DS::View<char>, InterpreterValue>(&Memory::global_general_allocator, 1);
            this->functions =  DS::Hashmap<DS::View<char>, Frontend::FunctionDeclaration*>(&Memory::global_general_allocator, 1);
        }

        bool has_var(DS::View<char> key) {
            Scope* current = this;
            while (current != nullptr) {
                if (current->variables.has(key)) {
                    return true;
                }

                current = current->parent;
            }

            return false;
        }

        void put_var(DS::View<char> key, InterpreterValue value) {
            Scope* current = this;
            while (current != nullptr) {
                if (current->variables.has(key)) {
                    return current->put_var(key, value);
                }

                current = current->parent;
            }
            
            this->variables.put(key, value);
        }

        InterpreterValue get_var(DS::View<char> key) {
            RUNTIME_ASSERT(this->has_var(key));
            
            Scope* current = this;
            while (current != nullptr) {
                if (current->variables.has(key)) {
                    return current->variables.get(key);
                }

                current = current->parent;
            }

            return InterpreterValue{VOID};
        }

        // ----------------------------------------

        bool has_func(DS::View<char> key) {
            Scope* current = this;
            while (current != nullptr) {
                if (current->functions.has(key)) {
                    return true;
                }

                current = current->parent;
            }

            return false;
        }

        void put_func(DS::View<char> key, Frontend::FunctionDeclaration* value) {
            RUNTIME_ASSERT(!this->has_var(key));
            
            this->functions.put(key, value);
        }

        Frontend::FunctionDeclaration* get_func(DS::View<char> key) {
            RUNTIME_ASSERT(this->has_func(key));
            
            Scope* current = this;
            while (current != nullptr) {
                if (current->functions.has(key)) {
                    return current->functions.get(key);
                }

                current = current->parent;
            }

            return nullptr;
        }
    private:
        DS::Hashmap<DS::View<char>, InterpreterValue> variables;
        DS::Hashmap<DS::View<char>, Frontend::FunctionDeclaration*> functions;
    };

    InterpreterValue interpret_nodes(DS::Vector<ASTNode*> nodes, Scope* scope);

    InterpreterValue evaluate_ints(TokenType op, float left, float right) {
        InterpreterValue ret = {};
        ret.i = 0;
        ret.type = INTEGER;


        switch (op) {
            case TS_PLUS: {
                ret.i = left + right;
                return ret;
            } break;

            case TS_MINUS: {
                ret.i = left - right;
                return ret;
            } break;

            case TS_STAR: {
                ret.i = left * right;
                return ret;
            } break;
        
            case TS_DIVISION: {
                ret.i = left / right;
                return ret;
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        return InterpreterValue{VOID};
    }

    InterpreterValue evaluate_floats(TokenType op, float left, float right) {
        InterpreterValue ret = {};
        ret.f = 0;
        ret.type = FLOAT;


        switch (op) {
            case TS_PLUS: {
                ret.f = left + right;
                return ret;
            } break;

            case TS_MINUS: {
                ret.f = left - right;
                return ret;
            } break;

            case TS_STAR: {
                ret.f = left * right;
                return ret;
            } break;
        
            case TS_DIVISION: {
                ret.f = left / right;
                return ret;
            } break;

            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        return InterpreterValue{VOID};
    }

    InterpreterValue interpret_binary_expression(TokenType op, InterpreterValue left, InterpreterValue right) {
        if (left.type == INTEGER && right.type == INTEGER) {
            return evaluate_ints(op, left.i, right.i);
        } else if (left.type == INTEGER && right.type == FLOAT) {
            return evaluate_floats(op, (float)left.i, right.f);
        } else if (left.type == FLOAT && right.type == INTEGER) {
            return evaluate_floats(op, left.f, (float)right.i);
        } else if (left.type == FLOAT && right.type == FLOAT) {
            return evaluate_floats(op, left.f, right.f);
        } else {
            RUNTIME_ASSERT(false);
        }

        return InterpreterValue{VOID};
    }

    InterpreterValue interpret_expression(Expression* e, Scope* scope) {
        InterpreterValue ret = {};
        ret.type = VOID;

        switch (e->type) {
            case EXPRESSION_TYPE_INTEGER: {
                ret.type = INTEGER;
                ret.i = e->integer->value;
                return ret;
            } break;

            case EXPRESSION_TYPE_FLOAT: {
                ret.type = FLOAT;
                ret.f = e->floating->value;
                return ret;
            } break;

            case EXPRESSION_TYPE_BOOLEAN: {
                ret.type = BOOLEAN;
                ret.b = e->boolean->value;
                return ret;
            } break;

            case EXPRESSION_TYPE_IDENTIFIER: {
                return scope->get_var(e->identifier->name);
            } break;

            case EXPRESSION_TYPE_BINARY_OPERATION: {
                InterpreterValue lv = interpret_expression(e->binary->left, scope);
                InterpreterValue rv = interpret_expression(e->binary->right, scope);

                return interpret_binary_expression(e->binary->operation.type, lv, rv);
            } break;

            case EXPRESSION_TYPE_FUNCTION_CALL: {
                FunctionDeclaration* func_decl = scope->get_func(e->function_call->function_name);
                int arg_count = e->function_call->arguments.count();

                Scope functionScope = Scope(scope);
                for (int i = 0; i < arg_count; i++) {
                    Parameter param = func_decl->parameters[i];
                    Expression* arg = e->function_call->arguments[i];
                    functionScope.put_var(param.variable_name, interpret_expression(arg, scope));
                }

                return interpret_nodes(func_decl->body, &functionScope);
            } break;
            
            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        return ret;
    }

    InterpreterValue interpret_statement(Statement* s, Scope* scope) {
        InterpreterValue ret = {};
        ret.type = VOID;

        switch (s->type) {
            case STATEMENT_TYPE_ASSIGNMENT: {
                InterpreterValue rhs = interpret_expression(s->assignment->rhs, scope);
                scope->put_var(s->assignment->variable_name, rhs);
            } break;

            case STATEMENT_TYPE_RETURN: {
                return interpret_expression(s->ret->expression, scope);
            } break;
                        
            case STATEMENT_TYPE_SCOPE: {
                Scope block_scope = Scope(scope);
                return interpret_nodes(s->scope->body, &block_scope);
            } break;

            case STATEMENT_TYPE_PRINT: {
                InterpreterValue v = interpret_expression(s->print->expr, scope);
                LOG_TRACE("%d\n", v.i);
            } break;
            
            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        return ret;
    }

    void interpret_decleration(Decleration* decl, Scope* scope) {
        switch (decl->type) {
            case DECLERATION_TYPE_VARIABLE: {
                InterpreterValue rhs = interpret_expression(decl->variable->rhs, scope);
                scope->put_var(decl->variable->variable_name, rhs);
            } break;

            case DECLERATION_TYPE_FUNCTION: {
                scope->put_func(decl->function->function_name, decl->function);
            } break;
        }
    }

    InterpreterValue interpret_nodes(DS::Vector<ASTNode*> nodes, Scope* scope) {
        InterpreterValue ret = {};
        ret.type = VOID;

        for (ASTNode* node : nodes) {
            switch (node->type) {
                case AST_NODE_DECLERATION: {
                    interpret_decleration(node->decleration, scope);
                } break;

                case AST_NODE_STATEMENT: {
                    return interpret_statement(node->statement, scope);
                } break;
                
                 
                default: {
                    RUNTIME_ASSERT(false);
                } break;
            }
        }

        return ret;
    }

    void interpret_program(ASTNode* node) {
        Scope global_scope = Scope(nullptr);

        for (Decleration* decl : node->program->declerations) {
            interpret_decleration(decl, &global_scope);
        }

        DS::View main_key = DS::View("main", sizeof("main") - 1);
        FunctionDeclaration* main_decl = global_scope.get_func(main_key);
        interpret_nodes(main_decl->body, &global_scope);
    }
}

