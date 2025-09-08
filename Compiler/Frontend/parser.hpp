#pragma once

#include <Core/core.hpp>
#include "ast.hpp"
#include "token.hpp"

namespace Frontend::Parser {
    ASTNode* generate_ast(Memory::Allocator& allocator, const Container::Vector<Token>& tokens);
}