#include <Core/core.hpp>

typedef struct Expression Expression;

typedef struct StringExpression {
    DS::View<char> name;
    int line;
} StringExpression;

typedef struct IntegerExpression {
    int value;
    int line;
} IntegerExpression;

typedef struct FloatExpression {
    double value;
    int line;
} FloatExpression;

typedef struct BoolExpression {
    bool value;
    int line;
} BoolExpression;

typedef struct IdentifierExpression {
    DS::View<char> name;
    int line;
} IdentifierExpression;

typedef struct UnaryOperationExpression {
    Token operation;
    Expression* operand;
    int line;
} UnaryOperationExpression;

typedef struct BinaryOperationExpression {
    Token operation;
    Expression* left;
    Expression* right;
    int line;
} BinaryOperationExpression;

typedef struct LogicalOperationExpression {
    Token operation;
    Expression* left;
    Expression* right;
    int line;
} LogicalOperationExpression;

typedef struct GroupingExpression {
    Expression* value;
    int line;
} GroupingExpression;

typedef enum ExpressionType {
    EXPRESSION_TYPE_STRING,
    EXPRESSION_TYPE_INTEGER,
    EXPRESSION_TYPE_FLOAT,
    EXPRESSION_TYPE_BOOLEAN,
    EXPRESSION_TYPE_IDENTIFER,
    EXPRESSION_TYPE_UNARY_OPERATION,
    EXPRESSION_TYPE_BINARY_OPERATION,
    EXPRESSION_TYPE_LOGICAL_OPERATION,
    EXPRESSION_TYPE_GROUPING
} ExpressionType;

typedef struct Expression {
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
    };

    static Expression* String(Memory::BaseAllocator* allocator, DS::View<char> name, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_STRING;
        ret->str = (StringExpression*)allocator->malloc(sizeof(StringExpression));
        ret->str->name = name;
        ret->str->line = line;
        
        return ret;
    }
    static Expression* Integer(Memory::BaseAllocator* allocator, int value, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_INTEGER;
        ret->integer = (IntegerExpression*)allocator->malloc(sizeof(IntegerExpression));
        ret->integer->value = value;
        ret->integer->line = line;
    
        return ret;
    }
    static Expression* Float(Memory::BaseAllocator* allocator, float value, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_FLOAT;
        ret->floating = (FloatExpression*)allocator->malloc(sizeof(FloatExpression));
        ret->floating->value = value;
        ret->floating->line = line;
        
        return ret;
    }
    static Expression* Boolean(Memory::BaseAllocator* allocator, bool value, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_BOOLEAN;
        ret->boolean = (BoolExpression*)allocator->malloc(sizeof(BoolExpression));
        ret->boolean->value = value;
        ret->boolean->line = line;
        
        return ret;
    }

    static Expression* Identifier(Memory::BaseAllocator* allocator, DS::View<char> name, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_IDENTIFER;
        ret->identifier = (IdentifierExpression*)allocator->malloc(sizeof(IdentifierExpression));
        ret->identifier->name = name;
        ret->identifier->line = line;
        
        return ret;
    }
    static Expression* Unary(Memory::BaseAllocator* allocator, Token operation, Expression* operand, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_UNARY_OPERATION;
        ret->unary = (UnaryOperationExpression*)allocator->malloc(sizeof(UnaryOperationExpression));
        ret->unary->operation = operation;
        ret->unary->operand = operand;
        ret->unary->line = line;
        
        return ret;
    }
    static Expression* Binary(Memory::BaseAllocator* allocator, Token operation, Expression* left, Expression* right, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_BINARY_OPERATION;
        ret->binary = (BinaryOperationExpression*)allocator->malloc(sizeof(BinaryOperationExpression));
        ret->binary->operation = operation;
        ret->binary->left = left;
        ret->binary->right = right;
        ret->binary->line = line;

        return ret;
    }
    static Expression* Logical(Memory::BaseAllocator* allocator, Token operation, Expression* left, Expression* right, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_LOGICAL_OPERATION;
        ret->logical = (LogicalOperationExpression*)allocator->malloc(sizeof(LogicalOperationExpression));
        ret->logical->operation = operation;
        ret->logical->left = left;
        ret->logical->right = right;
        ret->logical->line = line;

        return ret;
    }
    static Expression* Grouping(Memory::BaseAllocator* allocator, Expression* value, int line) {
        Expression* ret = (Expression*)allocator->malloc(sizeof(Expression));
        ret->type = EXPRESSION_TYPE_GROUPING;
        ret->grouping = (GroupingExpression*)allocator->malloc(sizeof(GroupingExpression));
        ret->grouping->value = value;
        ret->grouping->line = line;
        
        return ret;
    }

private: 
    Expression() = default;
} Expression;
