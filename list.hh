#ifndef __LIST_H_
#define __LIST_H_

/*  Singly and circularly linked list */

#include <iostream>
#include <memory>  // smart pointers
#include "exception.hh"


namespace ADT {

    using std::ostream;
    using std::shared_ptr;
    using std::make_shared;
    using ADT::Empty;


    template <typename T> class LinkedList;
    template <typename T> ostream& operator<<(ostream&, const LinkedList<T>&);
    template <typename T> class CLinkedList;
    template <typename T> ostream& operator<<(ostream&, const CLinkedList<T>&);

    template <typename T>
    class Node {
        /* Singly and circularly linked list node  */
        public:
            Node() {};
            Node(const T& e): val(e), next(nullptr) {};
            friend class LinkedList<T>;
            friend ostream& operator<< <>(ostream&, const LinkedList<T>&);
            friend class CLinkedList<T>;
            friend ostream& operator<< <>(ostream&, const CLinkedList<T>&);
        private:
            T val;
            shared_ptr<Node<T>> next;
    };


    /* ------ Singlely Linked List ------ */
    template <typename T>
    class LinkedList {
        /*
        ** DSAC 3.13, 3.14, 3.15. Shaffer DSAA 4.8.
         */
        public:
            bool empty() const {
                return head == nullptr;}
            const T& front() const {
                return head->val;}
            void insert_front(const T& e) {
                shared_ptr<Node<T>> x = make_shared<Node<T>>(e);
                x->next = head;
                head = x;
            }
            void remove_front() {
                if (empty()) return;
                shared_ptr<Node<T>> old_head = head;
                head = head -> next;
                // delete old_head;
            }
            friend ostream& operator<< <>(ostream&, const LinkedList<T>&);
        private:
            shared_ptr<Node<T>> head;
    };

    template <typename T>
    ostream& operator<<(ostream& s, const LinkedList<T>& ll) {
        if (ll.empty())
            return s << "[]";
        s << '[' << (ll.head -> val) << ']';
        shared_ptr<Node<T>> p = ll.head -> next;
        while (p) {
            s << " -> " << '[' << (p -> val) << ']';
            p = p -> next;
        }
        return s;
    }


    /* ------ Circularly Linked List ------ */
    template <typename T>
    class CLinkedList {
        /* Circular linked list
         * DSAC 3.13, 3.14, 3.15. Shaffer DSAA 4.8.
         */
        public:
            bool empty() const {
                return tail == nullptr;
            }
            const T& front() const {
                if (empty())
                    throw Empty("front() of empty CLinkedList.");
                return tail -> next -> val;
            }
            const T& back() const {
                if (empty())
                    throw Empty("back() of empty CLinkedList.");
                return tail -> val;
            }
            void advance() {
                if (empty())
                    throw Empty("advance() of empty CLinkedList.");
                tail = tail -> next;
            }
            void insert(const T& e) {
                auto u = make_shared<Node<T>>(e);
                if (empty()) {
                    u -> next = u;
                    tail = u;
                }
                else {
                    u -> next = tail -> next;
                    tail -> next = u;
                }
            }
            void remove() {
                if (empty())
                    throw Empty("Remove from empty circularly linked list");
                auto head = tail -> next;
                if (head == tail)
                    tail = nullptr;
                else
                    tail -> next = head -> next;
            }
            friend ostream& operator<< <>(ostream&, const CLinkedList<T>&);
        private:
            shared_ptr<Node<T>> tail;
    };

    template <typename T>
    ostream& operator<<(ostream& os, const CLinkedList<T>& cl) {
        if (cl.empty())
            return os << "[]";
        auto head = cl.tail -> next;
        os << '[' << (head -> val) << ']';
        shared_ptr<Node<T>> p = head -> next;
        while (p != head) {
            os << " -> " << '[' << (p -> val) << ']';
            p = p -> next;
        }
        return os;
    }


    /* ------ Doubly Linked List ------ */
    template <typename T> class DLinkedList;
    template <typename T> class DNodeIterator;
    template <typename T> ostream& operator<<(ostream&, const DLinkedList<T>&);

    
    template <typename T>
    class DNode {
        /*
        ** Doubly linked list node.
         */
        public:
            friend class DLinkedList<T>;
            friend ostream& operator<< <>(ostream&, const DLinkedList<T>&);
            friend class DNodeIterator<T>;

        private:
            T val;
            shared_ptr<DNode<T>> prev;
            shared_ptr<DNode<T>> next;
    };


    template <typename T>
    class DNodeIterator {
        public:
            DNodeIterator(shared_ptr<DNode<T>> u) {
                v = u;
            }
            T& operator*() {
                return v -> val;
            }
            bool operator==(const DNodeIterator<T>& p) const {
                return v == p.v;
            }
            bool operator!=(const DNodeIterator<T>& p) const {
                return v != p.v;
            }
            DNodeIterator<T>& operator++() {
                v = v -> next;
                return *this;
            }
            DNodeIterator<T>& operator--() {
                v = v -> prev;
                return *this;
            }
            shared_ptr<DNode<T>> operator->() const {
                return v;
            }

            friend class DLinkedList<T>;

        private:
            shared_ptr<DNode<T>> v;
    };

    
    template <typename T>
    class DLinkedList {
        /*
        ** DSAC code fragment 3.22-3.27.
        ** Use dummy sentinel nodes for head and tail.
         */
        public:
            DLinkedList()
                : head{make_shared<DNode<T>>()}, tail{make_shared<DNode<T>>()}, m_size{0} {
                head -> next = tail;
                tail -> prev = head;
            }

            ~DLinkedList() {}

            size_t size() const {
                return m_size;
            }
        
            bool empty() const {
                return head -> next == tail;
            }

            DNodeIterator<T> begin() const {
                return DNodeIterator<T>(head -> next);
            }

            DNodeIterator<T> end() const {
                return DNodeIterator<T>(tail);
            }

            const T& front() const {
                if (empty())
                    throw Empty("front() of empty DLinkedList");
                return head -> next -> val;
            };

            const T& back() const {
                if (empty())
                    throw Empty("back() of empty DLinkedList");
                return tail -> prev -> val;
            };

            void insert(const DNodeIterator<T>& p, const T& e) {
                auto u = p -> prev;
                auto v = make_shared<DNode<T>>();
                v -> val = e;
                v -> next = p.v;
                v -> prev = u;
                u -> next = p -> prev = v;
                m_size++;
            }

            void erase(const DNodeIterator<T>& p) {
                auto u = p -> prev;
                auto w = p -> next;
                u -> next = w;
                w -> prev = u;
                m_size--;
            }

            void push_front(const T& e) {
                insert(begin(), e);
            }

            void push_back(const T& e) {
                insert(end(), e);
            }

            void pop_front() {
                if (empty())
                    throw Empty("pop_front() called on empty DLinkedList");
                erase(begin());
            }

            void pop_back() {
                if (empty())
                    throw Empty("pop_back() called on empty DLinkedList");
                erase(--end());
            }

            friend ostream& operator<< <>(ostream&, const DLinkedList<T>&);

        private:
            shared_ptr<DNode<T>> head;
            shared_ptr<DNode<T>> tail;
            size_t m_size;
    };


    template <typename T>
    ostream& operator<<(ostream& os, const DLinkedList<T>& dl) {
        if (dl.empty())
            return os << "[]";
        auto p = dl.begin();
        os << '[' << p -> val << ']';
        for (++p; p != dl.end(); ++p) {
            os << " <-> " << '[' << p -> val << ']';
        }
        return os;
    }
    

}  // end of namespace ADT
    
#endif // __LIST_H_
