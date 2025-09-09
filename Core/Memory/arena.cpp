#include "arena.hpp"

namespace Memory {
    Arena::Arena(Memory::Allocator allocator, void* memory, byte_t allocation_size, int flags, u8 alignment) : allocator(allocator) {
        RUNTIME_ASSERT_MSG(memory, "Memory can't be a null pointer!\n");
        RUNTIME_ASSERT_MSG(allocation_size != 0, "Can't have a zero allocation size!\n");
        RUNTIME_ASSERT_MSG(!((flags & ARENA_FLAG_CIRCULAR) && (flags & ARENA_FLAG_FIXED)), "Can't have both a fixed an circular arena!\n");

        this->flags = flags;
        this->used = 0;
        this->used_save_point = 0;
        this->capacity = allocation_size;
        this->alignment = alignment;
        this->base_address = (u8*)memory;
    }

    Arena::~Arena() {
        RUNTIME_ASSERT_MSG(this->flags != ARENA_FLAG_INVALID, "Arena is invalid!\n");

        if (!(this->flags & ARENA_FLAG_STACK_MEMORY)) {
            this->allocator.free(this->base_address);
        }

        this->flags = ARENA_FLAG_INVALID;
    }

    Arena Arena::Fixed(void* memory, byte_t allocation_size, bool is_stack_memory, Memory::Allocator allocator) {
        if (is_stack_memory) {
            RUNTIME_ASSERT(!allocator.valid());
        } else if (!is_stack_memory) {
            RUNTIME_ASSERT(allocator.valid());
        }

        return Arena(allocator, memory, allocation_size, ARENA_FLAG_FIXED|(is_stack_memory ? ARENA_FLAG_STACK_MEMORY : 0), 8);
    }

    Arena Arena::Circular(void* memory, byte_t allocation_size, bool is_stack_memory, Memory::Allocator allocator) {
        if (is_stack_memory) {
            RUNTIME_ASSERT(!allocator.valid());
        } else if (!is_stack_memory) {
            RUNTIME_ASSERT(allocator.valid());
        }

        return Arena(allocator, memory, allocation_size, ARENA_FLAG_CIRCULAR|(is_stack_memory ? ARENA_FLAG_STACK_MEMORY : 0), 8);
    }

    void* Arena::push(byte_t allocation_size) {
        RUNTIME_ASSERT_MSG(this->flags != ARENA_FLAG_INVALID, "Arena is invalid!\n");
        RUNTIME_ASSERT_MSG(allocation_size != 0, "Element size can't be zero!\n");

        if (this->flags & ARENA_FLAG_FIXED) {
            RUNTIME_ASSERT_MSG(this->used + allocation_size <= this->capacity, "Ran out of arena memory!\n");
        } else if (this->flags & ARENA_FLAG_CIRCULAR) {
            if ((this->used + allocation_size > this->capacity)) {
                this->used = 0;
                RUNTIME_ASSERT_MSG(this->used + allocation_size <= this->capacity, "Element size exceeds circular arena allocation capacity!\n");
            }
        }

        u8* ret = this->base_address + this->used;
        byte_t previous_used = this->used;
        this->used += allocation_size;
        if ((this->used & (this->alignment - 1)) != 0) { // if first bit is set then its not aligned
            this->used += (this->alignment - (this->used & (this->alignment - 1)));
        }

        this->size_stack.push(this->used - previous_used);

        return ret;
    }

    void Memory::Arena::pop(byte_t bytes_to_pop) {
        RUNTIME_ASSERT_MSG(this->flags != ARENA_FLAG_INVALID, "Arena is invalid!\n");

        this->used -= bytes_to_pop;
    }

    void Memory::Arena::pop(void* data) {
        RUNTIME_ASSERT(data);

        byte_t bytes_to_pop = this->size_stack.peek();
        
        if (this->base_address + (this->used - bytes_to_pop) == data) {
            this->used -= bytes_to_pop;
            this->size_stack.pop();
        }
    }

    void Arena::free() {
        /*
        RUNTIME_ASSERT_MSG(this->flags != ARENA_FLAG_INVALID, "Arena is invalid!\n");

        if (!(this->flags & ARENA_FLAG_STACK_MEMORY)) {
            ckg_free(this->base_address);
        }

        if (arena->size_stack) {
            Alloc_T* a = global_allocator.allocate;
            Free_T* f  = global_allocator.free;
            void* ctx  = global_allocator.ctx;
            ckg_bind_custom_allocator(ckg_default_libc_malloc, ckg_default_libc_free, NULLPTR);
            this->size_stack.
            ckg_stack_free(arena->size_stack);
            ckg_bind_custom_allocator(a, f, ctx);
        }

        arena->flags = ARENA_FLAG_INVALID;
        */
    }
}