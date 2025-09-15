#pragma once

#include <Core/core.hpp>
#include "expression.hpp"

namespace Frontend {
    typedef struct ASTNode ASTNode;

    struct VariableDecleration {
        DS::View<char> variable_name;
        DS::View<char> type_name;
        Expression* right;
        u32 line;
    };

    struct FunctionDecleration {
        DS::View<char> function_name;
        DS::View<char> return_type_name;
        DS::Vector<VariableDecleration*> params;
        DS::Vector<ASTNode*> body;
        u32 line;
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
            DS::View<char> type_name, Expression* right, u32 line
        ) {
            Decleration* ret = (Decleration*)allocator->malloc(sizeof(Decleration));
            ret->type = DECLERATION_TYPE_VARIABLE;
            ret->variable = (VariableDecleration*)allocator->malloc(sizeof(VariableDecleration));
            ret->variable->variable_name = name;
            ret->variable->type_name = type_name;
            ret->variable->right = right;
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