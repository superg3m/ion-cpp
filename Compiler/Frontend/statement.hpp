#pragma once

#include <Core/core.hpp>
#include "expression.hpp"
#include "types.hpp"

namespace Frontend {
    typedef struct ASTNode ASTNode;

    struct AssignmentStatement {
        DS::View<char> variable_name;
        Expression* rhs;
        u32 line;
    };

    struct ConditionalStatement {
        Expression* condition;
        
    };


    /**
     * First element in the conditionals is if
     * second is  element in the conditionals is else if
     */
    struct IfElseStatement {
        DS::Vector<Expression*> conditions;
        DS::Vector<DS::Vector<ASTNode*>> if_else_code_blocks;

        DS::Vector<ASTNode*> else_code_block; // [Optional]
    };

    /**
     * First element in the conditionals is if
     * second is  element in the conditionals is if
     */
    struct WhileStatement {
        ConditionalStatement* conditional;
    };

    /**
     * First element in the conditionals is if
     * second is  element in the conditionals is if
     */
    struct WhileStatement {
        Decleration* initalization;
        ConditionalStatement* conditional;
        ConditionalStatement* conditional;
    };

    enum DeclerationType {
        DECLERATION_TYPE_VARIABLE,
        DECLERATION_TYPE_FUNCTION,
    };

    struct Decleration {
        DeclerationType type;
        union {
            VariableDecleration* variable;
            FunctionDecleration* function;
        };

        static Decleration* Variable(
            Memory::BaseAllocator* allocator, DS::View<char> name, 
            Type type, Expression* rhs, u32 line
        ) {
            Decleration* ret = (Decleration*)allocator->malloc(sizeof(Decleration));
            ret->type = DECLERATION_TYPE_VARIABLE;
            ret->variable = (VariableDecleration*)allocator->malloc(sizeof(VariableDecleration));
            ret->variable->variable_name = name;
            ret->variable->type = type;
            ret->variable->rhs = rhs;
            ret->variable->line = line;
            
            return ret;
        }

        static Decleration* Function(
            Memory::BaseAllocator* allocator, DS::View<char> function_name, 
            DS::View<char> return_type_name, DS::Vector<ASTNode*> body, u32 line
        ) {
            Decleration* ret = (Decleration*)allocator->malloc(sizeof(Decleration));
            ret->type = DECLERATION_TYPE_FUNCTION;
            ret->function = (FunctionDecleration*)allocator->malloc(sizeof(FunctionDecleration));
            ret->function->function_name = function_name;
            ret->function->return_type_name = return_type_name;
            ret->function->line = line;
            ret->function->body = body;
            
            return ret;
        }
        
    private:
        Decleration() = default;
    };
}