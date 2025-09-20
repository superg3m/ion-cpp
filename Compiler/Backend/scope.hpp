#pragma once

#include "../Frontend/ast.hpp"

namespace Backend {
    struct Scope {
        Scope* parent;

        bool has_var(DS::View<char> key) {
            Scope* current = this;
            while (current != nullptr) {
                if (current->variables.has(key)) {
                    return true;
                }

                current = current->parent;
            }

            return false;
        }

        void put_var(DS::View<char> key, Frontend::Expression* value) {
            Scope* current = this;
            while (current != nullptr) {
                if (current->variables.has(key)) {
                    return current->put_var(key, value);
                }

                current = current->parent;
            }
            
            this->variables.put(key, value);
        }

        Frontend::Expression* get_var(DS::View<char> key) {
            RUNTIME_ASSERT(this->has_var(key));
            
            Scope* current = this;
            while (current != nullptr) {
                if (current->variables.has(key)) {
                    return current->variables.get(key);
                }

                current = current->parent;
            }

            return nullptr;
        }

        // ----------------------------------------

        bool has_func(DS::View<char> key) {
            Scope* current = this;
            while (current != nullptr) {
                if (current->functions.has(key)) {
                    return true;
                }

                current = current->parent;
            }

            return false;
        }

        void put_func(DS::View<char> key, Frontend::FunctionDecleration* value) {
            RUNTIME_ASSERT(!this->has_var(key));
            
            this->functions.put(key, value);
        }

        Frontend::FunctionDecleration* get_func(DS::View<char> key) {
            RUNTIME_ASSERT(this->has_func(key));
            
            Scope* current = this;
            while (current != nullptr) {
                if (current->functions.has(key)) {
                    return current->functions.get(key);
                }

                current = current->parent;
            }

            return nullptr;
        }
    private:
        DS::Hashmap<DS::View<char>, Frontend::Expression*> variables;
        DS::Hashmap<DS::View<char>, Frontend::FunctionDecleration*> functions;
    };
}
