#include <Core/core.hpp>

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
    Platform::read_entire_file(allocator, file_name, file_size, error);
    if (error != ERROR_SUCCESS) {
        LOG_ERROR("Error failed to read file: %s\n", error_str(error));
    }

    return 0;
}