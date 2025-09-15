#pragma once

#include <Core/core.hpp>
#include "expression.hpp"
#include "decleration.hpp"

namespace Frontend {
    enum NodeType {
        AST_INVALID,
        AST_NODE_PROGRAM,
        AST_NODE_EXPRESSION,
        AST_NODE_STATEMENT,
        AST_NODE_DECLERATION,
    };

    struct Program {
        DS::Vector<Decleration*> declerations;
    };

    struct ASTNode {
        NodeType type = AST_INVALID;
        union {
            Program* program;
            Expression* expression;
            Decleration* decleration;
        };

        static ASTNode* Program(Memory::BaseAllocator* allocator, Program* program) {
            ASTNode* ret = (ASTNode*)allocator->malloc(sizeof(ASTNode));
            ret->type = AST_NODE_PROGRAM;
            ret->program = program;

            return ret;
        }

        static ASTNode* Expression(Memory::BaseAllocator* allocator, Expression* expression) {
            ASTNode* ret = (ASTNode*)allocator->malloc(sizeof(ASTNode));
            ret->type = AST_NODE_EXPRESSION;
            ret->expression = expression;

            return ret;
        }

        static ASTNode* Decleration(Memory::BaseAllocator* allocator, Decleration* decleration) {
            ASTNode* ret = (ASTNode*)allocator->malloc(sizeof(ASTNode));
            ret->type = AST_NODE_DECLERATION;
            ret->decleration = decleration;

            return ret;
        }

        void pretty_print(Memory::BaseAllocator* allocator);
    };
}

