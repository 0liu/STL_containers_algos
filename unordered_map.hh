#ifndef __UNORDERED_MAP_H_
#define __UNORDERED_MAP_H_

#include <utility>
#include <iterator>
#include "vector.hh"
#include "hash.hh"

namespace ADT {


template <typename K, typename V>
class unordered_map {
    // TODO: Need const_iterator

    public:
        using T = std::pair<K, V>;
        class iterator;

        unordered_map() { init_buckets(m_init_bucket_num); }
        virtual ~unordered_map() { clear(); }

        // iterator
        iterator begin() { return iterator(this, true); }
        iterator end() { return iterator(this, false); }

        // Capacity
        bool empty() const noexcept { return m_size == 0; }
        size_t size() const noexcept { return m_size; }
        size_t bucket_count() const noexcept { return m_bcnt; } 

        // Element access
        V& operator[](const K& key);
        
        // Element lookup
        iterator find(const K& key);
         
        // Modifiers
        void insert(const T& p) { (*this)[p.first] = p.second; }
        size_t erase(const K& key);
        void clear();
        
    private:
        struct Node {
            T kv;
            Node* next;
        };

        static constexpr int m_init_bucket_num = 100;
        static constexpr double m_max_load = .8;  // 80%
        int m_bcnt;  // number of buckets
        int m_size;
        vector<Node*> m_buckets;

        // supporting methods
        void init_buckets(int k);
        Node* insert_new_node(const K& key);
        void reserve(int n);
        Node* find_node(int bucket_idx, const K& key, Node*& prv);

    public:  // iterator
        class iterator : public std::iterator<std::forward_iterator_tag, K> {
            public:
                iterator() = default;
                iterator(unordered_map<K, V> *ump, int bucket_idx, Node* node)
                    : m_ump{ump}, m_bucket_idx{bucket_idx}, m_node{node} {}
                iterator(unordered_map<K, V> *ump, bool begin) {
                    m_ump = ump;
                    if (begin) {
                        m_bucket_idx = 0;
                        m_node = ump -> m_buckets[m_bucket_idx];
                        while (!m_node && ++m_bucket_idx < ump -> m_bcnt)
                            m_node = ump -> m_buckets[m_bucket_idx];
                    }
                    else {  // end
                        m_bucket_idx = ump -> m_bcnt;
                        m_node = nullptr;
                    }
                }

                iterator(const iterator& it) {
                    m_ump = it.m_ump;
                    m_bucket_idx = it.m_bucket_idx;
                    m_node = it.m_node;
                }

                iterator& operator=(iterator it) {
                    m_ump = it.m_ump;
                    m_bucket_idx = it.m_bucket_idx;
                    m_node = it.m_node;
                }

                iterator & operator++() {
                    m_node = m_node->next;
                    while (!m_node && ++m_bucket_idx < m_ump -> m_bcnt)
                        m_node = m_ump -> m_buckets[m_bucket_idx];
                    return *this;
                }

                iterator operator++(int) {
                    iterator old(*this);
                    operator++();
                    return old;
                }

                bool operator==(const iterator& it) {
                    return m_ump == it.m_ump && m_bucket_idx == it.m_bucket_idx && m_node == it.m_node;
                }

                bool operator!=(const iterator& it) {
                    return !(*this == it);
                }

                T& operator*() {
                    return m_node -> kv;
                }

                T* operator->() {
                    return &(m_node -> kv);
                }

            private:
                unordered_map<K, V> *m_ump;
                int m_bucket_idx;
                Node* m_node;
        };
};


template <typename K, typename V>
void unordered_map<K, V>::init_buckets(int k) {
    k = k < 1 ? 1 : k;
    m_buckets = vector<Node*>(k, nullptr);
    m_bcnt = k;
    m_size = 0;
}

template <typename K, typename V>
typename unordered_map<K, V>::Node* unordered_map<K, V>::insert_new_node(const K& key) {
    int bucket_idx = hash(key) % m_bcnt;
    Node* node = new Node;
    (node -> kv).first = key;
    (node -> kv).second = V();
    node -> next = m_buckets[bucket_idx];
    m_buckets[bucket_idx] = node;
    m_size++;
    return node;
}

template <typename K, typename V>
void unordered_map<K, V>::reserve(int n) {
    vector<Node*> b{m_buckets};
    m_bcnt = n + (n >> 3) + (n < 9 ? 3: 6);
    init_buckets(m_bcnt);
    for (size_t i = 0; i < b.size(); ++i) {
        Node* node = b[i];
        while (node) {
            Node* next = node -> next;
            // re-hash
            const K& key = (node->kv).first;
            int new_idx = hash(key) % m_bcnt;
            node -> next = m_buckets[new_idx];
            m_buckets[new_idx] = node;
            m_size++;
            node = next;
        }
    }
}

template <typename K, typename V>
typename unordered_map<K, V>::Node* unordered_map<K, V>::find_node(int bucket_idx, const K& key, Node*& prv) {
    prv = nullptr;
    Node* node = m_buckets[bucket_idx];
    while (node && (node->kv).first != key) {
        prv = node;
        node = node -> next;    
    }
    return node;
}

template <typename K, typename V>
V& unordered_map<K, V>::operator[](const K& key) {
    int bucket_idx = hash(key) % m_bcnt;
    Node* prv;
    Node* node = find_node(bucket_idx, key, prv);
    if (!node) {  // default initialize a node for key
        if (m_size > m_max_load * m_bcnt)
            reserve(int(m_size / m_max_load));
        node = insert_new_node(key);
    }
    return (node -> kv).second;
}    

template <typename K, typename V>
typename unordered_map<K, V>::iterator unordered_map<K, V>::find(const K& key) {
    Node* prv;
    int bucket_idx = hash(key) % m_bcnt;
    Node* node = find_node(bucket_idx, key, prv);
    if (!node)
        return end();
    else
        return {this, bucket_idx, node};
}

template <typename K, typename V>
size_t unordered_map<K, V>::erase(const K& key) {
    int bucket_idx = hash(key) % m_bcnt;
    Node* prv;
    Node* node = find_node(bucket_idx, key, prv);
    if (!node)
        return 0;

    if (!prv)  // key is the first node in the bucket
        m_buckets[bucket_idx] = node -> next;
    else
        prv -> next = node -> next;
    delete node;
    m_size--;
    return 1;
}

template <typename K, typename V>
void unordered_map<K, V>::clear() {
    for (size_t i = 0; i < m_bcnt; ++i) {
        Node* node = m_buckets[i];
        while (node) {
            Node* next = node -> next;
            delete node;
            node = next;
        }
        m_buckets[i] = nullptr;
    }
    m_size = 0;
}


}  // end of namespace ADT


#endif // __UNORDERED_MAP_H_
