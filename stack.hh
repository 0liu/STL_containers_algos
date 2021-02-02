#ifndef __STACK_H_
#define __STACK_H_


#include <iostream>
#include <memory>
#include "exception.hh"
#include "list.hh"


using std::ostream;
using std::shared_ptr;
using ADT::Empty;
using ADT::Overflow;
using ADT::LinkedList;


namespace ADT {

    template <typename T> class ArrayStack;
    template <typename T> ostream& operator<<(ostream&, const ArrayStack<T>&);


    template <typename T>
    class ArrayStack {
        /*
        ** Array based stack with a specified capacity.
        ** Modified from DSAC code fragment 5.4.
        ** Removed exception specification and use shared_ptr for array.
        ** Template forward declarations
         */
        enum {DEFAULT_CAPACITY = 100};  // default stack capacity
        public:
            ArrayStack(int cap=DEFAULT_CAPACITY)
                : arr(new T[cap]), capacity(cap), t(-1) {}        
            int max_len() const {return capacity;}
            int size() const {return t + 1;}
            bool empty() const {return t < 0;}
            const T& top() const {
                if (empty())
                    throw Empty("Top of empty stack");
                return arr[t];        
            }
            void push(const T& x) {
                if (size() == capacity)
                    throw Overflow("Push to full stack");
                arr[++t] = x;
            }
            void pop() {
                if (empty())
                    throw Empty("Pop from empty stack");
                --t;
            };
            friend ostream& operator<< <>(ostream&, const ArrayStack<T>&);
        private:
            shared_ptr<T[]> arr;  // array of stack elements
            int capacity;  // stack max capacity
            int t;  // array index of stack top
    };


    template <typename T>
    ostream& operator<<(ostream& os, const ArrayStack<T>& stk) {
        for (int i=0; i<stk.size(); i++)
            os << stk.arr[i] << ' ';
        return os;
    }



    template <typename T> class LinkedStack;
    template <typename T> ostream& operator<<(ostream&, const LinkedStack<T>&);

    template <typename T>
    class LinkedStack {
        /*
        ** LIFO stack, implemented with a singly linked list.
        ** DSAC 5.7, 5.8. Shaffer DSAA 4.19.
         */
        public:
            LinkedStack() : ll(), n(0) {}
            int size() const {return n;}
            bool empty() const {return n == 0;}
            const T& top() const {
                if (empty())
                    throw Empty("Top of empty stack");
                return ll.front();
            }
            void push(const T& x) {
                ++n;
                ll.insert_front(x);
            }
            void pop() {
                if (empty())
                    throw Empty("Pop from empty stack");
                --n;
                ll.remove_front();
            }
            friend ostream& operator<< <>(ostream&, const LinkedStack<T>&);
        private:
            LinkedList<T> ll;
            int n;  // Number of current elements
    };


    template<typename T>
    ostream& operator<<(ostream& os, const LinkedStack<T>& linkstk) {
        return os << linkstk.ll;
    }

    
}  // end of namespace ADT


#endif // __STACK_H_
