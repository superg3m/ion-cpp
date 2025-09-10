#include "allocator.hpp"
#include "../DataStructure/ds.hpp"
#include <cstdlib>
#include <new>

namespace Memory {
    GeneralAllocator::GeneralAllocator() {
        this->valid = true;
    }

    GeneralAllocator::~GeneralAllocator() {
        this->valid = false;
    }

    void* GeneralAllocator::malloc(byte_t allocation_size) {
        return std::malloc(allocation_size);
    }

    void GeneralAllocator::free(void* data) {
        std::free(data);
    }

    void* GeneralAllocator::realloc(void* data, byte_t old_allocation_size, byte_t new_allocation_size) {
        RUNTIME_ASSERT_MSG(this->valid, "Allocator is invalid!\n");
        RUNTIME_ASSERT(old_allocation_size != 0);
        RUNTIME_ASSERT(new_allocation_size != 0);

        void* ret = this->malloc(new_allocation_size);
        Memory::copy(ret, new_allocation_size, data, old_allocation_size);
        this->free(data);

        return ret;
    }


    ArenaAllocator::ArenaAllocator() {
        this->valid = true;
    }

    ArenaAllocator::~ArenaAllocator() {
        if (!(this->flags & ARENA_FLAG_STACK_MEMORY)) {
            this->free(this->base_address);
        }

        this->valid = false;
    }

    ArenaAllocator::ArenaAllocator(void* memory, byte_t allocation_size, int flags, u8 alignment) {
        RUNTIME_ASSERT_MSG(memory, "Memory can't be a null pointer!\n");
        RUNTIME_ASSERT_MSG(allocation_size != 0, "Can't have a zero allocation size!\n");
        int mask = ARENA_FLAG_FIXED | ARENA_FLAG_CIRCULAR;
        RUNTIME_ASSERT_MSG((flags & mask) != mask, "Can't have both a fixed and circular arena!\n");

        this->valid = true;
        this->flags = flags;
        this->used = 0;
        this->used_save_point = 0;
        this->capacity = allocation_size;
        this->alignment = alignment;
        this->base_address = (u8*)memory;
        DS::Stack<byte_t>* address = (DS::Stack<byte_t>*)this->malloc(sizeof(DS::Stack<byte_t>));
        this->size_stack = new (address) DS::Stack<byte_t>();
    }

    ArenaAllocator ArenaAllocator::Fixed(void* memory, byte_t capacity, bool is_stack_memory) {
        int flags = ARENA_FLAG_FIXED | (is_stack_memory ? ARENA_FLAG_STACK_MEMORY : 0);
        return ArenaAllocator(memory, capacity, flags, 8);
    }

    ArenaAllocator ArenaAllocator::Circular(void* memory, byte_t capacity, bool is_stack_memory) {
        int flags = ARENA_FLAG_CIRCULAR | (is_stack_memory ? ARENA_FLAG_STACK_MEMORY : 0);
        return ArenaAllocator(memory, capacity, flags, 8);
    }

    void* ArenaAllocator::malloc(byte_t allocation_size) {
        RUNTIME_ASSERT_MSG(this->valid, "Allocator is invalid!\n");
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
        if ((this->used & (this->alignment - 1)) != 0) {
            this->used += (this->alignment - (this->used & (this->alignment - 1)));
        }

        if (this->size_stack) {
            this->size_stack->push(this->used - previous_used);
        }

        return ret;
    }

    void ArenaAllocator::free(void* data) {
        RUNTIME_ASSERT(data);
        RUNTIME_ASSERT_MSG(this->valid, "Allocator is invalid!\n");

        if (this->data_is_poppable(data)) {
            this->used -= this->size_stack->pop();
        }
    }

    void ArenaAllocator::free(byte_t byte_to_free) {
        RUNTIME_ASSERT_MSG(this->valid, "Allocator is invalid!\n");
        this->used -= byte_to_free;
    }

    void* ArenaAllocator::realloc(void* data, byte_t old_allocation_size, byte_t new_allocation_size) {
        RUNTIME_ASSERT_MSG(this->valid, "Allocator is invalid!\n");
        RUNTIME_ASSERT(old_allocation_size != 0);
        RUNTIME_ASSERT(new_allocation_size != 0);

        if (this->data_is_poppable(data)) {
            this->free(data);
            return this->malloc(new_allocation_size);
        }

        void* ret = this->malloc(new_allocation_size);
        Memory::copy(ret, new_allocation_size, data, old_allocation_size);
        this->free(data);

        return ret;
    }

    bool ArenaAllocator::data_is_poppable(void* data) {
        byte_t bytes_to_pop = this->size_stack->peek();
        return this->base_address + (this->used - bytes_to_pop) == data;
    }
}
