#include <Core/core.hpp>

typedef struct VariableDecleration {
    DS::View<char> name;
    DS::View<char> type_name;
    int line;
} VariableDecleration;

typedef struct FunctionDecleration {

} FunctionDecleration;

enum DeclerationType {
    DECLERATION_TYPE_VARIABLE,
    DECLERATION_TYPE_FUNCTION,
};

typedef struct Decleration {
    DeclerationType type;
    union {
        VariableDecleration* variable;
        FunctionDecleration* function;
    };

    static Decleration* variable(Memory::BaseAllocator* allocator, DS::View<char> name, DS::View<char> type_name, int line) {
        Decleration* ret = (Decleration*)allocator->malloc(sizeof(Decleration));
        ret->type = DECLERATION_TYPE_VARIABLE;
        ret->variable = (VariableDecleration*)allocator->malloc(sizeof(VariableDecleration));
        ret->variable->name = name;
        ret->variable->type_name = type_name;
        
        return ret;
    }
    
private:
    Decleration() = default;
};
