#pragma once

#include <Core/core.hpp>
#include "ast.hpp"

namespace Frontend {
    ASTNode* generate_ast(Memory::BaseAllocator* allocator, const DS::Vector<Token>& tokens);
}