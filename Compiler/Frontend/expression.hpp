#pragma once

#include <Core/core.hpp>
#include "types.hpp"

namespace Frontend {
    typedef struct Expression Expression;

    struct StringExpression {
        DS::View<char> name;
        int line;
    };

    struct IntegerExpression {
        int value;
        int line;
    };

    struct FloatExpression {
        double value;
        int line;
    };

    struct BoolExpression {
        bool value;
        int line;
    };

    struct IdentifierExpression {
        DS::View<char> name;
        int line;
    };

    struct UnaryOperationExpression {
        Token operation;
        Expression* operand;
        int line;
    };

    struct BinaryOperationExpression {
        Token operation;
        Expression* left;
        Expression* right;
        int line;
    };

    struct LogicalOperationExpression {
        Token operation;
        Expression* left;
        Expression* right;
        int line;
    };

    struct GroupingExpression {
        Expression* value;
        int line;
    };

    struct FunctionCallExpression {
        DS::View<char> function_name;
        Type return_type;
        u32 line;
    };

    enum ExpressionType {
        EXPRESSION_TYPE_STRING,
        EXPRESSION_TYPE_INTEGER,
        EXPRESSION_TYPE_FLOAT,
        EXPRESSION_TYPE_BOOLEAN,
        EXPRESSION_TYPE_IDENTIFER,
        EXPRESSION_TYPE_UNARY_OPERATION,
        EXPRESSION_TYPE_BINARY_OPERATION,
        EXPRESSION_TYPE_LOGICAL_OPERATION,
        EXPRESSION_TYPE_GROUPING,
        EXPRESSION_TYPE_FUNCTION_CALL
    };

    struct Expression {
        ExpressionType type;
        union {
            StringExpression* str;
            IntegerExpression* integer;
            FloatExpression* floating;
            BoolExpression* boolean;
            IdentifierExpression* identifier;
            UnaryOperationExpression* unary;
            BinaryOperationExpression* binary;
            LogicalOperationExpression* logical;
            GroupingExpression* grouping;
            FunctionCallExpression* function_call;
        };

        static Expression* String(Memory::BaseAllocator* allocator, DS::View<char> name, int line);
        static Expression* Integer(Memory::BaseAllocator* allocator, int value, int line);
        static Expression* Float(Memory::BaseAllocator* allocator, float value, int line);
        static Expression* Boolean(Memory::BaseAllocator* allocator, bool value, int line);

        static Expression* Identifier(Memory::BaseAllocator* allocator, DS::View<char> name, int line);
        static Expression* Unary(Memory::BaseAllocator* allocator, Token operation, Expression* operand, int line);
        static Expression* Binary(Memory::BaseAllocator* allocator, Token operation, Expression* left, Expression* right, int line);
        static Expression* Logical(Memory::BaseAllocator* allocator, Token operation, Expression* left, Expression* right, int line);
        static Expression* Grouping(Memory::BaseAllocator* allocator, Expression* value, int line);

        static Expression* FunctionCall(Memory::BaseAllocator* allocator, DS::View<char> name, Type return_type, u32 line);
    private: 
        Expression() = default;
    };
}