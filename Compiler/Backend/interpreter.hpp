#include "../frontend/ast.hpp"
#include "scope.hpp"

using namespace Frontend;

namespace Backend {
    enum InterpreterValueType {
        VOID,
        INTEGER,
        FLOAT,
        BOOLEAN,
    };

    struct InterpreterValue {
        InterpreterValueType type;
        union {
            int i;
            float f;
            bool b;
        };
    };

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
    }

    InterpreterValue interpret_binary_expression(TokenType op, Expression* left, Expression* right) {
        if (left->type == EXPRESSION_TYPE_INTEGER && right->type == EXPRESSION_TYPE_INTEGER) {
            return evaluate_ints(op, left->integer->value, right->integer->value);
        } else if (left->type == EXPRESSION_TYPE_INTEGER && right->type == EXPRESSION_TYPE_FLOAT) {
            return evaluate_floats(op, (float)left->integer->value, right->floating->value);
        } else if (left->type == EXPRESSION_TYPE_FLOAT && right->type == EXPRESSION_TYPE_INTEGER ) {
            return evaluate_floats(op, left->floating->value, (float)right->integer->value);
        } else if (left->type == EXPRESSION_TYPE_FLOAT && right->type == EXPRESSION_TYPE_FLOAT) {
            return evaluate_floats(op, left->floating->value, right->floating->value);
        }
    }

    InterpreterValue interpret_expression(Expression* e) {
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
            
            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        return ret;
    }

    InterpreterValue interpret_statement(Statement* s) {
        InterpreterValue ret = {};
        ret.type = VOID;

        switch (s->type) {
            case STATEMENT_TYPE_ASSIGNMENT: {

            } break;

            case STATEMENT_TYPE_RETURN: {
                return interpret_expression(s->ret->expression);
            } break;
            
            default: {
                RUNTIME_ASSERT(false);
            } break;
        }

        return ret;
    }


    InterpreterValue interpret_statements(DS::Vector<Statement*> statemenets) {
        InterpreterValue ret = {};
        ret.type = VOID;

        for (Statement* s : statemenets) {
            ret = interpret_statement(s);
            if (ret.type != VOID) {
                return ret;
            }
        }

        return ret;
    }


    /*
    interpret_program(ASTNode* node) {
        node->program.
    }
    */
}

