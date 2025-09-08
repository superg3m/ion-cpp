#pragma once

#include <initializer_list>
#include <type_traits>

#include "../Memory/memory.hpp"
#include "../Common/common.hpp"

namespace Container {
    template <typename T>
    struct Vector {
        Vector(std::initializer_list<T> list) {
            this->m_count = list.size();
            this->m_capacity = this->m_count * 2;
            this->m_allocator = Memory::Allocator::libc();

            this->m_data = (T*)this->m_allocator.malloc(this->m_capacity * sizeof(T));
            Memory::copy(this->m_data, this->m_capacity * sizeof(T), list.begin(), list.size() * sizeof(T));
        }

        Vector(std::initializer_list<T> list, Memory::Allocator allocator) {
            this->m_count = list.size();
            this->m_capacity = this->m_count * 2;
            this->m_allocator = allocator;

            this->m_data = (T*)this->m_allocator.malloc(this->m_capacity * sizeof(T));
            Memory::copy(this->m_data, this->m_capacity * sizeof(T), list.begin(), list.size() * sizeof(T));
        }

        Vector(u64 capacity = 1, Memory::Allocator allocator = Memory::Allocator::libc()) {
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
            this->destroy();

            this->m_count = other.m_count;
            this->m_capacity = other.m_capacity;
            this->m_data = other.m_data;
            this->m_allocator = other.m_allocator;

            // Leave other in a valid empty state
            other.m_count    = 0;
            other.m_capacity = 0;
            other.m_data     = nullptr;
            other.m_allocator = Memory::Allocator::invalid();

            return *this;
        }

        ~Vector() {
            this->destroy();
        }

        void resize(u64 count) {
            RUNTIME_ASSERT_MSG(false, "resize not implemented!\n");
        }

        void destroy() {
            if (this->m_data) {
                for (byte_t i = 0; i < m_count; ++i) {
                    this->m_data[i].~T();
                }
            }

            this->m_allocator.free(this->m_data);

            this->m_data = nullptr;
            this->m_count = 0;
            this->m_capacity = 0;
            this->m_allocator = Memory::Allocator::invalid();
        }

        void push(T value) {
            if (this->m_capacity < this->m_count + 1) {
                this->grow();
            }

            this->m_data[this->count()] = value;
            this->m_count += 1;
        }

        void unstable_swapback_remove(int i) {
            RUNTIME_ASSERT_MSG(this->m_count > 0, "You may not remove if the vector is empty!\n");
            RUNTIME_ASSERT_MSG((i >= 0) && (this->m_count - 1 >= i), "index is outside of bounds!\n");

            this->m_count -= 1;
            if (this->m_count == 0) {
                this->m_data[i] = this->m_data[this->count()];
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
        Memory::Allocator m_allocator = Memory::Allocator::invalid();
    };

    template <typename T>
    struct Stack {
        Stack(u64 capacity = 1, Memory::Allocator* allocator = nullptr) {
            this->m_count = 0;
            this->m_capacity = capacity;
            this->m_allocator = allocator;

            this->m_allocator->malloc(this->m_capacity * sizeof(T));
        }

        ~Stack() {
            this->m_allocator->free(this->data);

            this->m_data = nullptr;
            this->m_count = 0;
            this->m_capacity = 0;
            this->m_allocator = Memory::Allocator::invalid();
        }

        T peek() const {
            RUNTIME_ASSERT_MSG(!this->empty(), "You may not peek if the stack is empty!\n");

            return this->data[this->count() - 1];
        }

        void push(T value) {
            if (this->capactiy < this->m_count + 1) {
                this->grow();
            }

            this->data[this->count()] = value;
            this->count += 1;
        }

        T pop() {
            RUNTIME_ASSERT_MSG(!this->empty(), "You may not pop if the stack is empty!\n");

            this->m_count -= 1;
            return this->data[this->m_count()];
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
            byte_t old_allocation_size = (this->m_capactiy * sizeof(T));
            this->capactiy *= 2;
            byte_t new_allocation_size = (this->m_capactiy * sizeof(T));
            this->m_data = this->m_allocator->realloc(this->data, old_allocation_size, new_allocation_size);
        }
    private:
        T* m_data;
        u64 m_count;
        u64 m_capacity;
        Memory::Allocator* m_allocator;
    };
    
    template <typename T>
    struct RingQueue {
        RingQueue(byte_t capacity = 1, Memory::Allocator allocator = Memory::Allocator::invalid()) {
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
            this->m_allocator = Memory::Allocator::invalid();
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
        Memory::Allocator m_allocator = Memory::Allocator::invalid();

        byte_t m_read = 0;
        byte_t m_write = 0;
    };
}