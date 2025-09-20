#include <Core/core.hpp>
#include "Frontend/parser.hpp"
#include "Frontend/typechecker.hpp"
#include "Backend/interpreter.hpp"

#define PROGRAM_CAPACITY KB(20)

int main(int argc, char** argv) {
    char* executable_name = argv[0];
    if (argc < 2) {
        LOG_ERROR("Usage: %s <filename>\n", executable_name);
        return 0;
    }

    u8 program_memory[PROGRAM_CAPACITY] = {0};
    Memory::ArenaAllocator allocator = Memory::ArenaAllocator::Fixed(program_memory, PROGRAM_CAPACITY, true);

    char* file_name = argv[1];
    Error error = ERROR_SUCCESS;

    byte_t file_size = 0;
    u8* data = Platform::read_entire_file(&allocator, file_name, file_size, error);
    if (error != ERROR_SUCCESS) {
        LOG_ERROR("Error failed to read file: %s\n", error_str(error));
    }

    DS::Vector<Token> tokens = DS::Vector<Token>(&allocator, 50);
    Lexer::generate_tokens(data, file_size, tokens);

    for (const Token& token : tokens) {
        const char* token_type_string = token.type_to_string();
        LOG_DEBUG("%s(%.*s) | Line: %d\n", token_type_string, token.sv.length, token.sv.data, token.line);
    }

    Frontend::ASTNode* ast = Frontend::generate_ast(&allocator, tokens);
    Frontend::type_check_ast(ast);
    
    ast->pretty_print(&Memory::global_general_allocator);

    Backend::interpret_program(ast);

    return 0;
}