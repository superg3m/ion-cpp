#pragma once

#include <Core/core.hpp>
#include "expression.hpp"

namespace Frontend {
    enum NodeType {
        AST_NODE_PROGRAM,
        AST_NODE_EXPRESSION,
        AST_NODE_STATEMENT,
    };

    struct ASTNode {
        NodeType type;
        union {
            Expression* expression;
        };

        static ASTNode* Expression(Memory::BaseAllocator* allocator, Expression* expression) {
            ASTNode* ret = (ASTNode*)allocator->malloc(sizeof(ASTNode));
            ret->type = AST_NODE_EXPRESSION;
            ret->expression = expression;

            return ret;
        }

        void pretty_print(Memory::BaseAllocator* allocator);
    };
}

