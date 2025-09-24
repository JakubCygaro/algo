#ifndef DATATYPES_HPP
#define DATATYPES_HPP
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iterator>
#include <optional>
#include <print>
#include <tuple>
#include <utility>
#include <vector>

namespace dt {
    namespace {
        template <typename T>
        inline bool a_less_b_fn(const T& a, const T& b){
            return a < b;
        }
        template <typename T>
        inline bool a_more_b_fn(const T& a, const T& b){
            return !a_less_b_fn<T>(a, b);
        }
    }
    template <typename Key, typename T, bool(*CompareFunc)(const Key&, const Key&)>
    class Heap {
    private:
        struct Cell {
            Key key;
            T value;
            Cell(){};
            Cell(Key key, T value) : key(key), value(value){};
            Cell(std::tuple<Key, T> tup) : key(std::get<0>(tup)), value(std::get<1>(tup)){}
        };
        Cell* m_data;
        std::size_t m_cap{};
        std::size_t m_size{};

        inline static constexpr unsigned int GROW_FACTOR = 2;
        inline static constexpr unsigned int SHRINK_FACTOR = 2;

    public:
    Heap() : m_cap(1), m_size(0) {
        m_data = new Cell[m_cap];
    }
    explicit Heap(std::size_t capacity) : m_cap(capacity), m_size(0) {
        m_data = new Cell[m_cap];
    }
    Heap(const Heap<Key, T, CompareFunc>& other) : m_cap(other.m_cap), m_size(other.m_size) {
        m_data = new Cell[m_cap];
        std::memcpy(m_data, other.m_data, m_cap * sizeof(Cell));
    }
    Heap& operator=(const Heap<Key, T, CompareFunc>& other) {
        m_cap = other.m_cap;
        m_size = other.m_size;
        m_data = new Cell[m_cap];
        std::memcpy(m_data, other.m_data, m_cap * sizeof(Cell));
        return *this;
    }
    Heap(Heap<Key, T, CompareFunc>&& other) : m_cap(other.m_cap), m_size(other.m_size) {
        m_data = other.m_data;
        other.m_size = 0;
        other.m_cap = 0;
        other.m_data = nullptr;
    }
    Heap& operator=(Heap<Key, T, CompareFunc>&& other) {
        m_cap = other.m_cap;
        m_size = other.m_size;
        m_data = other.m_data;
        other.m_size = 0;
        other.m_cap = 0;
        other.m_data = nullptr;
        return *this;
    }
    ~Heap() {
        if(m_data){
            delete[] m_data;
        }
    }
    private:
        inline void grow() {
            auto old_cap = m_cap;
            m_cap = m_cap * GROW_FACTOR;
            auto tmp = new Cell[m_cap];
            std::memcpy(tmp, m_data, old_cap * sizeof(Cell));
            delete[] m_data;
            m_data = tmp;
        }
        inline void shrink() {
            m_cap = std::clamp(m_cap / SHRINK_FACTOR, m_size, m_cap);
            auto tmp = new Cell[m_cap];
            std::memcpy(tmp, m_data, m_cap * sizeof(Cell));
            delete[] m_data;
            m_data = tmp;
        }
        inline std::optional<std::size_t> parent(std::size_t index) const {
            if (index == 0)
                return std::nullopt;
            if(index % 2 == 0) {
                index--;
            }
            return index / 2;
        }
        inline std::optional<std::size_t> left_child(std::size_t index) const{
            auto ret = index * 2 + 1;
            return ret < m_size ? std::optional<std::size_t>{ret} : std::nullopt;
        }
        inline std::optional<std::size_t>  right_child(std::size_t index) const{
            auto ret = index * 2 + 2;
            return ret < m_size ? std::optional<std::size_t>{ret} : std::nullopt;
        }
    public:
        inline bool empty() const {
            return m_size == 0;
        }
        inline std::size_t size() const {
            return m_size;
        }
        inline void insert(Key k, T elem) {
            if(++m_size >= m_cap)
                grow();
            auto i = m_size - 1;
            m_data[i] = Cell{k, elem};
            while(auto j = shift_up(i)) {
                i = j.value();
            }
        }
        inline std::tuple<Key, T> extract() {
            auto ret = m_data[0];
            std::swap(m_data[0], m_data[--m_size]);
            std::size_t i = 0;
            while(auto j = shift_down(i)) {
                i = j.value();
            }
            return std::make_tuple(ret.key, ret.value);
        }
        inline T& top() const {
            return m_data[0];
        }
        inline bool delete_element(T* elem) {
            std::size_t i = 0;
            auto found = false;
            for(; i < m_size; i++){
                if(&m_data[i].value == elem){
                    found = true;
                    break;
                }
            }
            if(!found) return found;
            std::swap(m_data[i], m_data[--m_size]);

            m_data[m_size] = {{}, {}};

            if(i == m_size) return found;

            while(auto j = shift_up(i)){
                i = j.value();
            }
            while(auto j = shift_down(i)){
                i = j.value();
            }
            if (m_size < (m_cap / 2)) {
                shrink();
            }
#ifdef TEST_INTERNALS
            verify_heap_property();
#endif
            return found;
        }
        inline std::tuple<Key, T*> search(const T& comp) {
            std::size_t i = 0;
            auto found = false;
            for (;i < m_size && !empty(); i++){
                if(m_data[i].value == comp) {
                    found = true;
                    break;
                }
            }
            if(!found) return std::make_tuple<Key, T*>( {}, nullptr );
            return { m_data[i].key, &m_data[i].value };
        }
        /// if a shift up has occured, return the new position of i
        inline std::optional<std::size_t> shift_up(std::size_t i) {
            if(auto p = parent(i); p.has_value() && CompareFunc(m_data[i].key, m_data[p.value()].key)) {
                std::swap(m_data[i], m_data[p.value()]);
                return p.value();
            }
            return std::nullopt;
        }
        /// if a shift down has occured, return the new position of i
        inline std::optional<std::size_t> shift_down(std::size_t i) {
            auto lc = left_child(i);
            auto rc = right_child(i);
            if (lc && rc) {
                auto min_idx = CompareFunc(m_data[lc.value()].key, m_data[rc.value()].key) ? lc.value() : rc.value();
                if (CompareFunc(m_data[min_idx].key, m_data[i].key)) {
                    std::swap(m_data[min_idx], m_data[i]);
                    return min_idx;
                }
            } else if (lc.has_value() && CompareFunc(m_data[lc.value()].key, m_data[i].key)) {
                std::swap(m_data[lc.value()], m_data[i]);
                return lc.value();
            } else if (rc.has_value() && CompareFunc(m_data[rc.value()].key, m_data[i].key)) {
                std::swap(m_data[rc.value()], m_data[i]);
                return rc.value();
            }
            return std::nullopt;
        }
    private:
        inline void heapify_impl(){
            for(auto i = m_size - 1; i != 0; i--){
                if(auto p = parent(i); p.has_value()){
                    while(auto j = shift_down(p.value())) {
                        p = j;
                    }
                }
            }
#ifdef TEST_INTERNALS
            verify_heap_property();
#endif
        }
#ifdef TEST_INTERNALS
        inline void verify_heap_property(){
            for(std::size_t i = 0; i < m_size; i++){
                assert(!(shift_down(i).has_value() || shift_up(i).has_value()) && "heap property broken");
            }
        }
#endif
    public:
        inline static Heap<Key, T, CompareFunc> heapify(const std::vector<std::tuple<Key, T>>& arr){
            Heap<Key, T, CompareFunc> heap(arr.size());
            std::size_t i = 0;
            std::for_each(arr.begin(), arr.end(), [&](auto v){
                heap.m_data[i++] = {v};
            });
            heap.m_size = i;
            heap.heapify_impl();
            return heap;
        }
        inline static Heap<Key, T, CompareFunc> heapify(const std::vector<Key>& arr){
            Heap<Key, T, CompareFunc> heap(arr.size());
            heap.m_size = arr.size();
            std::size_t i = 0;
            std::for_each(arr.begin(), arr.end(), [&](auto v){
                heap.m_data[i++] = {v, T{}};
            });
            heap.heapify_impl();
            return heap;
        }
        inline void debug_print() {
            for(std::size_t i = 0; i < m_size; i++){
            }
        }
        inline void debug_print_full() {
            for(std::size_t i = 0; i < m_cap; i++){
            }
        }
    };
    template <typename Key, typename T>
    using MinHeap = Heap<Key, T, a_less_b_fn<Key>>;
    template <typename Key, typename T>
    using MaxHeap = Heap<Key, T, a_more_b_fn<Key>>;

    template <typename K, typename T>
    class RBTree {
    private:
        enum class ChildDir {
            LEFT,
            RIGHT,
        };
#define WHICH_CHILD(PARENTPTR, NODEPTR) PARENTPTR->left == NODEPTR ? ChildDir::LEFT : ChildDir::RIGHT

        enum class Color{
            RED,
            BLACK
        };
        struct Node {
            K key{};
            T data{};
            std::size_t size{1};
            Color color;
            Node* parent{};
            Node* left{nullptr}; //smaller
            Node* right{nullptr}; //larger
        };
        inline static const Node _NIL_V = Node{
            .right = nullptr,
            .left = nullptr,
            .parent = nullptr,
        };
        inline static const Node* NIL = &_NIL_V;

        Node* m_root{};
        std::size_t m_size{};

    public:
        RBTree(){

        }
        ~RBTree(){
            std::function<void(Node*)> delete_fn = [&](Node* n){
                if(n){
                    delete_fn(n->left);
                    delete_fn(n->right);
                    delete n;
                }
            };
            delete_fn(m_root);
        }
    private:
        Node* search_impl(const K& key) const {
            auto node = m_root;
            while(node){
                if(node->key == key){
                    return node;
                } else if(key > node->key){
                    node = node->right;
                } else {
                    node = node->left;
                }
            }
            return nullptr;
        }
        Node* min_impl(Node* root) const{
            auto node = root;
            while(node){
                if(node->left)
                    node = node->left;
                else
                    break;
            }
            return node;
        }
        Node* max_impl(Node* root) const {
            auto node = root;
            while(node){
                if(node->right)
                    node = node->right;
                else
                    break;
            }
            return node;
        }
        void output_sorted_impl(std::size_t& i, auto& v, Node* n){
            if(n){
                output_sorted_impl(i, v, n->left);
                v[i++] = { n->key, n->data };
                output_sorted_impl(i, v, n->right);
            }
        }
    public:
        bool insert(const K& key, T&& item){
            Node* prev = nullptr;
            Node** node = &m_root;

            while(*node){
                if(key > (*node)->key){
                    prev = *node;
                    node = &((*node)->right);
                } else if(key < (*node)->data){
                    prev = *node;
                    node = &((*node)->left);
                } else {
                    return false;
                }
            }
            *node = new Node;
            (*node)->key = key;
            (*node)->data = item;
            (*node)->parent = prev;
            (*node)->size = 1;
            inc_size(*node);
            insert_fixup(*node);
            m_size++;
            return true;
        }
        std::size_t size() const {
            return m_size;
        }
        bool empty() const {
            return m_size == 0;
        }
        T* search(const K key) const {
            auto found = search_impl(key);
            if(found){
                return &found->data;
            } else {
                return nullptr;
            }
        }
        std::tuple<K, T*> min() const {
            auto node = min_impl(m_root);
            if(node){
                return { node->key, &node->data };
            } else {
                return { {}, {} };
            }
        }
        std::tuple<K, T*> max() const {
            auto node = max_impl(m_root);
            if(node){
                return { node->key, &node->data };
            } else {
                return { {}, {} };
            }
        }
    private:
        Node* predecessor_node(const Node* node) const {
            if(!node) return nullptr;
            auto key = node->key;

            auto left_child = max_impl(node->left);
            if(left_child) return left_child;

            auto parent = node->parent;

            while (parent){
                if(parent->key < key)
                    return parent;
                else
                    parent = parent->parent;
            }
            return nullptr;
        }
        void inc_size(const Node* n){
            Node* parent = n->parent;
            while(parent){
                parent->size++;
                parent = parent->parent;
            }
        }
        void dec_size(const Node* n){
            // if(!n) return;
            Node* parent = n->parent;
            while(parent){
                parent->size--;
                parent = parent->parent;
            }
        }
    public:
        std::tuple<K, T*> predecessor(const K& key) const {
            auto node = search_impl(key);
            if(!node) return {};

            auto left_child = max_impl(node->left);
            if(left_child) return { left_child->key, &left_child->data };

            auto parent = node->parent;

            while (parent){
                if(parent->key < key)
                    return { parent->key, &parent->data };
                else
                    parent = parent->parent;
            }
            return {};
        }
        std::tuple<K, T*> successor(const K& key) const {
            auto node = search_impl(key);
            if(!node) return {};

            auto right_child = min_impl(node->right);
            if(right_child) return { right_child->key, &right_child->data };

            auto parent = node->parent;

            while (parent){
                if(parent->key > key)
                    return { parent->key, &parent->data };
                else
                    parent = parent->parent;
            }
            return {};
        }
    private:
        Node* successor_node(Node* node) const {
            if(!node) return {};

            auto right_child = min_impl(node->right);
            if(right_child) return right_child;

            auto parent = node->parent;

            while (parent){
                if(parent->key > node->key)
                    return parent;
                else
                    parent = parent->parent;
            }
            return nullptr;
        }
    public:
        std::vector<std::tuple<K, T>> output_sorted() {
            std::vector<std::tuple<K, T>> ret(m_size);
            std::size_t i = 0;
            output_sorted_impl(i, ret, m_root);
            return ret;
        }
        bool delete_element(const K& key){
            auto z = search_impl(key);
            if(!z) return false;

            auto y = z;
            auto y_original_color = y->color;
            // fake x in case the real one was null
            Node _sentinel = Node{ .size = 42069, .color = Color::BLACK  };
            Node* x = nullptr;

            if(!z->left && z->right){
                // x = z->right ? z->right : &_sentinel;
                // if(!z->right){
                //     x->parent = z->parent;
                // }
                x = z->right;
                transplant(z, z->right);
            }
            else if (!z->right && z->left){
                // x = z->left ? z->left : &_sentinel;
                // if(!z->left){
                //     x->parent = z->parent;
                // }
                x = z->left;
                transplant(z, z->left);
            } else if (z->left && z->right){
                y = min_impl(z->right);
                y_original_color = y->color;
                x = y->right ? y->right : &_sentinel;
                // in case y has no right child
                if(!y->right){
                    // y->right = x; // make y->right point to the sentinel
                    x->parent = y; // and the sentinel point to y as its parent
                }
                if(y->parent == z){
                    x->parent = y;
                }
                else {
                    transplant(y, y->right); // y->right might be sentinel at this point
                    y->right = z->right;
                    if(y->right){
                        y->right->parent = y;
                        y->right->size = y->size;
                    }
                }
                transplant(z, y);
                y->left = z->left;
                y->size = z->size;
                if(y->left)
                    y->left->parent = y;
                y->color = z->color;
            } else {
                //sentinel takes place of a leaf node
                x = &_sentinel;
                x->parent = z->parent;
                if(z->parent && z->parent->left == z)
                    z->parent->left = nullptr;
                else if(z->parent)
                    z->parent->right = nullptr;
            }
            dec_size(x);
            const bool sentinel = x == &_sentinel;
            if(y_original_color == Color::BLACK){
                delete_fixup(x);
            }
            // clean this up
            if(sentinel){
                if(!x->parent) m_root = nullptr;
                else if(x->parent->right == x) x->parent->right = nullptr;
                else if(x->parent->left == x) x->parent->left = nullptr;
            }


// start:
//             auto y = node;
//             auto original_color = y->color;
//             auto parent = node->parent;
//             // pointer to the pointer of the parent to this node
//             Node** child_from_parent = nullptr;
//             if(parent){
//                 child_from_parent = parent->left == node ? &parent->left : &parent->right;
//             }
//             // is leaf
//             if(!node->left && !node->right){
//                 // make the parent forget me
//                 if(parent){
//                     *child_from_parent = nullptr;
//                 } else if (node == m_root){
//                     m_root = nullptr;
//                 }
//             } // two children case
//             else if(node->left && node->right){
//                 auto n = predecessor_node(node);
//                 K tmp_k = n->key;
//                 T tmp_d = n->data;
//                 n->key = node->key;
//                 n->data = node->data;
//                 node->key = tmp_k;
//                 node->data = tmp_d;
//                 node = n;
//                 goto start; //lets get sloppy
//             } // only has left child
//             else if(node->left){
//                 if(parent){
//                     *child_from_parent = node->left;
//                     (*child_from_parent)->parent = parent;
//                     (*child_from_parent)->color = original_color;
//                 } else {
//                     m_root = node->left;
//                     m_root->parent = nullptr;
//                     m_root->size--;
//                     m_root->color = Color::BLACK;
//                 }
//             } // only has right child
//             else if(node->right){
//                 if(parent){
//                     *child_from_parent = node->right;
//                     (*child_from_parent)->parent = parent;
//                     (*child_from_parent)->color = original_color;
//                 } else {
//                     m_root = node->right;
//                     m_root->parent = nullptr;
//                     m_root->size--;
//                     m_root->color = Color::BLACK;
//                 }
//
//             }
//             else {
//                 return false;
//             }
//             if(original_color == Color::BLACK){
//
//             }
            delete z;
            m_size--;
            return true;
        }
        void delete_fixup(Node* x){
            if(!x) return;

            while(x && x->parent && x != m_root && x->color == Color::BLACK){
                if(x == x->parent->left){
                    auto w = x->parent->right; // sibling
                    if(w && w->color == Color::RED){
                        w->color = Color::BLACK;
                        x->parent->color = Color::RED;
                        rotate_left(x->parent);
                        w = x->parent->right;
                    }
                    if (w && (!w->left || w->left->color == Color::BLACK) && (!w->right || w->right->color == Color::BLACK)){
                        w->color = Color::RED;
                        x = x->parent;
                    }
                    else if (w && (!w->right || w->right->color == Color::BLACK)){
                        if(w->left) w->left->color = Color::BLACK;
                        w->color = Color::RED;
                        rotate_right(w);
                        w = x->parent->right;
                        if(w) w->color = x->parent->color;
                        x->parent->color = Color::BLACK;
                        if(w->right) w->right->color = Color::BLACK;
                        rotate_left(x->parent);
                        x = m_root;
                    } else {
                        break;
                    }
                } else {
                    auto w = x->parent->left;
                    if(w && w->color == Color::RED){
                        w->color = Color::BLACK;
                        x->parent->color = Color::RED;
                        rotate_right(x->parent);
                        w = x->parent->left;
                    }
                    if (w && (!w->right || w->right->color == Color::BLACK) && (!w->left || w->left->color == Color::BLACK)){
                        w->color = Color::RED;
                        x = x->parent;
                    }
                    else if (w && (!w->left || w->left->color == Color::BLACK)){
                        if(w->right) w->right->color = Color::BLACK;
                        w->color = Color::RED;
                        rotate_left(w);
                        w = x->parent->left;
                        if(w) w->color = x->parent->color;
                        x->parent->color = Color::BLACK;
                        if(w->left) w->left->color = Color::BLACK;
                        rotate_right(x->parent);
                        x = m_root;
                    } else {
                        break;
                    }
                }
            }
            x->color = Color::BLACK;
        }
        std::tuple<K, T*> select(std::size_t i){
            if(i++ > m_size) return {};
            auto node = m_root;

            while(node){
                auto j = node->left ? node->left->size : 0;
                if(i == j + 1) return { node->key, &node->data };
                else if(i < j + 1){
                    node = node->left;
                }
                else if(i > j + 1){
                    i = i - j - 1;
                    node = node->right;
                }
            }
            return {};
        }
    private:
        bool rotate_right(Node* node_x){
            if(!node_x) return false;
            if(!node_x->left) return false;

            auto node_y = node_x->left;

            node_y->parent = node_x->parent;

            if(node_y->parent){
                Node** child_ptr = node_y->parent->right == node_x ? &node_y->parent->right : &node_y->parent->left;
                *child_ptr = node_y;
            } else {
                m_root = node_y;
            }

            node_x->left = node_y->right;
            if(node_x->left)
                node_x->left->parent = node_x;

            node_x->parent = node_y;
            node_y->right = node_x;

            rotate_size_fixup(node_x, node_y);
            return true;
        }
        bool rotate_left(Node* node_x){
            if(!node_x) return false;
            if(!node_x->right) return false;

            auto node_y = node_x->right;

            node_y->parent = node_x->parent;

            if(node_y->parent){
                Node** child_ptr = node_y->parent->right == node_x ? &node_y->parent->right : &node_y->parent->left;
                *child_ptr = node_y;
            } else {
                m_root = node_y;
            }
            node_x->right = node_y->left;
            if(node_x->right)
                node_x->right->parent = node_x;

            node_x->parent = node_y;
            node_y->left = node_x;

            rotate_size_fixup(node_x, node_y);
            return true;
        }
        void rotate_size_fixup(Node* node_x, Node* node_y){
            node_x->size = 1;
            if(node_x->right){
                node_x->size += node_x->right->size;
            }
            if(node_x->left){
                node_x->size += node_x->left->size;
            }
            node_y->size = 1;
            if(node_y->right){
                node_y->size += node_y->right->size;
            }
            if(node_y->left){
                node_y->size += node_y->left->size;
            }
        }
        void insert_fixup(Node* z){
            z->color = Color::RED;

            while(z && z->parent && z->parent->color == Color::RED){
                auto grandparent = z->parent->parent;
                if(grandparent && z->parent == grandparent->left){
                    auto uncle = grandparent->right;
                    if(uncle && uncle->color == Color::RED){
                        z->parent->color = Color::BLACK;
                        uncle->color = Color::BLACK;
                        grandparent->color = Color::RED;
                        z = grandparent;
                        continue;
                    }
                    else if(z == z->parent->right){
                        z = z->parent;
                        rotate_left(z);
                    } 
                    z->parent->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    rotate_right(z->parent->parent);
                } else if(grandparent){
                    auto uncle = grandparent->left;
                    if(uncle && uncle->color == Color::RED){
                        z->parent->color = Color::BLACK;
                        uncle->color = Color::BLACK;
                        grandparent->color = Color::RED;
                        z = grandparent;
                        continue;
                    }
                    else if(z == z->parent->left){
                        z = z->parent;
                        rotate_right(z);
                    } 
                    z->parent->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    rotate_left(z->parent->parent);
                }  
            }
            m_root->color = Color::BLACK;


            // I am red and my parent is red
            // while(z && z->parent && z->parent->color == Color::RED){
            //     Node* grandparent = z->parent->parent;
            //     ChildDir parent_dir{};
            //     if(grandparent){
            //         parent_dir = WHICH_CHILD(grandparent, z->parent);
            //     }
            //     Node* uncle = nullptr;
            //     if(grandparent){
            //         uncle = (parent_dir == ChildDir::LEFT) ? grandparent->right : grandparent->left;
            //     }
            //     auto uncle_color = uncle ? uncle->color : Color::BLACK;
            //     auto z_dir = WHICH_CHILD(z->parent, z);
            //
            //     if(uncle_color == Color::RED){
            //         z->parent->color = Color::BLACK;
            //         uncle->color = Color::BLACK;
            //         if(grandparent) grandparent->color = Color::RED;
            //         z = grandparent;
            //         continue;
            //     }
            //     else if(parent_dir == ChildDir::LEFT){
            //         if(z_dir == ChildDir::RIGHT){
            //             z = z->parent;
            //             rotate_left(z);
            //             z_dir = ChildDir::LEFT;
            //         }
            //         if(z->parent) z->parent->color = Color::BLACK;
            //         if(grandparent) grandparent->color = Color::RED;
            //         rotate_right(grandparent);
            //         break;
            //     } else if(parent_dir == ChildDir::RIGHT) {
            //         if(z_dir == ChildDir::LEFT){
            //             z = z->parent;
            //             rotate_right(z);
            //             z_dir = ChildDir::RIGHT;
            //         }
            //         if(z->parent) z->parent->color = Color::BLACK;
            //         if(grandparent) grandparent->color = Color::RED;
            //         rotate_left(grandparent);
            //         break;
            //     }
            // }
            // m_root->color = Color::BLACK;
        }
        void transplant(Node* u, Node* v){
            if(!u->parent){
                m_root = v;
            } else if (u == u->parent->left){
                u->parent->left = v;
            } else {
                u->parent->right = v;
            }
            if(v){
                v->parent = u->parent;
            }
        }
    public:
    };

    template<typename T>
    struct Hash {
        std::size_t operator()(const T& t) const noexcept;
    };

    template<>
    struct Hash<std::size_t> {
        inline std::size_t operator()(const std::size_t& t) const noexcept {
            return t;
        }
    };

    template<typename K, typename T, typename H = Hash<K>>
    class HashMap{
        static inline constexpr auto GROW_FACTOR = 2;
        static inline constexpr auto FILL_RATIO_GROW_THRESHOLD = 0.6;
        static inline constexpr auto FILL_RATIO_SHRINK_THRESHOLD = 0.2;
        struct Item {
            K key{};
            T value{};
        };

        using item_t = std::optional<Item>;

        item_t* m_data{};

        std::size_t m_size{};
        std::size_t m_cap{1};
        public:

        HashMap(){
            m_data = new item_t[m_cap];
            m_data[0] = std::nullopt;
        }
        HashMap(const HashMap& other):
            m_size(other.m_size)
            , m_cap(other.m_cap)
        {
            m_data = new item_t[m_cap];
            std::copy(std::begin(other.m_data), std::begin(other.m_data) + m_cap, std::begin(m_data));
        }
        HashMap& operator=(const HashMap& other)
        {
            m_size = other.m_size;
            m_cap = other.m_cap;
            m_data = new item_t[m_cap];
            std::copy(std::begin(other.m_data), std::begin(other.m_data) + m_cap, std::begin(m_data));
        }
        HashMap(HashMap&& other):
            m_size(other.m_size)
            , m_cap(other.m_cap)
        {
            m_data = other.m_data;
            other.m_data = nullptr;
            other.m_cap = 0;
            other.m_size = 0;
        }
        HashMap& operator=(HashMap&& other)
        {
            m_size = other.m_size;
            m_cap = other.m_cap;
            m_data = other.m_data;
            other.m_data = nullptr;
            other.m_cap = 0;
            other.m_size = 0;
        }
        ~HashMap(){
            if(m_data)
                delete[] m_data;
        }
    private:
        void grow(){
            auto new_cap = m_cap * GROW_FACTOR;
            auto new_data = new item_t[new_cap];
            std::fill(new_data, new_data + new_cap, std::nullopt);
            auto old_data = m_data;
            auto old_cap = m_cap;
            m_data = new_data;
            m_cap = new_cap;
            reinsert(old_cap, old_data);

            delete[] old_data;
        }
        void shrink(){
            auto new_cap = m_cap / GROW_FACTOR;
            auto new_data = new item_t[new_cap];
            std::fill(new_data, new_data + new_cap, std::nullopt);
            auto old_data = m_data;
            auto old_cap = m_cap;
            m_data = new_data;
            m_cap = new_cap;
            reinsert(old_cap, old_data);

            delete[] old_data;
        }
        void reinsert(std::size_t cap, item_t* data){
            for(size_t i = 0; i < cap; i++){
                if(data[i].has_value()){
                    auto item = data[i].value();
                    (void)insert_impl(item.key, std::move(item.value));
                }
            }
        }

        bool insert_impl(const K& key, const T&& value){
            auto hash = H()(key);
            auto idx = hash % m_cap;
            auto start = idx;
            do{
                if(!(m_data[idx].has_value())){
                    m_data[idx] = Item{ .key = key, .value = value };
                    return true;
                } else if(m_data[idx].has_value() && m_data[idx].value().key == key){
                    return false;
                }
                idx = (idx + 1) % m_cap;
            } while(idx != start);
            return false;
        }
        item_t* find_impl(const K& key) const noexcept {
            auto hash = H()(key);
            auto idx = hash % m_cap;
            auto start = idx;
            do{
                if(m_data[idx].has_value() && m_data[idx].value().key == key){
                    return { &m_data[idx] };
                }
                idx = (idx + 1) % m_cap;
            } while(idx != start);
            return nullptr;
        }
    public:
        bool insert(std::pair<const K&, const T&&> p){
            return insert(p.first, std::move(p.second));
        }
        bool insert(const K& key, const T&& value){
            if(m_size / static_cast<float>(m_cap) > FILL_RATIO_GROW_THRESHOLD)
                grow();
            m_size++;
            return insert_impl(key, std::move(value));
        }
        T* find(const K& key) const noexcept {
            if(auto f = find_impl(key); f){
                return &(f->value().value);
            }
            return nullptr;
        }
        bool remove(const K& key){
            auto f = find_impl(key);
            if(f) {
                *f = std::nullopt;
                m_size--;
                if(m_size / static_cast<float>(m_cap) < FILL_RATIO_SHRINK_THRESHOLD)
                    shrink();
                return true;
            }
            return false;
        }
        bool contains(const K& key) const noexcept {
            auto f = find_impl(key);
            return f ? f->has_value() : false;
        }

        std::size_t size() const noexcept {
            return m_size;
        }
        bool empty() const noexcept {
            return m_size == 0;
        }
    };

    template<typename T, int HASHNUM>
    struct BloomHash{
        std::array<std::size_t, HASHNUM> operator()(const T& t)const noexcept;
    };

    template<typename T, int HASHNUM, typename BH = BloomHash<T, HASHNUM>>
    class BloomFilter{
    public:
        using byte_t = unsigned char;
        inline static constexpr size_t chunk_size = sizeof(byte_t);
        inline static constexpr size_t bits_in_chunk = chunk_size * 8;
    private:
        std::size_t m_size{};
        std::size_t m_bits{};
        byte_t* m_data{};

    public:
        inline BloomFilter(std::size_t size):
            m_size(size)
            , m_bits(m_size * bits_in_chunk)
        {
            m_data = new byte_t[m_size];
        }
        inline BloomFilter(const BloomFilter& other):
            m_size(other.m_size)
            , m_bits(m_size * bits_in_chunk)
        {
            m_data = new byte_t[m_size];
            std::copy(m_data, m_data + m_size, other.m_data);
        }
        inline BloomFilter& operator=(const BloomFilter& other)
        {
            m_size = other.m_size;
            m_bits = other.m_bits;
            m_data = new byte_t[m_size];
            std::copy(m_data, m_data + m_size, other.m_data);
        }
        inline BloomFilter(BloomFilter&& other):
            m_size(other.m_size)
            , m_bits(m_size * bits_in_chunk)
        {
            m_data = other.m_data;
            other.m_data = nullptr;
            other.m_size = 0;
        }
        inline BloomFilter& operator=(BloomFilter&& other)
        {
            m_size = other.m_size;
            m_bits = other.m_bits;
            m_data = other.m_data;
            other.m_data = nullptr;
            other.m_size = 0;
        }
        inline void insert(const T& val){
            std::array<std::size_t, HASHNUM> hashes = BH()(val);

            for(std::size_t i = 0; i < HASHNUM; i++){
                auto hash = hashes[i];
                auto bit_idx = hash % m_bits;
                auto chunk_idx = bit_idx / bits_in_chunk;
                auto bit_in_chunk_idx = bit_idx % bits_in_chunk;

                auto _in = (1 << bit_in_chunk_idx);
                // _in = (m_data[chunk_idx] | _in);
                m_data[chunk_idx] |= _in;
            }
        }
        inline bool contains(const T& val) const{
            std::array<std::size_t, HASHNUM> hashes = BH()(val);

            bool ret = true;

            for(std::size_t i = 0; i < HASHNUM; i++){
                auto hash = hashes[i];
                auto bit_idx = hash % m_bits;
                auto chunk_idx = bit_idx / bits_in_chunk;
                auto bit_in_chunk_idx = bit_idx % bits_in_chunk;

                auto _in = (1 << bit_in_chunk_idx);
                ret &= (_in == (m_data[chunk_idx] & _in));
            }
            return ret;
        }
        inline std::size_t chunks() const {
            return m_size;
        }
        inline std::size_t bits() const {
            return m_size * bits_in_chunk;
        }
        inline void clear(){
            std::fill(m_data, m_data + m_size, 0);
        }
    };

}
#endif
