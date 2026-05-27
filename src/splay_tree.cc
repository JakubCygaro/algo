#include "common.hpp"
#include <cstddef>
#include <iostream>
#include <print>

#define priv private:
#define pub public:

template <typename K, typename T>
class SplayTree {
private:
    struct Node {
        K key { };
        T data { };
        std::size_t size { 1 };
        Node* parent { };
        union {
            struct {
                Node* left { nullptr }; // smaller
                Node* right { nullptr }; // larger
            };
            Node* _ord[2];
        };
        char balance;
    };
    Node* m_root { };
    std::size_t m_size { };

    pub SplayTree()
    {
    }

    priv Node* search_impl(const K& key, Node* r) const
    {
        auto node = r;
        while (node) {
            if (node->key == key) {
                return node;
            } else if (key > node->key) {
                node = node->right;
            } else {
                node = node->left;
            }
        }
        return nullptr;
    }

    pub bool insert(const K key, T&& item)
    {
        Node* prev = nullptr;
        Node** to_insert = &m_root;

        while (*to_insert) {
            if (key > (*to_insert)->key) {
                prev = *to_insert;
                to_insert = &((*to_insert)->right);
            } else if (key < (*to_insert)->key) {
                prev = *to_insert;
                to_insert = &((*to_insert)->left);
            } else {
                return false;
            }
        }
        *to_insert = new Node;
        (*to_insert)->key = key;
        (*to_insert)->data = item;
        (*to_insert)->parent = prev;
        m_size++;
        splay(*to_insert);
        return true;
    }
    std::size_t size() const
    {
        return m_size;
    }
    bool empty() const
    {
        return m_size == 0;
    }
    T* search(const K key) const
    {
        auto found = search_impl(key, m_root);
        if (found) {
            return &found->data;
        } else {
            return nullptr;
        }
    }

    priv Node* succ(Node* n)
    {
        while (n->left) {
            n = n->left;
        }
        return n;
    }
    priv Node* pred(Node* n)
    {
        while (n->right) {
            n = n->right;
        }
        return n;
    }
    priv bool delete_element_impl(const K& key, Node* r)
    {
        auto z = search_impl(key, r);
        if (!z)
            return false;
        if (z->left && z->right) {
            auto p = pred(z->left);
            std::swap(p->key, z->key);
            std::swap(p->data, z->data);
            return delete_element_impl(p->key, p);
        } else if (auto c = z->left ? z->left : z->right; c) {
            transplant(z, c);
        } else {
            transplant(z, nullptr);
        }
        splay(z->parent);
        delete z;
        m_size--;

        return true;
    }

    pub bool delete_element(const K& key)
    {
        return delete_element_impl(key, m_root);
    }

    priv void _print_traverse(Node* n, int depth)
    {
        if (!n)
            return;
        if (n->right)
            _print_traverse(n->right, depth + 2);
        std::printf("%*c", depth, ' ');
        std::println("{}", n->key);
        if (n->left)
            _print_traverse(n->left, depth + 2);
        std::flush(std::cout);
    }

    pub void print_traverse()
    {
        std::println("======");
        _print_traverse(m_root, 0);
        std::println("======");
    }

    priv void splay(Node* x)
    {
        while (x->parent) {
            auto p = x->parent;
            auto g = p->parent;
            // p is root
            if (!g) {
                // zig
                if (x == p->left)
                    x = rotate_right(p, x);
                else
                    x = rotate_left(p, x);
            } else if ((x == p->left && p == g->left)
                || (x == p->right && p == g->right)) {
                // zig-zig
                if (x == p->left) {
                    p = rotate_right(g, p);
                    x = rotate_right(p, x);
                } else {
                    p = rotate_left(g, p);
                    x = rotate_left(p, x);
                }
            } else {
                // zig-zag
                if (x == p->left)
                    x = rotate_right(p, x);
                else
                    x = rotate_left(p, x);
                if (x == g->left)
                    x = rotate_right(g, x);
                else
                    x = rotate_left(g, x);
            }
        }
    }

    priv Node* rotate_left(Node* x, Node* z)
    {
        auto t23 = z->left;
        x->right = t23;
        if (t23)
            t23->parent = x;
        z->left = x;
        z->parent = x->parent;
        if (!x->parent)
            m_root = z;
        else if (x == x->parent->left)
            x->parent->left = z;
        else
            x->parent->right = z;
        x->parent = z;
        return z;
    }
    priv Node* rotate_right(Node* x, Node* z)
    {
        auto t23 = z->right;
        x->left = t23;
        if (t23)
            t23->parent = x;
        z->right = x;
        z->parent = x->parent;
        if (!x->parent)
            m_root = z;
        else if (x == x->parent->left)
            x->parent->left = z;
        else
            x->parent->right = z;
        x->parent = z;
        return z;
    }
    priv Node* transplant(Node* u, Node* v)
    {
        if (!u->parent) {
            m_root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        if (v) {
            v->parent = u->parent;
        }
        return v;
    }

    priv void count_nodes_impl(Node* r, std::size_t* counter)
    {
        if (r)
            (*counter)++;
        else
            return;
        count_nodes_impl(r->right);
        count_nodes_impl(r->left);
    }

    priv void count_nodes()
    {
        m_size = 0;
        count_nodes_impl(m_root, &m_size);
    }

    priv SplayTree(Node* root)
    {
        m_root = root;
        count_nodes();
    }

    pub void join(SplayTree&& t2)
    {
        auto largest = succ(m_root);
        splay(largest);
        m_root->right = t2.m_root;
        t2.m_root->parent = m_root;
        t2.m_root = nullptr;
        count_nodes();
    }

    pub SplayTree split(Node* n)
    {
        splay(n);
        auto right = m_root->right;
        right->parent = nullptr;
        m_root->right = nullptr;
        count_nodes();
        return SplayTree(right);
    }
};
int main(void)
{
    SplayTree t = SplayTree<char, int>();
    t.insert('a', 69);
    t.insert('f', 420);
    t.insert('l', 2137);
    t.insert('b', 1337);
}
