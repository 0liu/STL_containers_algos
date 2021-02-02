#ifndef __QUEUE_H_
#define __QUEUE_H_


#include <iostream>
#include <memory>
#include "exception.hh"
#include "list.hh"


namespace ADT {

    using std::ostream;
    using std::shared_ptr;
    using ADT::Empty;
    using ADT::Overflow;
    using ADT::Underflow;    
    using ADT::CLinkedList;
    using ADT::DLinkedList;


    template <typename T> class ArrayQueue;
    template <typename T> ostream& operator<<(ostream&, const ArrayQueue<T>&);


    template <typename T>
    class ArrayQueue {
        /*
        ** Array based queue with a specified capacity.
        ** DSAC 5.17 and Shaffer DSAA 4.26.
        */

        enum {DEFAULT_CAPACITY = 100};  // default queue capacity

        public:
            ArrayQueue(int cap=DEFAULT_CAPACITY)
                // Reserve one spare slot to differentiate cases of empty and full.
                : arr(new T[cap+1]), capacity(cap+1), f(1), r(0) {}
            int max_len() const {return capacity-1;}
            int size() const {return (r + capacity - f + 1) % capacity;}
            bool empty() const {return size() == 0;}
            bool full() const {return (r+2) % capacity == f;}
            void clear() {r = 0; f = 1;}
            const T& front() const {
                if (empty())
                    throw Empty("Front of empty queue");
                return arr[f];
            }
            void enqueue(const T& x) {
                if (full())
                    throw Overflow("Enqueue to full queue");
                r = (r + 1) % capacity;
                arr[r] = x;
            }
            T& dequeue() {
                if (empty())
                    throw Underflow("Dequeue from empty queue");
                T& x {arr[f]};
                f = (f + 1) % capacity;
                return x;
            }
            friend ostream& operator<< <>(ostream&, const ArrayQueue<T>&);
        private:
            shared_ptr<T[]> arr;
            int capacity;  // queue max size
            int f;  // array index of queue front
            int r;  // array index of queue rear
    };


    template <typename T>
    ostream& operator<<(ostream& os, const ArrayQueue<T>& q) {
        if (q.empty())
            return os << "[]";
        os << '[' << q.front();
        for (int i = (q.f + 1) % q.capacity; i != (q.r + 1) % q.capacity; i = (i+1) % q.capacity)
            os << ' ' << q.arr[i];
        os << ']';
        return os;
    }


    /* Single-ended queue */
    template <typename T> class LinkedQueue;
    template <typename T> ostream& operator<<(ostream&, const LinkedQueue<T>&);

    template <typename T>
    class LinkedQueue {
        /*
        ** FIFO queue implemented with a circularly linked list
        ** DSAC 5.18, 5.20.
         */
        public:
            LinkedQueue() : n(0) {}
            int size() const {
                return n;
            }
            bool empty() const {
                return n == 0;
            }
            const T& front() const {
                if (empty())
                    throw Empty("front() of empty LinkedQueue");
                return cl.front();
            }
            void enqueue(const T& e) {
                cl.insert(e);
                cl.advance();
                n++;
            }
            void dequeue() {
                if (empty()) {
                    throw Empty("dequeue() of empty LinkedQueue");
                }
                cl.remove();
                n--;
            }
            friend ostream& operator << <>(ostream&, const LinkedQueue<T>&);
        private:
            int n;
            CLinkedList<T> cl;
    };

    template<typename T>
    ostream& operator<<(ostream& os, const LinkedQueue<T>& linkqueue) {
        return os << linkqueue.cl;
    }


    /* Deque */
    template <typename T> class Deque;
    template <typename T> ostream& operator<<(ostream&, const Deque<T>&);

    template <typename T>
    class Deque {
        /*
        ** Double-ended queue implemented with doubly linked list.
        ** DSAC 5.21, 5.22.
         */
        public:
            int size() const {
                return n;
            }
            bool empty() const {
                return dl.empty();
            }
            const T& front() const {
                try {
                    return dl.front();
                }
                catch(const Empty& err) {
                    throw Empty("front() of empty Deque");
                }
            }
            const T& back() const {
                try {
                    return dl.back();
                }
                catch(const Empty& err) {
                    throw Empty("back() of empty Deque");
                }
            }
            void insert_front(const T& e) {
                dl.insert_front(e);
            }
            void insert_back(const T& e) {
                dl.insert_back(e);
            }
            void remove_front() {
                dl.remove_front();
            }
            void remove_back() {
                dl.remove_back();
            }
            friend ostream& operator<< <>(ostream&, const Deque<T>&);
        private:
            DLinkedList<T> dl;
            int n;
    };

    template <typename T>
    ostream& operator<<(ostream& os, const Deque<T>& dq) {
        return os << dq.dl;
    }



}  // End of namespace ADT


#endif // __QUEUE_H_
