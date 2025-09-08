#pragma once

#include <Core/core.hpp>
#include "ast.hpp"
#include "token.hpp"

namespace Frontend::Parser {
    ASTNode* generate_ast(const Container::Vector<Token>& tokens);
}