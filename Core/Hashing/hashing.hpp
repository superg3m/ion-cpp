#include "../Common/common.hpp"

namespace ION::Hashing {
    u64 siphash24(const void* source, u64 source_size);
    u64 cstring_hash(const void* str, u64 str_length);
    bool cstring_equality(const void* c1, byte_t c1_size, const void* c2, byte_t c2_size);
    u64 string_view_hash(const void* view, u64 str_length);
    bool string_view_equality(const void* c1, byte_t c1_size, const void* c2, byte_t c2_size);
}