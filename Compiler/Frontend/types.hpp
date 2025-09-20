#pragma once

#include <Core/core.hpp>


enum TypeModifier {
    TYPE_MODIFER_ARRAY,
    TYPE_MODIFER_POINTER,
};

namespace Frontend {
    typedef struct ASTNode ASTNode;

    struct Type {
        DS::View<char> name;
        TokenType type; // TPT_*

        // TODO(Jovanni): THIS IS SO BAD
        DS::Vector<TypeModifier> members;

        Type() = default;

        Type(DS::View<char> name, TokenType type, DS::Vector<TypeModifier> members) {
            this->name = name;
            this->type = type;
            this->members = members;
        }

        bool operator==(const Type& rhs) const {
            if (this->type == rhs.type) {
                if (this->type == TOKEN_IDENTIFIER) {
                    return String::equal(this->name, rhs.name);
                } else {
                    return true;
                }
            }   
            
            return false;
        }

        bool operator!=(const Type& rhs) const {
            return (*this == rhs) == false;
        }
    };
}