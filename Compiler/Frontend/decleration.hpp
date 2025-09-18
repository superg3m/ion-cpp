#pragma once

#include <Core/core.hpp>
#include "expression.hpp"
#include "statement.hpp"
#include "types.hpp"

namespace Frontend {
    typedef struct ASTNode ASTNode;

    struct VariableDecleration {
        DS::View<char> variable_name;
        Type type;
        Expression* rhs;
        u32 line;
    };

    struct FunctionDecleration {
        DS::View<char> function_name;
        Type return_type;
        DS::Vector<ASTNode*> body;
        u32 line;
    };

    struct StructDecleration {
        Type type;
        u32 line;
    };

    enum DeclerationType {
        DECLERATION_TYPE_VARIABLE,
        DECLERATION_TYPE_STRUCT,
        DECLERATION_TYPE_FUNCTION,
    };

    struct Decleration {
        DeclerationType type;
        union {
            VariableDecleration* variable;
            FunctionDecleration* function;
            StructDecleration* struct_decl;
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
            Memory::BaseAllocator* allocator, DS::View<char> func_name, 
            Type return_type, DS::Vector<ASTNode*> body, u32 line
        ) {
            Decleration* ret = (Decleration*)allocator->malloc(sizeof(Decleration));
            ret->type = DECLERATION_TYPE_FUNCTION;
            ret->function = (FunctionDecleration*)allocator->malloc(sizeof(FunctionDecleration));
            ret->function->function_name = func_name;
            ret->function->return_type = return_type;
            ret->function->body = body;
            ret->function->line = line;
             
            return ret;
        }

        static Decleration* Struct(
            Memory::BaseAllocator* allocator,
            Type type, u32 line
        ) {
            Decleration* ret = (Decleration*)allocator->malloc(sizeof(Decleration));
            ret->type = DECLERATION_TYPE_STRUCT;
            ret->struct_decl = (StructDecleration*)allocator->malloc(sizeof(StructDecleration));
            ret->struct_decl->type = type;
            ret->struct_decl->line = line;
             
            return ret;
        }
        
    private:
        Decleration() = default;
    };
}