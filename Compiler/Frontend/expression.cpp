#include "expression.hpp"

namespace Frontend {
    Expression* Expression::String(Memory::BaseAllocator* allocator, DS::View<char> name, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_STRING;
        ret->str = (StringExpression*)allocator->malloc(sizeof(StringExpression));
        ret->str->name = name;
        ret->str->line = line;
        
        return ret;
    }

    Expression* Expression::Integer(Memory::BaseAllocator* allocator, int value, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_INTEGER;
        ret->integer = (IntegerExpression*)allocator->malloc(sizeof(IntegerExpression));
        ret->integer->value = value;
        ret->integer->line = line;

        return ret;
    }

    Expression* Expression::Float(Memory::BaseAllocator* allocator, float value, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_FLOAT;
        ret->floating = (FloatExpression*)allocator->malloc(sizeof(FloatExpression));
        ret->floating->value = value;
        ret->floating->line = line;
        
        return ret;
    }

    Expression* Expression::Boolean(Memory::BaseAllocator* allocator, bool value, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_BOOLEAN;
        ret->boolean = (BoolExpression*)allocator->malloc(sizeof(BoolExpression));
        ret->boolean->value = value;
        ret->boolean->line = line;
        
        return ret;
    }

    Expression* Expression::Identifier(Memory::BaseAllocator* allocator, DS::View<char> name, Type type, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_IDENTIFIER;
        ret->identifier = (IdentifierExpression*)allocator->malloc(sizeof(IdentifierExpression));
        ret->identifier->name = name;
        ret->identifier->type = type;
        ret->identifier->line = line;
        
        return ret;
    }

    Expression* Expression::Unary(Memory::BaseAllocator* allocator, Token operation, Expression* operand, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_UNARY_OPERATION;
        ret->unary = (UnaryOperationExpression*)allocator->malloc(sizeof(UnaryOperationExpression));
        ret->unary->operation = operation;
        ret->unary->operand = operand;
        ret->unary->line = line;
        
        return ret;
    }

    Expression* Expression::Binary(Memory::BaseAllocator* allocator, Token operation, Expression* left, Expression* right, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_BINARY_OPERATION;
        ret->binary = (BinaryOperationExpression*)allocator->malloc(sizeof(BinaryOperationExpression));
        ret->binary->operation = operation;
        ret->binary->left = left;
        ret->binary->right = right;
        ret->binary->line = line;

        return ret;
    }

    Expression* Expression::Grouping(Memory::BaseAllocator* allocator, Expression* value, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_GROUPING;
        ret->grouping = (GroupingExpression*)allocator->malloc(sizeof(GroupingExpression));
        ret->grouping->value = value;
        ret->grouping->line = line;
        
        return ret;
    }

    Expression* Expression::FunctionCall(
        Memory::BaseAllocator* allocator, 
        DS::View<char> name, Type return_type, 
        DS::Vector<Expression*> arguments,
        u32 line
    ) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_FUNCTION_CALL;
        ret->function_call = (FunctionCallExpression*)allocator->malloc(sizeof(FunctionCallExpression));
        ret->function_call->function_name = name;
        ret->function_call->return_type = return_type;
        ret->function_call->arguments = arguments;
        ret->function_call->line = line;
        
        return ret;
    }
}