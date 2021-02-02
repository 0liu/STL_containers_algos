/*
C++17 implementations of classic tree abstract data structures:
Binary tree, binary search tree, AVL tree.

In practice use cases of trees are mostly covered by maps and sets.
STL has no tree implementations.

References for trees ADT using shared_ptr:
https://thispointer.com/shared_ptr-binary-trees-and-the-problem-of-cyclic-references/
https://codereview.stackexchange.com/questions/191211/binary-search-tree-data-structure-implementation-in-c11-using-smart-pointers
https://github.com/xorz57/forest/blob/master/include/forest/BinarySearchTree.hpp
https://www.internalpointers.com/post/move-smart-pointers-and-out-functions-modern-c

References for dependent or independent names in templates:
https://isocpp.org/wiki/faq/templates#nondependent-name-lookup-types
https://isocpp.org/wiki/faq/templates#nondependent-name-lookup-members
*/


#include <exception>
#include <memory>
#include <vector>
#include <functional>
#include <stack>
#include <queue>
#include <algorithm>
#include <optional>

using namespace std;


class EmptyTreeError : public runtime_error {
public:
    EmptyTreeError(const string& msg) : runtime_error {msg} {}
};

class ValueError : public runtime_error {
public:
    ValueError(const string& msg) : runtime_error {msg} {}
};


template <typename T> class BinarySearchTree;
template <typename T> class AVLTree;

template <typename T>
class BinaryTree {

// Nested Node class. Composition design to avoid template Node class
protected:
    struct Node {
    public:
        Node() {}
        Node(const T& x) : m_key{x} {}
        Node(const Node &) = delete;
        Node(Node &&) = delete;
        Node& operator=(const Node &) = delete;
        Node& operator=(Node &&) = delete;

        bool is_root() {
            return !m_parent.lock();
        }

        bool is_leaf() {
            return !m_left && !m_right;
        }

    private:
        T m_key;
        shared_ptr<Node> m_left {nullptr};  // in-class member initialization
        shared_ptr<Node> m_right {nullptr};
        weak_ptr<Node> m_parent {shared_ptr<Node>(nullptr)};
        friend class BinaryTree;
        friend class BinarySearchTree<T>;
        friend class AVLTree<T>;
    };

// Constructors
public:
    BinaryTree(): m_root{nullptr} {}
    BinaryTree(const vector<optional<T>>& tree_vec) {
        // Build the binary tree from a BFS vector representation, where NIL node is nullopt.
        if (tree_vec.empty() || tree_vec[0] == nullopt) {
            m_root = nullptr;
            return;
        }
        else {
            m_root = make_shared<Node>(tree_vec[0].value());
        }

        queue<shared_ptr<Node>> child_queue;
        child_queue.push(m_root);

        shared_ptr<Node> node;
        for (auto i=1; i<tree_vec.size(); i++) {
            if (i % 2 == 1) {
                node = child_queue.front();
                child_queue.pop();
                if (node) {
                    if (tree_vec[i] == nullopt) {
                        node -> m_left = nullptr;
                    } else {
                        node -> m_left = make_shared<Node>(tree_vec[i].value());
                        node -> m_left -> m_parent = node;
                    }
                }
                child_queue.push(node -> m_left);
            } else {
                if (node) {
                    if (tree_vec[i] == nullopt) {
                        node -> m_right = nullptr;
                    } else {
                        node -> m_right = make_shared<Node>(tree_vec[i].value());
                        node -> m_right -> m_parent = node;
                    }
                }
                child_queue.push(node -> m_right);
            }
        }
    }

// Member data
protected:
    shared_ptr<Node> m_root;

// properties: empty, root, size and height
public:
    bool empty() const {return m_root == nullptr;}
    const T& root() const { return m_root -> m_key; }
    auto size() const { return size(m_root); }
    auto height() const { return height(m_root); }
protected:
    size_t size(const shared_ptr<Node> node) const {
        // return size of a branch under node
        if (!node)
            return 0;
        return size(node -> m_left) + size(node -> m_right) + 1;
    }

    int height(const shared_ptr<Node> node) const {
        // Return the height of the subtree rooted at node.
        // Define the height of a NIL node is -1 for easy rebalance.
        // Ref: https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-006-introduction-to-algorithms-fall-2011/readings/binary-search-trees/avl.py  # noqa
        if (!node)
            return -1;
        return max(height(node -> m_left), height(node -> m_right)) + 1;
    }

// tree traversal algos
public:
    using Callback = function<void(const T&)>;  // function type passed in to process node data while traversing tree
    // Recursive tree traversals
    void pre_order_traversal(Callback cb) const { pre_order(m_root, cb); }
    void in_order_traversal(Callback cb) const { in_order(m_root, cb); }
    void post_order_traversal(Callback cb) const { post_order(m_root, cb); }
    // Iterative tree traversals
    void pre_order_iter_traversal(Callback cb) const { pre_order_iter(m_root, cb); }
    void in_order_iter_traversal(Callback cb) const { in_order_iter(m_root, cb); }
    void post_order_iter_traversal_twostacks(Callback cb) const { post_order_iter_twostacks(m_root, cb); }
    void post_order_iter_traversal_onestack(Callback cb) const { post_order_iter_onestack(m_root, cb); }
    void breadth_first_traversal(Callback cb) const { bfs(m_root, cb); }
private:
    void pre_order(const shared_ptr<Node>, Callback) const;
    void in_order(const shared_ptr<Node>, Callback) const;
    void post_order(const shared_ptr<Node>, Callback) const;
    void pre_order_iter(const shared_ptr<Node>, Callback) const;
    void in_order_iter(const shared_ptr<Node>, Callback) const;
    void post_order_iter_twostacks(const shared_ptr<Node>, Callback) const;
    void post_order_iter_onestack(const shared_ptr<Node>, Callback) const;
    void bfs(const shared_ptr<Node>, Callback) const;
};


template <typename T>
class BinarySearchTree : public BinaryTree<T> {
    using Node = typename BinaryTree<T>::Node;
public:
    BinarySearchTree() : BinaryTree<T>() {}
    BinarySearchTree(const vector<T>& tree_vec) : BinaryTree<T>() {
        for (auto x : tree_vec) {
            iter_insert(x);
        }
    }

public:
    int find(const T& x) {
        // Return index of x in BST if it exists, else -1.
        find_target_key = x;
        find_count = -1;
        is_found = false;
        this -> in_order_traversal(bind(in_order_find, placeholders::_1, this));
        if (!is_found) find_count = -1;
        return find_count;
    }
    bool search(const T& x) const {return search(this -> m_root, x) != nullptr;}
    const T& minimum() const {
        if (!this -> m_root) throw(EmptyTreeError("Empty BinarySearchTree."));
        return minimum(this -> m_root) -> m_key;}
    const T& maximum() const {
        if (!this -> m_root) throw(EmptyTreeError("Empty BinarySearchTree."));
        return maximum(this -> m_root) -> m_key;}
    const T& successor(const T& x) const {
        if (!this -> m_root) throw(EmptyTreeError("Empty BinarySearchTree."));
        auto x_node = search(this -> m_root, x);
        if (!x_node) throw ValueError("Input value not in BinarySearchTree.");
        auto node = successor(x_node);
        if (!node) throw ValueError("Successor does not exist.");
        return node -> m_key;
    }
    const T& predecessor(const T& x) const {
        if (!this -> m_root) throw(EmptyTreeError("Empty BinarySearchTree."));
        auto x_node = search(this -> m_root, x);
        if (!x_node) throw ValueError("Input value not in BinarySearchTree.");
        auto node = predecessor(x_node);
        if (!node) throw ValueError("predecessor does not exist.");
        return node -> m_key;
    }
    void iter_insert(const T& x) {
        auto new_node = _iter_insert(x);
    }
    void insert(const T& x) {
        this -> m_root = _rec_insert(this -> m_root, x); }  // recursive insert
    void remove(const T& x) {
        if (!this -> m_root) throw(EmptyTreeError("Empty BinarySearchTree."));
        auto x_node = search(this -> m_root, x);
        if (!x_node) throw ValueError("Input value not in BinarySearchTree.");
        remove(x_node);
    }

protected:
    T find_target_key; // target key for in_order_find()
    int find_count;  // temp counter for in_order_find()
    bool is_found;  // temp flag for in_order_find()
    static void in_order_find(const T& e, void * this_pointer) {
        auto self = static_cast<BinarySearchTree<T>*>(this_pointer);
        if (self -> is_found) return;
        ++ self -> find_count;
        if (e == self -> find_target_key)
            self -> is_found = true;
    }
    shared_ptr<Node> search(const shared_ptr<Node>, const T&) const;
    shared_ptr<Node> iter_search(const shared_ptr<Node>, const T&) const;
    shared_ptr<Node> minimum(const shared_ptr<Node>) const;
    shared_ptr<Node> maximum(const shared_ptr<Node>) const;
    shared_ptr<Node> successor(const shared_ptr<Node>) const;
    shared_ptr<Node> predecessor(const shared_ptr<Node>) const;
    shared_ptr<Node> _iter_insert(const T&);  // recursive insert
    shared_ptr<Node> _rec_insert(const shared_ptr<Node>, const T&);  // recursive insert
    void transplant(const shared_ptr<Node>, shared_ptr<Node>);
    void remove(const shared_ptr<Node>);
};


template <typename T>
class AVLTree : public BinarySearchTree<T> {
    /*
    AVL balanced binary search tree.
    Reference: MIT OCW 6.006 2011 Lecture 6.
    Video: https://www.youtube.com/watch?v=FNeL18KsWPc
    Re-balance algorithm: 29:50 - 48:33
    AVL Sort vs Heap Sort: 48:33 - End
    Code: https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-006-introduction-to-algorithms-fall-2011/readings/binary-search-trees/  # noqa
    */
using Node = typename BinaryTree<T>::Node;
public:
    AVLTree() : BinarySearchTree<T>() {}
    AVLTree(const vector<T>& tree_vec) : BinarySearchTree<T>() {
        for (auto e : tree_vec)
            insert(e);
    }
    void insert(const T& e) {
        auto x = BinarySearchTree<T>::_iter_insert(e);
        rebalance(x);
    }
private:
    void left_rotate(shared_ptr<Node>);
    void right_rotate(shared_ptr<Node>);
    void rebalance(shared_ptr<Node>);
};


/* ------ Template Class Member Function Definitions: Core Algorithms ------ */

template <typename T>
void BinaryTree<T>::pre_order(const shared_ptr<Node> node, Callback cb) const {
    if (!node) return;
    cb(node -> m_key);
    pre_order(node -> m_left, cb);
    pre_order(node -> m_right, cb);
}

template <typename T>
void BinaryTree<T>::in_order(const shared_ptr<Node> node, Callback cb) const {
    if (!node) return;
    in_order(node -> m_left, cb);
    cb(node -> m_key);
    in_order(node -> m_right, cb);
}

template <typename T>
void BinaryTree<T>::post_order(const shared_ptr<Node> node, Callback cb) const {
    if (!node) return;
    post_order(node -> m_left, cb);
    post_order(node -> m_right, cb);
    cb(node -> m_key);
}

template <typename T>
void BinaryTree<T>::pre_order_iter(const shared_ptr<Node> node, Callback cb) const {
    // Iterative preorder traversal using a stack.
    // http://www.geeksforgeeks.org/iterative-preorder-traversal/
    // 1) Create an empty stack and push root node to stack.
    // 2) Do following while nodeStack is not empty.
    // ….a) Pop an item from stack and print it.
    // ….b) Push right child of popped item to stack
    // ….c) Push left child of popped item to stack
    if (!node) return;
    stack<shared_ptr<Node>> stk;
    stk.push(node);
    shared_ptr<Node> nd;
    while (!stk.empty()) {
        nd = stk.top();
        stk.pop();
        cb(nd -> m_key);
        if (nd -> m_right) stk.push(nd -> m_right);
        if (nd -> m_left) stk.push(nd -> m_left);
    }
}

template <typename T>
void BinaryTree<T>::in_order_iter(const shared_ptr<Node> node, Callback cb) const {
    if (!node) return;
    stack<shared_ptr<Node>> stk;
    shared_ptr<Node> nd {node};
    while (!stk.empty() || nd) {
        if (nd) {
            stk.push(nd);
            nd = nd -> m_left;
        }
        else {  // non-empty stack
            nd = stk.top();
            stk.pop();
            cb(nd -> m_key);
            nd = nd -> m_right;
        }
    }
}

template <typename T>
void BinaryTree<T>::post_order_iter_twostacks(const shared_ptr<Node> node, Callback cb) const {
    // Iterative postorder traversal using two stacks.
    // http://www.geeksforgeeks.org/iterative-postorder-traversal/
    // 1. Push root to first stack.
    // 2. Loop while first stack is not empty
    //    2.1 Pop a node from first stack and push it to second stack
    //    2.2 Push left and right children of the popped node to first stack
    // 3. Print contents of second stack
    if (!node) return;
    stack<shared_ptr<Node>> stk1;
    stack<shared_ptr<Node>> stk2;
    stk1.push(node);
    shared_ptr<Node> nd;
    while (!stk1.empty()) {
        nd = stk1.top();
        stk1.pop();
        stk2.push(nd);
        if (nd -> m_left) stk1.push(nd -> m_left);
        if (nd -> m_right) stk1.push(nd -> m_right);
    }
    while (!stk2.empty()) {
        cb(stk2.top() -> m_key);
        stk2.pop();
    }
}

template <typename T>
void BinaryTree<T>::post_order_iter_onestack(const shared_ptr<Node> node, Callback cb) const {
    if (!node) return;
    stack<shared_ptr<Node>> stk;
    shared_ptr<Node> nd {node};
    shared_ptr<Node> last {nullptr};  // last visited node
    while (!stk.empty() || nd) {
        if (nd) {
            stk.push(nd);
            nd = nd -> m_left;
        }
        else {  // reached current level's leftmost node's left NIL child
            nd = stk.top();  // go back to parent
            if (!nd -> m_right || last == nd -> m_right) {  // no right child, or has visited right child last time
                cb(nd -> m_key);
                stk.pop();
                last = nd;
                nd = nullptr;
            }
            else {  // visit right child
                nd = nd -> m_right;
            }
        }
    }
}

template <typename T>
void BinaryTree<T>::bfs(const shared_ptr<Node> node, Callback cb) const {
    if (!node) return;
    queue<shared_ptr<Node>> child_queue;
    child_queue.push(node);
    shared_ptr<Node> nd;
    while (!child_queue.empty()) {
        nd = child_queue.front();
        cb(nd -> m_key);
        child_queue.pop();
        if (nd -> m_left) child_queue.push(nd -> m_left);
        if (nd -> m_right) child_queue.push(nd -> m_right);
    }
}

template <typename T>
shared_ptr<typename BinaryTree<T>::Node> BinarySearchTree<T>::search(const shared_ptr<Node> node, const T& x) const {
    if (!node || x == node -> m_key)
        return node;
    if (x < node -> m_key)
        return search(node -> m_left, x);
    else
        return search(node -> m_right, x);
}

template <typename T>
shared_ptr<typename BinaryTree<T>::Node> BinarySearchTree<T>::iter_search(const shared_ptr<Node> node, const T& x) const {
    shared_ptr<Node> nd {node};
    while (nd and x != nd -> m_key)
        if (x < nd -> m_key)
            nd = nd -> m_left;
        else
            nd = nd -> m_right;
    return nd;
}

template <typename T>
shared_ptr<typename BinaryTree<T>::Node> BinarySearchTree<T>::minimum(const shared_ptr<Node> node) const {
    auto nd {node};
    while (nd && nd -> m_left)
        nd = nd -> m_left;
    return nd;
}

template <typename T>
shared_ptr<typename BinaryTree<T>::Node> BinarySearchTree<T>::maximum(const shared_ptr<Node> node) const {
    auto nd {node};
    while (nd && nd -> m_right)
        nd = nd -> m_right;
    return nd;
}

template <typename T>
shared_ptr<typename BinaryTree<T>::Node> BinarySearchTree<T>::successor(const shared_ptr<Node> node) const {
    if (!node) return nullptr;
    if (node -> m_right)
        return minimum(node -> m_right);
    auto nd {node};
    auto par {node -> m_parent.lock()};
    // If current node is its parent's right child, keep going up;
    // If current node is its parent's left child, parent is its successor.
    while (par && nd == par -> m_right) {
        nd = par;
        par = par -> m_parent.lock();
    }
    return par;
}

template <typename T>
shared_ptr<typename BinaryTree<T>::Node> BinarySearchTree<T>::predecessor(const shared_ptr<Node> node) const {
    if (!node) return nullptr;
    if (node -> m_left)
        return maximum(node -> m_left);
    auto nd {node};
    auto par {node -> m_parent.lock()};
    // If current node is its parent's right child, keep going up;
    // If current node is its parent's left child, parent is its successor.
    while (par && nd == par -> m_left) {
        nd = par;
        par = par -> m_parent.lock();
    }
    return par;
}

template <typename T>
shared_ptr<typename BinaryTree<T>::Node> BinarySearchTree<T>::_iter_insert(const T& x) {
    // CLRS 12.3 TREE-INSERT(T, z)
    auto new_nd = make_shared<Node>(x);
    shared_ptr<Node> par_nd {nullptr};  // parent of current node x
    auto cur_nd = this -> m_root;
    while (cur_nd) {  // search correct position for new node and its parent
        par_nd = cur_nd;
        if (x < cur_nd -> m_key)
            cur_nd = cur_nd -> m_left;
        else
            cur_nd = cur_nd -> m_right;
    }
    new_nd -> m_parent = par_nd;
    if (!par_nd)
        this -> m_root = new_nd;  // BST was empty
    else if (x < par_nd -> m_key)
        par_nd -> m_left = new_nd;
    else
        par_nd -> m_right = new_nd;
    return new_nd;
}

template <typename T>
shared_ptr<typename BinaryTree<T>::Node> BinarySearchTree<T>::_rec_insert(shared_ptr<Node> node, const T& x) {
    if (!node)
        return make_shared<Node>(x);
    else if (x < node -> m_key) {
        node -> m_left = _rec_insert(node -> m_left, x);
        node -> m_left -> m_parent = node;
    }
    else {
        node -> m_right = _rec_insert(node -> m_right, x);
        node -> m_right -> m_parent = node;
    }
    return node;
}

template <typename T>
void BinarySearchTree<T>::transplant(const shared_ptr<Node> u, shared_ptr<Node> v) {
    if (this -> m_root == u) {
        this -> m_root = v;
        v -> m_parent = u -> m_parent;
        return;
    }
    auto par = u -> m_parent.lock();
    if (u == par -> m_left)
        par -> m_left = v;
    else
        par -> m_right = v;
    if (v != nullptr)
        v -> m_parent = weak_ptr<Node>(par);
}

template <typename T>
void BinarySearchTree<T>::remove(const shared_ptr<Node> node) {
    // CLRS 12.3 TREE-DELETE(T, z)
    if (!node -> m_left)
        transplant(node, node -> m_right);
    else if (!node -> m_right)
        transplant(node, node -> m_left);
    else {
        auto suc = successor(node);
        if (node -> m_right != suc) {  // case D: delete successor, and move it to top of node's right branch
            transplant(suc, suc -> m_right);
            suc -> m_right = node -> m_right;
            suc -> m_right -> m_parent = weak_ptr<Node>(suc);
        }
        // case C & D: Transplant node's right branch to node position
        transplant(node, suc);
        // Connect node's left branch to the new node, the old node's successor
        suc -> m_left = node -> m_left;
        suc -> m_left -> m_parent = weak_ptr<Node>(suc);
    }
}

template <typename T>
void AVLTree<T>::left_rotate(shared_ptr<Node> x) {
    /*
    CLRS 13.2 LEFT-ROTATE(T, x)
    */
    auto y = x -> m_right;
    if (!y) return;
    // Disconnect x with y, and point x's right to y's left, beta.
    x -> m_right = y -> m_left;
    if (y -> m_left) y -> m_left -> m_parent = weak_ptr<Node>(x);
    // Connect y to x's parent
    y -> m_parent = x -> m_parent;
    if (!x -> m_parent.lock()) {
        this -> m_root = y;
    }
    else {
        if (x == x -> m_parent.lock() -> m_left)
            x -> m_parent.lock() -> m_left = y;
        else
            x -> m_parent.lock() -> m_right = y;
    }
    // Reconnect y to x
    y -> m_left = x;
    x -> m_parent = weak_ptr<Node>(y);
}

template <typename T>
void AVLTree<T>::right_rotate(shared_ptr<Node> x) {
    /*
    RIGHT-ROTATE(T, x) mirrors LEFT-ROTATE.
    */
    auto y = x -> m_left;
    if (!y) return;
    // Disconnect x with y, and point x's right to y's left, beta.
    x -> m_left = y -> m_right;
    if (y -> m_right) y -> m_right -> m_parent = weak_ptr<Node>(x);
    // Connect y to x's parent
    y -> m_parent = x -> m_parent;
    if (!x -> m_parent.lock()) {
        this -> m_root = y;
    }
    else {
        if (x == x -> m_parent.lock() -> m_left)
            x -> m_parent.lock() -> m_left = y;
        else
            x -> m_parent.lock() -> m_right = y;
    }
    // Reconnect y to x
    y -> m_right = x;
    x -> m_parent = weak_ptr<Node>(y);
}

template <typename T>
void AVLTree<T>::rebalance(shared_ptr<Node> x) {
    /*
    Check children height balance from node x up to root. If unbalance is
    found, rebalance and then keep checking until reaching root's parent.
    */
    while (x) {  // loop until x is root's parent
        if (this -> height(x -> m_left) > this -> height(x -> m_right) + 1) {  // left heavy
            if (this -> height(x -> m_left -> m_left) >= this -> height(x -> m_left -> m_right)) {  // straight-line case
                this -> right_rotate(x);
            }
            else {  // zig-zag case. Double rotations.
                this -> left_rotate(x -> m_left);
                this -> right_rotate(x);
            }
        } else if (this -> height(x -> m_right) > this -> height(x -> m_left) + 1) {  // right heavy
            if (this -> height(x -> m_right -> m_right) >= this -> height(x -> m_right -> m_left)) {
                this -> left_rotate(x);
            }
            else {
                this -> right_rotate(x -> m_right);
                this -> left_rotate(x);
            }
        } else {
            // x is a balanced node. No need to do anything.
        }
        // Children height balance achieved at x. Now move up.
        x = x -> m_parent.lock();
    }
}
