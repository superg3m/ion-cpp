#include <Core/core.hpp>

#include "Frontend/token.hpp"
#include "Frontend/lexer.hpp"
#include "Frontend/parser.hpp"

void* arena_allocator(const Memory::Allocator* allocator, byte_t allocation_size) {
    Memory::Arena* arena = (Memory::Arena*)allocator->m_ctx;

    return arena->push(allocation_size);
}

void arena_free(const Memory::Allocator* allocator, void* data) {
    Memory::Arena* arena = (Memory::Arena*)allocator->m_ctx;

    arena->pop(data);
}

#define PROGRAM_MEMORY KB(500)

int main(int argc, char** argv) {
    char* executable_name = argv[0];
    if (argc < 2) {
        LOG_ERROR("Usage: %s <filename>\n", executable_name);
        return 0;
    }

    u8 program_memory[PROGRAM_MEMORY] = {0};
    Memory::Arena arena = Memory::Arena::Fixed(program_memory, PROGRAM_MEMORY, true);
    Memory::Allocator allocator = Memory::Allocator(arena_allocator, arena_free, (void*)&arena);
    // Memory::Allocator allocator = Memory::Allocator::libc();

    char* file_name = argv[1];
    Error error = ERROR_SUCCESS;

    byte_t file_size = 0;
    u8* data = Platform::read_entire_file(allocator, file_name, file_size, error);
    if (error != ERROR_SUCCESS) {
        LOG_ERROR("Error failed to read file: %s\n", error_str(error));
    }

    Container::Vector<Token> tokens = Container::Vector<Token>(50, allocator);
    Frontend::Lexer::generate_tokens(data, file_size, tokens);

    for (const Token& token : tokens) {
        const char* token_type_string = token.to_string();
        LOG_DEBUG("%s(%.*s)\n", token_type_string, token.source_view.length, token.source_view.data);
    }

    ASTNode* ast = Frontend::Parser::generate_ast(allocator, tokens);
    // Frontend::AST::pretty_print_ast(ast);
    (void)ast;

    arena.free();

    return 0;
}