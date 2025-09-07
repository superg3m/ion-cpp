#include <Core/core.hpp>

int main(int argc, char** argv) {
    char* executable_name = argv[0];
    if (argc < 2) {
        LOG_ERROR("Usage: %s <filename>\n", executable_name);
        return 0;
    }

    char* file_name = argv[1];
    ION::Memory::Allocator allocator = ION::Memory::Allocator::libc();

    byte_t file_size = 0;
    IonError error = ION_ERROR_SUCCESS;
    ION::Platform::read_entire_file(allocator, file_name, file_size, error);
    if (error != ION_ERROR_SUCCESS) {
        LOG_ERROR("Error failed to read file: %s\n", ion_error_str(error));
    }

    ION::Container::Hashmap<int, int>map = ION::Container::Hashmap<int, int>();

    return 0;
}