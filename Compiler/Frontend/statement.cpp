#include "statement.hpp"

namespace Frontend {
    Statement* Statement::Assignment(Memory::BaseAllocator* allocator, DS::View<char> name, Expression* rhs, u32 line) {
        Statement* ret = (Statement*)allocator->malloc(sizeof(Statement));
        ret->type = STATEMENT_TYPE_ASSIGNMENT;
        ret->assignment = (AssignmentStatement*)allocator->malloc(sizeof(AssignmentStatement));
        ret->assignment->variable_name = name;
        ret->assignment->rhs = rhs;
        ret->assignment->line = line;
        
        return ret;
    }

    Statement* Statement::Return(Memory::BaseAllocator* allocator, Expression* expression, u32 line) {
        Statement* ret = (Statement*)allocator->malloc(sizeof(Statement));
        ret->type = STATEMENT_TYPE_RETURN;
        ret->ret = (ReturnStatment*)allocator->malloc(sizeof(ReturnStatment));
        ret->ret->expression = expression;
        ret->ret->line = line;
        
        return ret;
    }

    Statement* Statement::Scope(Memory::BaseAllocator* allocator, DS::Vector<ASTNode*> body, u32 line) {
        Statement* ret = (Statement*)allocator->malloc(sizeof(Statement));
        ret->type = STATEMENT_TYPE_SCOPE;
        ret->scope = (ScopeStatement*)allocator->malloc(sizeof(ScopeStatement));
        ret->scope->body = body;
        ret->scope->line = line;
        
        return ret;
    }

    Statement* Statement::Print(Memory::BaseAllocator* allocator, Expression* expr, u32 line) {
        Statement* ret = (Statement*)allocator->malloc(sizeof(Statement));
        ret->type = STATEMENT_TYPE_PRINT;
        ret->print = (PrintStatement*)allocator->malloc(sizeof(PrintStatement));
        ret->print->expr = expr;
        ret->print->line = line;
        
        return ret;
    }
}