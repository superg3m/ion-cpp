#pragma once

#include <Core/core.hpp>
#include "expression.hpp"
#include "types.hpp"

namespace Frontend {
    typedef struct ASTNode ASTNode;
    typedef struct Decleration Decleration;

    struct AssignmentStatement {
        DS::View<char> variable_name;
        Expression* rhs;
        u32 line;
    };

    /**
     * First element in the conditionals is if
     * second is  element in the conditionals is else if
     */
    struct IfStatement {
        DS::Vector<Expression*> conditions;
        DS::Vector<DS::Vector<ASTNode*>> if_else_code_blocks;
        DS::Vector<ASTNode*> else_code_block; // [Optional]
    };

    /**
     * First element in the conditionals is if
     * second is  element in the conditionals is if
     */
    struct WhileStatement {
        Expression** conditional;
        DS::Vector<ASTNode*> code_block;
    };

    struct ForStatement {
        Decleration* initalization;
        Expression* conditional;
        DS::Vector<ASTNode*> code_block;
    };

    struct FunctionCallStatment {
    };

    enum StatementType {
        STATEMENT_TYPE_ASSIGNMENT,
    };

    struct Statement {
        StatementType type;
        union {
            AssignmentStatement* assignment;
        };

        static Statement* Assignment(Memory::BaseAllocator* allocator, DS::View<char> name, Expression* rhs, u32 line);
    private:
        Statement() = default;
    };
}