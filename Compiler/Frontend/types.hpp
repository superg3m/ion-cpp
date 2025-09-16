#pragma once

#include <Core/core.hpp>

enum TypeModifier {
    TYPE_MODIFER_ARRAY,
    TYPE_MODIFER_POINTER,
};

struct Type {
    DS::Vector<TypeModifier> modifiers;
    DS::View<char> name;

    Type() = default;

    Type(DS::Vector<TypeModifier> modifiers, DS::View<char> name) : modifiers(modifiers) {
        this->name = name;
    }

    bool operator==(const Type& rhs) const {
        if (this->modifiers.count() != rhs.modifiers.count()) {
            return false;
        }

        for (int i = 0; i < this->modifiers.count(); i++) {
            TypeModifier m1 = rhs.modifiers[i];
            TypeModifier m2 = rhs.modifiers[i];

            if (m1 != m2) {
                return false;
            }
        }

        return String::equal(this->name, rhs.name);
    }

    bool operator!=(const Type& rhs) const {
        return (*this == rhs) == false;
    }
};