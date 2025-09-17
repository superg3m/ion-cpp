#pragma once

#include <Core/core.hpp>

/*
enum TypeModifier {
    TYPE_MODIFER_ARRAY,
    TYPE_MODIFER_POINTER,
};
*/

struct Type {
    DS::View<char> name;
    TokenType type; // TPT_*

    Type() = default;

    Type(DS::View<char> name, TokenType type) {
        this->name = name;
        this->type = type;
    }

    bool operator==(const Type& rhs) const {
        if (this->type == rhs.type) {
            if (this->type == TPT_STRING) {
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