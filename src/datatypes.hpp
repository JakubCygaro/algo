#ifndef DATATYPES_HPP
#define DATATYPES_HPP
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <print>
#include <tuple>
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
        struct Node {
            K key{};
            T data{};
            std::size_t size{1};
            Node* parent{};
            Node* left{}; //smaller
            Node* right{}; //larger
        };

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
            if(prev){
                prev->size++;
            }
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
        std::vector<std::tuple<K, T>> output_sorted() {
            std::vector<std::tuple<K, T>> ret(m_size);
            std::size_t i = 0;
            output_sorted_impl(i, ret, m_root);
            return ret;
        }
        bool delete_element(const K& key){
            auto node = search_impl(key);
            if(!node) return false;
start:
            auto parent = node->parent;
            // pointer to the pointer of the parent to this node
            Node** child_from_parent = nullptr;
            if(parent){
                child_from_parent = parent->left == node ? &parent->left : &parent->right;
            }
            // is leaf
            if(!node->left && !node->right){
                // make the parent forget me
                if(parent){
                    *child_from_parent = nullptr;
                    parent->size--;
                } else if (node == m_root){
                    m_root = nullptr;
                }
            } // two children case
            else if(node->left && node->right){
                auto n = predecessor_node(node);
                K tmp_k = n->key;
                T tmp_d = n->data;
                n->key = node->key;
                n->data = node->data;
                node->key = tmp_k;
                node->data = tmp_d;
                node->size--;
                node = n;
                goto start; //lets get sloppy
            } // only has left child
            else if(node->left){
                if(parent){
                    *child_from_parent = node->left;
                    parent->size--;
                    (*child_from_parent)->parent = parent;
                } else {
                    m_root = node->left;
                    m_root->parent = nullptr;
                    m_root->size--;
                }
            } // only has right child
            else if(node->right){
                if(parent){
                    *child_from_parent = node->right;
                    parent->size--;
                    (*child_from_parent)->parent = parent;
                } else {
                    m_root = node->right;
                    m_root->parent = nullptr;
                    m_root->size--;
                }

            }
            else {
                return false;
            }
            delete node;
            m_size--;
            return true;
        }
        std::tuple<K, T*> select(std::size_t i){
            if(i > m_size) return {};
            auto node = m_root;
            auto j = node->left ? node->left->size : 0;

            while(node){

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
            if(!node_x->left) return false;

            auto node_y = node_x->left;

            node_y->parent = node_x->parent;

            Node** child_ptr = node_y->parent->key < node_y->key ? &node_y->parent->right : &node_y->parent->left;
            *child_ptr = node_y;

            node_x->left = node_y->right;
            node_y->right->parent = node_x;

            node_x->parent = node_y;
            node_y->right = node_x;
        }
        bool rotate_left(Node* node_x){
            if(!node_x->right) return false;

            auto node_y = node_x->right;

            node_y->parent = node_x->parent;

            Node** child_ptr = node_y->parent->key < node_y->key ? &node_y->parent->right : &node_y->parent->left;
            *child_ptr = node_y;

            node_x->right = node_y->left;
            node_y->left->parent = node_x;

            node_x->parent = node_y;
            node_y->left = node_x;
        }
    public:
    };
}
#endif
