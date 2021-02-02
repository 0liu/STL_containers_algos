#ifndef __VECTOR_H_
#define __VECTOR_H_


#include <iostream>
#include <utility>
#include "exception.hh"


namespace ADT {

    template <typename T>
    class vector {
        /*
        ** TODO: Need iterator, const_iterator and reverse iterator classes.
         */

        public:
            // Use pointer as iterator
            using iterator = T*;
            using const_iterator = const T*; 

            // Constructors and destructor
            vector() : m_size{0}, m_capacity{0}, m_buffer{nullptr} {}
            explicit vector(size_t n) : m_size{n}, m_capacity{n}, m_buffer{new T[n]} {}
            explicit vector(size_t n, const T& init_val)
                : m_size{n}, m_capacity{n}, m_buffer{new T[n]} {
                for (size_t i = 0; i < n; ++i)
                    m_buffer[i] = init_val;
            }
            ~vector() { delete[] m_buffer; }

            // Copy constructor, move constructor, and copy and move assignment operators
            friend void swap(vector<T>& va, vector<T>& vb) noexcept {
                std::swap(va.m_buffer, vb.m_buffer);
                std::swap(va.m_size, vb.m_size);
                std::swap(va.m_capacity, vb.m_capacity);
            }
            vector(const vector<T>& v)
                : m_size{v.m_size}, m_capacity{v.m_capacity}, m_buffer{new T[v.m_size]} {
                std::memcpy(m_buffer, v.m_buffer, v.m_size * sizeof(T));
            }
            vector(vector<T>&& v) noexcept : vector<T>() { swap(*this, v); }
            vector<T>& operator=(vector<T> v) { swap(*this, v); return *this; }

            // Iterators
            iterator begin() { return m_buffer; }
            iterator end() { return m_buffer + m_size; }
            const_iterator begin() const { return m_buffer; }
            const_iterator end() const { return m_buffer + m_size; }


            // Capacity
            size_t size() const { return m_size; }
            size_t capacity() const { return m_capacity; }
            bool empty() const { return m_size == 0; }
            void reserve(size_t capacity);
            void resize(size_t n, const T& val = T());

            // Element access
            T& operator[](size_t index) { return m_buffer[index]; }
            T& front() { return m_buffer[0]; }
            T& back() { return m_buffer[m_size - 1]; }

            // Modifiers
            void push_back(const T& val);
            void pop_back();
            void clear();

        private:
            size_t m_size;
            size_t m_capacity;
            T* m_buffer;
    };

    template <typename T>
    void vector<T>::reserve(size_t capacity) {
        // Resize the storage
        if (capacity > m_capacity) {
            size_t new_capacity = capacity + (capacity >> 3) + (capacity < 9 ? 3 : 6);  // CPython list resize strategy
            T* new_array = new T[new_capacity];
            std::memcpy(new_array, m_buffer, m_size * sizeof(T));
            delete[] m_buffer;
            m_buffer = new_array;
            m_capacity = new_capacity;
        }
    }

    template <typename T>
    void vector<T>::resize(size_t n, const T& val) {
        // Resize the array with the same strategy as std::vector::resize().
        if (n > m_capacity)
            reserve(n);
        if (n < m_size) {  // shrink array
            for (size_t i = n; i < m_size; ++i)
                (m_buffer + i) -> ~T();
        }
        else {  // expand array
            for (size_t i = m_size; i < n; ++i)
                m_buffer[i] = val;
        }
        m_size = n;
    }

    template <typename T>
    void vector<T>::push_back(const T& val) {
        if (m_size == m_capacity)
            reserve(m_capacity + 1);
        m_buffer[m_size++] = val;
    }

    template <typename T>
    void vector<T>::pop_back() {
        m_buffer[--m_size].~T();
    }

    template <typename T>
    void vector<T>::clear() {
        delete[] m_buffer;
        m_buffer = nullptr;
        m_size = m_capacity = 0;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& os, const vector<T>& v) {
        os << '[';
        for (auto p = v.begin(); p != v.end(); ++p) {
            os << *p;
            if (p+1 != v.end())
                os << ' ';
        }
        return os << ']';
    }
            

}  // end of namespace ADT


#endif // __VECTOR_H_
