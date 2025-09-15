/*
enum TypeModifier {
    TYPE_MODIFER_ARRAY,
    TYPE_MODIFER_POINTER,
};

struct Type {
    DS::Vector<TypeModifier> modifers;
    DS::View<char> name;

    bool operator==(const Type& rhs) const {
        if (this->modifier.count() != rhs.modifier.count()) {
            return false;
        }

        for (int i = 0; i < this->modifiers.count(); i++) {
            TypeModifier m1 = rhs.modifiers[i]
            TypeModifier m2 = rhs.modifiers[i]

            if (m1 != m2) {
                return false;
            }
        }

        return String::equal(this->name, rhs.name)
    }

};
*/