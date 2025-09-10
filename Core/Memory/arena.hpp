#pragma once

#include "../core.hpp"

#define ARENA_DEFAULT_ALLOCATION_SIZE MegaBytes(1)

namespace Memory {
    enum ArenaFlag {
        ARENA_FLAG_INVALID = -1,
        ARENA_FLAG_FIXED = 0x1,
        ARENA_FLAG_CIRCULAR = 0x2,
        ARENA_FLAG_STACK_MEMORY = 0x4,
        ARENA_FLAG_COUNT = 4,
    };

    struct Arena {
        int flags;
        byte_t used;
        byte_t used_save_point;
        byte_t capacity;
        u8 alignment;
        u8* base_address;
        DS::Stack<byte_t> size_stack;
        Memory::Allocator allocator;

        Arena(Memory::Allocator allocator, void* memory, byte_t allocation_size, int flags, u8 alignment);
        ~Arena();

        static Arena Fixed(void* memory, byte_t allocation_size, bool is_stack_memory, Memory::Allocator allocator = Memory::Allocator::invalid());
        static Arena Circular(void* memory, byte_t allocation_size, bool is_stack_memory, Memory::Allocator allocator = Memory::Allocator::invalid());

        void* push(byte_t allocation_size);
        void pop(byte_t bytes_to_pop);
        void pop(void* data);
        void free();
    };
}