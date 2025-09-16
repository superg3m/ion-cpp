#include "platform.hpp"

#if defined(PLATFORM_WINDOWS)
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <windowsx.h>
    #include <timeapi.h>

    #include "../Common/logger.hpp"
    #include "../Common/assert.hpp"
    #include "../Common/error.hpp"

    namespace Platform {
        global double g_frequency = {0};
        global double g_start_time = {0};
        global TIMECAPS g_device_time_caps;

        bool initialize() {
            LARGE_INTEGER frequency;
            QueryPerformanceFrequency(&frequency);
            g_frequency = (double)frequency.QuadPart;

            LARGE_INTEGER start_time;
            QueryPerformanceCounter(&start_time);
            g_start_time = (double)start_time.QuadPart;
            
            if (timeGetDevCaps(&g_device_time_caps, sizeof(TIMECAPS)) != MMSYSERR_NOERROR) {
                MessageBoxA(0, "Failed to get timecaps!", "Error!", MB_ICONEXCLAMATION | MB_OK);

                return false;
            }

            LOG_DEBUG("Platform initalization!\n");
            return true;
        }

        void shutdown() {
            timeEndPeriod(g_device_time_caps.wPeriodMin);
            LOG_DEBUG("Platform Shutdown!\n");
        }

        bool file_path_exists(const char* path) {
            return (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES);
        }

        bool copy_file(const char* source_path, const char* dest_path, bool block_until_success) {
            if (block_until_success) {
                while (!CopyFileA(source_path, dest_path, FALSE)) {
                    sleep(10);
                }

                return true;
            } 

            return CopyFileA(source_path, dest_path, FALSE) == 0;
        }

        void sleep(u32 ms) {
            Sleep((DWORD)ms);
        }

        double get_seconds_elapsed() {
            LARGE_INTEGER counter = {0};
            QueryPerformanceCounter(&counter);
            double time_in_seconds = (double)(counter.QuadPart - g_start_time) / (double)g_frequency;
        
            return time_in_seconds;
        }

        internal void* win32_malloc(const Memory::BaseAllocator** allocator, byte_t allocation_size) {
            (void)allocator;
            return VirtualAlloc(nullptr, allocation_size, MEM_COMMIT, PAGE_READWRITE);
        }

        internal void win32_free(const Memory::BaseAllocator** allocator, void* data) {
            (void)allocator;
            VirtualFree(data, 0, MEM_RELEASE);
        }

        Memory::BaseAllocator* get_allocator() {
            return Memory::BaseAllocator*(win32_malloc, win32_free);
        }

        u8* read_entire_file(Memory::BaseAllocator* allocator, const char* file_path, byte_t& out_file_size, Error& error) {
            HANDLE file_handle = CreateFileA(file_path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (file_handle == INVALID_HANDLE_VALUE) {
                LOG_ERROR("CreateFileA() returned an INVALID_HANDLE_VALUE, the file_path/path is likely wrong: read_entire_file(%s)\n", file_path);
                error = ERROR_RESOURCE_NOT_FOUND;
                
                return nullptr;
            }

            LARGE_INTEGER large_int;
            Memory::zero(&large_int, sizeof(LARGE_INTEGER));
            BOOL success = GetFileSizeEx(file_handle, &large_int);
            if (!success) {
                LOG_ERROR("GetFileSizeEx() Failed to get size from file_handle: read_entire_file(%s)\n", file_path);
                error = ERROR_RESOURCE_NOT_FOUND;
                CloseHandle(file_handle);
                return nullptr;
            }

            byte_t file_size = (byte_t)large_int.QuadPart;
            if (file_size > SIZE_MAX) {
                LOG_ERROR("File size is bigger than max size: read_entire_file(%s)\n", file_path);
                error = ERROR_RESOURCE_TOO_BIG;
                CloseHandle(file_handle);

                return nullptr;
            }

            u8* file_data = (u8*)allocator->malloc(file_size + 1); // +1 for null-terminator

            DWORD bytes_read = 0;
            success = ReadFile(file_handle, file_data, (DWORD)file_size, &bytes_read, nullptr);
            CloseHandle(file_handle);
            if (!success && bytes_read == file_size) {
                LOG_ERROR("ReadFile() Failed to get the file data or bytes read doesn't match file_size: read_entire_file(%s)\n", file_path);
                error = ERROR_RESOURCE_NOT_FOUND;
                allocator->free(file_data);
                return nullptr;
            }

            out_file_size = (byte_t)file_size;

            return file_data;
        }

        DLL load_dll(const char* dll_path, Error& error)  {
            HMODULE library = LoadLibraryA(dll_path);
            if (!library) {
                LOG_ERROR("LoadLibraryA() failed: load_dll(%s)\n", dll_path);
                error = ERROR_RESOURCE_NOT_FOUND;

                return nullptr;
            }

            return library;
        }

        DLL free_dll(DLL dll, Error& error)  {
            RUNTIME_ASSERT(dll);
            FreeLibrary((HMODULE)dll);

            return nullptr;
        }

        void* get_proc_address(DLL dll, const char* proc_name, Error& error) {
            RUNTIME_ASSERT(dll);

            void* proc = (void*)GetProcAddress((HMODULE)dll, proc_name);
            if (!proc) {
                LOG_ERROR("GetProcAddress() failed: get_proc_address(%s)\n", proc_name);
                error = ERROR_RESOURCE_NOT_FOUND;
                return nullptr;
            }

            return proc;
        }
    }
#endif