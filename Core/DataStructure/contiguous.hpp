#pragma once

#include <initializer_list>
#include <type_traits>

#include "../Memory/memory.hpp"
#include "../Common/common.hpp"

namespace DS {
    template <typename T>
    struct Vector {
        Vector(std::initializer_list<T> list, Memory::BaseAllocator& allocator = Memory::global_general_allocator) : m_allocator(allocator) {
            this->m_count = list.size();
            this->m_capacity = this->m_count * 2;
            this->m_allocator = allocator;

            this->m_data = (T*)this->m_allocator.malloc(this->m_capacity * sizeof(T));
            Memory::copy(this->m_data, this->m_capacity * sizeof(T), list.begin(), list.size() * sizeof(T));
        }

        Vector(u64 capacity = 1, Memory::BaseAllocator& allocator = Memory::global_general_allocator) : m_allocator(allocator) {
            this->m_count = 0;
            this->m_capacity = capacity;
            this->m_allocator = allocator;

            this->m_data = (T*)this->m_allocator.malloc(this->m_capacity * sizeof(T));
        }

        // Prevent copy
        Vector(const Vector&) = delete;
        Vector& operator=(const Vector&) = delete;

        // Prevent move
        Vector(Vector&&) = delete;
        Vector& operator=(Vector&& other) {
            RUNTIME_ASSERT(&this->m_allocator == &other.m_allocator && "Cannot move between vectors with different allocators!");

            this->destory();

            this->m_count = other.m_count;
            this->m_capacity = other.m_capacity;
            this->m_data = other.m_data;

            // Leave other in a invalid empty state
            other.m_count    = 0;
            other.m_capacity = 0;
            other.m_data     = nullptr;

            return *this;
        }

        ~Vector() {
            this->destory();
        }

        void resize(u64 count) {
            RUNTIME_ASSERT_MSG(false, "resize not implemented!\n");
        }

        void push(T value) {
            if (this->m_capacity < this->m_count + 1) {
                this->grow();
            }

            this->m_data[this->m_count] = value;
            this->m_count += 1;
        }

        void unstable_swapback_remove(int i) {
            RUNTIME_ASSERT_MSG(this->m_count > 0, "You may not remove if the vector is empty!\n");
            RUNTIME_ASSERT_MSG((i >= 0) && (this->m_count - 1 >= i), "index is outside of bounds!\n");

            this->m_count -= 1;
            if (this->m_count == 0) {
                this->m_data[i] = this->m_data[this->m_count];
            }
        }

        T* begin() { 
            return m_data; 
        }
        T* end() { 
            return m_data + m_count; 
        }
        const T* begin() const { 
            return m_data; 
        }
        const T* end() const { 
            return m_data + m_count; 
        }

        u64 count() const {
            return this->m_count;
        }

        u64 capacity() const {
            return this->m_capacity;
        }

        T* data() const {
            return this->m_data;
        }
        
        T operator[](int i) const {
            RUNTIME_ASSERT_MSG((i >= 0) && (this->m_count - 1 >= i), "index is outside of bounds!\n");

            return this->m_data[i];
        }

        T& operator[](int i) {
            RUNTIME_ASSERT_MSG((i >= 0) && (this->m_count - 1 >= i), "index is outside of bounds!\n");

            return this->m_data[i];
        }
        
        void grow() {
            byte_t old_allocation_size = (this->m_capacity * sizeof(T));
            this->m_capacity *= 2;
            byte_t new_allocation_size = (this->m_capacity * sizeof(T));
            this->m_data = (T*)this->m_allocator.realloc(this->m_data, old_allocation_size, new_allocation_size);
        }
    private:
        T* m_data = nullptr;
        u64 m_count = 0;
        u64 m_capacity = 0;
        Memory::BaseAllocator& m_allocator;

        void destory() {
            this->m_allocator.free(this->m_data);

            this->m_count = 0;
            this->m_capacity = 0;
            this->m_data = nullptr;
        }
    };

    template <typename T>
    struct Stack {
        Stack(u64 capacity = 1, Memory::BaseAllocator& allocator = Memory::global_general_allocator) : m_allocator(allocator) {
            this->m_count = 0;
            this->m_capacity = capacity;

            this->m_data = (T*)this->m_allocator.malloc(this->m_capacity * sizeof(T));
        }

        ~Stack() {
            this->free();
        }

        void free() {
            if (this->m_data) {
                for (byte_t i = 0; i < m_count; ++i) {
                    this->m_data[i].~T();
                }
            }

            this->m_allocator.free(this->m_data);

            this->m_data = nullptr;
            this->m_count = 0;
            this->m_capacity = 0;
        }

        T peek() const {
            RUNTIME_ASSERT_MSG(!this->empty(), "You may not peek if the stack is empty!\n");

            return this->m_data[this->count() - 1];
        }

        void push(T value) {
            if (this->m_capacity < this->m_count + 1) {
                this->grow();
            }

            this->m_data[this->m_count] = value;
            this->m_count += 1;
        }

        T pop() {
            RUNTIME_ASSERT_MSG(!this->empty(), "You may not pop if the stack is empty!\n");

            this->m_count -= 1;
            return this->m_data[this->m_count];
        }

        bool empty() const {
            return this->m_count == 0;
        }

        u64 count() const {
            return this->m_count;
        }

        u64 capacity() const {
            return this->m_capacity;
        }

        void grow() {
            byte_t old_allocation_size = (this->m_capacity * sizeof(T));
            this->m_capacity *= 2;
            byte_t new_allocation_size = (this->m_capacity * sizeof(T));
            this->m_data = (T*)this->m_allocator.realloc(this->m_data, old_allocation_size, new_allocation_size);
        }
    private:
        T* m_data = nullptr;
        u64 m_count = 0;
        u64 m_capacity = 0;
        Memory::BaseAllocator& m_allocator;
    };
    
    template <typename T>
    struct RingQueue {
        RingQueue(byte_t capacity = 1, Memory::BaseAllocator& allocator = Memory::global_general_allocator) : m_allocator(allocator) {
            RUNTIME_ASSERT(capacity > 0);
            
            this->m_count = 0;
            this->m_capacity = capacity;
            this->m_data = this->m_allocator->malloc(this->m_capacity * sizeof(T));
        }

        ~RingQueue() {
            this->m_allocator->free(this->m_data);

            this->m_data = nullptr;
            this->m_count = 0;
            this->m_capacity = 0;

        }

        bool enqueue(T value) {
            RUNTIME_ASSERT_MSG(!this->full(), "You may not enqueue if the ring queue is full!\n");

            this->data[this->m_write] = value;
            this->m_count += 1;
            this->write = (this->write + 1) % this->m_capacity;
        }
        
        T dequeue() {
            RUNTIME_ASSERT_MSG(!this->empty(), "You may not dequeue if the ring queue is empty!\n");

            T ret = this->data[this->m_read];
            this->m_count -= 1;
            this->m_read = (this->m_read + 1) % this->m_capacity;

            return ret;
        }

        bool empty() const {
            return this->m_count == 0;
        }

        bool full() const {
            return this->m_count == this->m_capacity;
        }

        u64 count() const {
            return this->m_count;
        }

        u64 capacity() const {
            return this->m_capacity;
        }

    private:
        T* m_data = nullptr;
        u64 m_count = 0;
        u64 m_capacity = 0;
        Memory::BaseAllocator& m_allocator;

        byte_t m_read = 0;
        byte_t m_write = 0;
    };
}