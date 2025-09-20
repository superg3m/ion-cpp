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
        u32 line;
    };

    struct ForStatement {
        Decleration* initalization;
        Expression* conditional;
        DS::Vector<ASTNode*> body;
        u32 line;
    };

    struct ReturnStatment {
        Expression* expression;
        u32 line;
    };

    struct ScopeStatement {
        DS::Vector<ASTNode*> body;
        u32 line;
    };

    struct PrintStatement {
        Expression* expr;
        u32 line;
    };

    enum StatementType {
        STATEMENT_TYPE_ASSIGNMENT,
        STATEMENT_TYPE_RETURN,
        STATEMENT_TYPE_SCOPE,
        STATEMENT_TYPE_PRINT,
    };

    struct Statement {
        StatementType type;
        union {
            AssignmentStatement* assignment;
            ReturnStatment* ret;
            ScopeStatement* scope;
            PrintStatement* print;
        };

        static Statement* Assignment(Memory::BaseAllocator* allocator, DS::View<char> name, Expression* rhs, u32 line);
        static Statement* Return(Memory::BaseAllocator* allocator, Expression* expression, u32 line);
        static Statement* Scope(Memory::BaseAllocator* allocator, DS::Vector<ASTNode*> body, u32 line);
        static Statement* Print(Memory::BaseAllocator* allocator, Expression* expr, u32 line);
    private:
        Statement() = default;
    };
}