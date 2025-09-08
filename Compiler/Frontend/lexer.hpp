#pragma once

#include <Core/core.hpp>
#include "token.hpp"

namespace Frontend::Lexer {
    void generate_tokens(u8* data, byte_t file_size, Container::Vector<Token>& out_tokens);
}