#include "../Frontend/expression.hpp"

struct RValue {

};

struct Scope {
    DS::Hashmap<char*, RValue> variables;
    Scope* parent;
};

struct Interpreter {
protected:
    virtual RValue interpret_expression(Expression* expression) = 0;
};

struct TreeWalkInterpreter : public Interpreter {
    void interpret_ast();

private:
    RValue interpret_expression(Expression* expression) override;
};