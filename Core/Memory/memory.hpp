#pragma once

#include "../Common/common.hpp"
#include "allocator.hpp"

namespace Memory {
    void zero(void* data, byte_t data_size_in_bytes);
    void copy(void* destination, byte_t destination_size, const void* source, byte_t source_size);
    bool equal(const void* buffer_one, byte_t b1_size, const void* buffer_two, byte_t b2_size);
}