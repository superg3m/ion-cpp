#pragma once

#include "../Common/common.hpp"

namespace DS {
    template<typename T>
    struct Stack;
}

namespace Memory {
    struct BaseAllocator {
        virtual ~BaseAllocator() = default;

        virtual void* malloc(byte_t allocation_size) = 0;
        virtual void free(void* data) = 0;
        virtual void* realloc(void* data, byte_t old_allocation_size, byte_t new_allocation_size) = 0;

        bool is_valid() const {
            return this->valid;
        }
    protected:
        bool valid = false;
    };

    struct GeneralAllocator : public BaseAllocator {
        GeneralAllocator();
        ~GeneralAllocator();

        void* malloc(byte_t allocation_size) override;
        void free(void* data) override;
        void* realloc(void* data, byte_t old_allocation_size, byte_t new_allocation_size) override;
    };

    static GeneralAllocator global_general_allocator = GeneralAllocator();

    enum ArenaFlag {
        ARENA_FLAG_FIXED       = 0x1,
        ARENA_FLAG_CIRCULAR    = 0x2,
        ARENA_FLAG_STACK_MEMORY= 0x4,
        ARENA_FLAG_COUNT       = 4,
    };

    struct ArenaAllocator : public BaseAllocator {
        ArenaAllocator();
        ~ArenaAllocator();

        ArenaAllocator(void* memory, byte_t allocation_size, int flags, u8 alignment);

        static ArenaAllocator Fixed(void* memory, byte_t capacity, bool is_stack_memory);
        static ArenaAllocator Circular(void* memory, byte_t memory_capacity, bool is_stack_memory);

        void* malloc(byte_t allocation_size) override;
        void free(void* data) override;
        void free(byte_t byte_to_free);
        void* realloc(void* data, byte_t old_allocation_size, byte_t new_allocation_size) override;

    private:
        int flags = 0;
        byte_t used = 0;
        byte_t used_save_point = 0;
        byte_t capacity = 0;
        u8 alignment = 0;
        u8* base_address = nullptr;

        DS::Stack<byte_t>* size_stack;

        bool data_is_poppable(void* data);
    };
}
