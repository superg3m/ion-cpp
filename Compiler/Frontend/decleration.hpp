#pragma once

#include <Core/core.hpp>
#include "expression.hpp"

struct VariableDecleration {
    DS::View<char> name;
    DS::View<char> type_name;
    Expression* right;
    int line;
};

/*
struct VariableDeclerationAssignment {
    DS::View<char> name;
    DS::View<char> type_name;
    int line;
};
*/

struct FunctionDecleration {
    DS::View<char> name;
    // params
    int line;
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
        DS::View<char> type_name, Expression* right, int line
    ) {
        Decleration* ret = (Decleration*)allocator->malloc(sizeof(Decleration));
        ret->type = DECLERATION_TYPE_VARIABLE;
        ret->variable = (VariableDecleration*)allocator->malloc(sizeof(VariableDecleration));
        ret->variable->name = name;
        ret->variable->type_name = type_name;
        ret->variable->right = right;
        
        return ret;
    }
    
private:
    Decleration() = default;
};
