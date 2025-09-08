#pragma once

#include <Core/core.hpp>
#include "expression.hpp"
#include "token.hpp"

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
};