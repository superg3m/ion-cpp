#include "platform.hpp"

#if defined(PLATFORM_LINUX) || defined(PLATFORM_APPLE)
    #include "../Common/logger.hpp"
    #include "../Common/assert.hpp"
    #include "../Error/error.hpp"

    #include <unistd.h>
    #include <dlfcn.h>
    #include <stdio.h>

    namespace Platform {
        bool initialize() { return true; }
        void shutdown() {}
        double get_seconds_elapsed() { return 0.0; }

        bool file_path_exists(const char* path) {
            FILE *fptr = fopen(path, "r");

            if (fptr == nullptr) {
                return false;
            }

            fclose(fptr);

            return true;
        }

        int copy_file(const char* source_path, const char* dest_path, int block_until_success) {
            const size_t BUFFER_SIZE = 4096;

            for (;;) {
                FILE* in = fopen(source_path, "rb");
                if (!in) {
                    if (!block_until_success) return 0;
                    sleep(10);
                    continue;
                }

                FILE* out = fopen(dest_path, "wb");
                if (!out) {
                    fclose(in);
                    if (!block_until_success) return 0;
                    sleep(10);
                    continue;
                }

                char buffer[BUFFER_SIZE];
                size_t bytes;
                int success = 1;

                while ((bytes = fread(buffer, 1, BUFFER_SIZE, in)) > 0) {
                    if (fwrite(buffer, 1, bytes, out) != bytes) {
                        success = 0;
                        break;
                    }
                }

                fclose(in);
                fclose(out);

                if (success) return 1;
                if (!block_until_success) return 0;
                sleep(10);
            }
        }

        void sleep(u32 ms) {
            usleep(ms * 1000);
        }

        Memory::Allocator get_allocator() {
            return Memory::Allocator::libc();
        }

        u8* read_entire_file(Memory::Allocator& allocator, const char* file_name, byte_t& out_file_size, Error& error) {
            FILE* file_handle = fopen(file_name, "rb");
            if (file_handle == nullptr) {
                LOG_ERROR("Invalid file_handle, the file_name/path is likely wrong: read_entire_file(%s)\n", file_name);
                error = ERROR_RESOURCE_NOT_FOUND;

                return nullptr;
            }

            if (fseek(file_handle, 0L, SEEK_END) != 0) {
                LOG_ERROR("fseek failed: read_entire_file(%s)\n", file_name);
                error = ERROR_RESOURCE_NOT_FOUND;
                fclose(file_handle);
                return NULL;
            }

            out_file_size = ftell(file_handle);
            if (out_file_size == -1L) {
                LOG_ERROR("ftell failed: read_entire_file(%s)\n", file_name);
                error = ERROR_RESOURCE_NOT_FOUND;
                fclose(file_handle);
                return NULL;
            }

            rewind(file_handle);
            if (ferror(file_handle)) {
                LOG_ERROR("rewind() failed: read_entire_file(%s)\n", file_name);
                error = ERROR_RESOURCE_NOT_FOUND;
                fclose(file_handle);
                return NULL;
            }

            u8* file_data = (u8*)allocator.malloc((byte_t)out_file_size + 1); // +1 for null terminator
            if (fread(file_data, out_file_size, 1, file_handle) != 1) {
                LOG_ERROR("fread() failed: read_entire_file(%s)\n", file_name);
                error = ERROR_RESOURCE_NOT_FOUND;
                allocator.free(file_data);
                fclose(file_handle);

                return nullptr;
            }

            fclose(file_handle);

            out_file_size = out_file_size + 1;

            return file_data;
        }

        DLL load_dll(const char* dll_path, Error& error)  {
            DLL library = dlopen(dll_path, RTLD_LAZY);
            if (!library) {
                LOG_ERROR("dlopen() failed: load_dll(%s)\n", dll_path);
                error = ERROR_RESOURCE_NOT_FOUND;

                return nullptr;
            }

            return library;
        }

        DLL free_dll(DLL dll, Error& error)  {
            RUNTIME_ASSERT(dll);
            dlclose(dll);

            return nullptr;
        }

        void* get_proc_address(DLL dll, const char* proc_name, Error& error) {
            RUNTIME_ASSERT(dll);

            void* proc = dlsym(dll, proc_name);
            if (!proc) {
                LOG_ERROR("dlsym() failed: ckg_os_get_proc_address(%s)\n", proc_name);
                error = ERROR_RESOURCE_NOT_FOUND;

                return nullptr;
            }

            return proc;
        }
    }
#endif