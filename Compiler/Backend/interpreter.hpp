#include "../Frontend/ast.hpp"

namespace Backend {
    struct RValue {
        Frontend::Type type;
    };

    struct Scope {
        DS::Hashmap<char*, RValue> variables;
        Scope* parent;
    };

    struct TreeWalkInterpreter {
        RValue interpret_expression(Frontend::Expression* expression);
    };
}

