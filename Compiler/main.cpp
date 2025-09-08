#include <Core/core.hpp>

#include "Frontend/token.hpp"
#include "Frontend/lexer.hpp"

int main(int argc, char** argv) {
    char* executable_name = argv[0];
    if (argc < 2) {
        LOG_ERROR("Usage: %s <filename>\n", executable_name);
        return 0;
    }

    char* file_name = argv[1];
    Memory::Allocator allocator = Memory::Allocator::libc();

    byte_t file_size = 0;
    Error error = ERROR_SUCCESS;
    u8 * data = Platform::read_entire_file(allocator, file_name, file_size, error);
    if (error != ERROR_SUCCESS) {
        LOG_ERROR("Error failed to read file: %s\n", error_str(error));
    }

    Container::Vector<Token> tokens = Container::Vector<Token>(50, allocator);
    Frontend::Lexer::generate_tokens(data, file_size, tokens);

    for (const Token& token : tokens) {
        const char* token_type_string = token.to_string();
        LOG_DEBUG("%s(%.*s)\n", token_type_string, token.source_view.length, token.source_view.data);
    }

    allocator.free(data);

    return 0;
}