#pragma once

#include <initializer_list>
#include <type_traits>

#include "../Memory/memory.hpp"
#include "../Common/common.hpp"

namespace DS {
    template <typename T>
    struct View {
        const T* data = nullptr;
        u64 length = 0;

        View() = default;
        View(const T* data, u64 length): data(data), length(length) {}

        DS::View<T>& operator=(const DS::View<T>& other) {
            if (this != &other) {
                this->data  = other.data;
                this->length = other.length;
            }

            return *this;
        }
    };
}