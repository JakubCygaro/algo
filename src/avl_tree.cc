#include <cassert>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <print>
#include <utility>
#include <vector>
template <typename K, typename T>
class AVLTree {
private:
    enum class Child {
        LEFT = 0,
        RIGHT = 1,
        ROOT = -1,
    };
    const char LEFT_HEAVY = -1;
    const char BALANCED = 0;
    const char RIGHT_HEAVY = 1;
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
        Child get_child_dir()
        {
            if (this->parent) {
                if (this->parent->left == this) {
                    return Child::LEFT;
                } else {
                    return Child::RIGHT;
                }
            } else {
                return Child::ROOT;
            }
        }
    };
    Node* m_root { };
    std::size_t m_size { };

public:
    AVLTree()
    {
    }

private:
    Node* search_impl(const K& key) const
    {
        auto node = m_root;
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

private:
    void insert_fixup(Node* z)
    {
        for (Node* x = z->parent; x != nullptr; x = z->parent) {
            Node *n{}, *g{};
            auto x_d = z->get_child_dir();
            // update balance factor of parent
            if (x_d == Child::RIGHT) {
                if (x->balance == RIGHT_HEAVY) {
                    g = x->parent;
                    if (z->balance == LEFT_HEAVY) {
                        n = rotate_right_left(x, z);
                    } else {
                        n = rotate_left(x, z);
                    }
                } else {
                    if (x->balance == LEFT_HEAVY) {
                        x->balance = BALANCED;
                        break;
                    }
                    x->balance = RIGHT_HEAVY;
                    z = x;
                    continue;
                }
            } else {
                if (x->balance == LEFT_HEAVY) {
                    g = x->parent;
                    if (z->balance == RIGHT_HEAVY) {
                        n = rotate_left_right(x, z);
                    } else {
                        n = rotate_right(x, z);
                    }
                } else {
                    if (x->balance == RIGHT_HEAVY) {
                        x->balance = BALANCED;
                        break;
                    }
                    x->balance = LEFT_HEAVY;
                    z = x;
                    continue;
                }
            }
            n->parent = g;
            if (g) {
                g->_ord[(int)x_d] = n;
                // d = x->get_child_dir();
                // if(d == Child::LEFT){
                //     g->left = n;
                // } else {
                //     g->right = n;
                // }
            } else {
                this->m_root = n;
            }
            break;
        }
    }

public:
    bool insert(const K& key, T&& item)
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
        (*to_insert)->size = 1;
        (*to_insert)->balance = BALANCED;
        m_size++;
        insert_fixup(*to_insert);
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
        auto found = search_impl(key);
        if (found) {
            return &found->data;
        } else {
            return nullptr;
        }
    }

private:
    void delete_fixup(Node* n)
    {
        Node* g { };
        Node* z { };
        char b = 0;
        for (Node* x = n->parent; x != nullptr; x = g) {
            g = x->parent;
            if (n == x->left) {
                if (x->balance == RIGHT_HEAVY) {
                    z = x->right;
                    b = z->balance;
                    if (b < 0) {
                        n = rotate_right_left(x, z);
                    } else {
                        n = rotate_left(x, z);
                    }
                } else {
                    if (x->balance == 0) {
                        x->balance = RIGHT_HEAVY;
                        break;
                    }
                    n = x;
                    n->balance = BALANCED;
                    continue;
                }
            } else {
                if (x->balance == LEFT_HEAVY) {
                    z = x->left;
                    b = z->balance;
                    if (b > 0) {
                        n = rotate_left_right(x, z);
                    } else {
                        n = rotate_right(x, z);
                    }
                } else {
                    if (x->balance == BALANCED) {
                        x->balance = LEFT_HEAVY;
                        break;
                    }
                    n = x;
                    n->balance = BALANCED;
                    continue;
                }
            }
            n->parent = g;
            if (g) {
                if (x == g->left) {
                    g->left = n;
                } else
                    g->right = n;
            } else {
                this->m_root = n;
            }
            if (b == 0)
                break;
        }
    }

public:
    bool delete_element(const K& key)
    {
        auto z = search_impl(key);
        if (!z)
            return false;
        auto x = z->parent;
        if (Node* c = z->left ? z->left : z->right; c){
            transplant(z, c);
            c->left = z->left;
            c->right = z->right;
            delete_fixup(c);
        }
        else if (auto z_d = z->get_child_dir(); z != this->m_root) {
            x->_ord[(int)z_d] = nullptr;
            delete_fixup(x);
        }
        delete z;

        return true;
    }

#define priv private:
#define pub public:

    priv void _print_traverse(Node* n, int depth)
    {
        if (!n)
            return;
        if (n->right)
            _print_traverse(n->right, depth + 2);
        std::printf("%*c", depth, ' ');
        std::println("{} : {}", n->key, (int)n->balance);
        if (n->left)
            _print_traverse(n->left, depth + 2);
        std::flush(std::cout);
    }

    pub void print_traverse()
    {
        _print_traverse(m_root, 0);
    }

private:
    Node* rotate_left(Node* x, Node* z)
    {
        auto t23 = z->left;
        x->right = t23;
        if(t23)
            t23->parent = x;
        z->left = x;
        x->parent = z;
        if(z->balance == BALANCED){
            x->balance = RIGHT_HEAVY;
            z->balance = LEFT_HEAVY;
        } else {
            x->balance = BALANCED;
            z->balance = BALANCED;
        }
        return z;
    }
    Node* rotate_right(Node* x, Node* z)
    {
        auto t23 = z->right;
        x->left = t23;
        if(t23)
            t23->parent = x;
        z->right = x;
        x->parent = z;
        if(z->balance == BALANCED){
            x->balance = LEFT_HEAVY;
            z->balance = RIGHT_HEAVY;
        } else {
            x->balance = BALANCED;
            z->balance = BALANCED;
        }
        return z;
    }
    Node* rotate_right_left(Node* x, Node* z)
    {
        auto y = z->left;
        auto t3 = y->right;
        z->left = t3;
        if(t3)
            t3->parent = z;
        y->right = z;
        z->parent = y;
        auto t2 = y->left;
        x->right = t2;
        if(t2)
            t2->parent = x;
        y->left = x;
        x->parent = y;
        if(y->balance == BALANCED) {
            x->balance = BALANCED;
            z->balance = BALANCED;
        } else if (y->balance == RIGHT_HEAVY){
            x->balance = LEFT_HEAVY;
            z->balance = BALANCED;
        } else {
            x->balance = BALANCED;
            z->balance = RIGHT_HEAVY;
        }
        y->balance = BALANCED;
        return y;
    }
    Node* rotate_left_right(Node* x, Node* z)
    {
        auto y = z->right;
        auto t3 = y->left;
        z->right = t3;
        if(t3)
            t3->parent = z;
        y->left = z;
        z->parent = y;
        auto t2 = y->right;
        x->left = t2;
        if(t2)
            t2->parent = x;
        y->right = x;
        x->parent = y;
        if(y->balance == BALANCED) {
            x->balance = BALANCED;
            z->balance = BALANCED;
        } else if (y->balance == LEFT_HEAVY){
            x->balance = RIGHT_HEAVY;
            z->balance = BALANCED;
        } else {
            x->balance = BALANCED;
            z->balance = LEFT_HEAVY;
        }
        y->balance = BALANCED;
        return y;
    }
    Node* transplant(Node* u, Node* v)
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

public:
};
int main(void)
{
    AVLTree<char, int> t { };
    std::vector<std::pair<char, int>> vals = {
        { 'm', 0 },
        { 'n', 1 },
        { 'o', 2 },
        { 'l', 3 },
        { 'k', 4 },
        { 'q', 5 },
        { 'p', 6 },
        { 'h', 6 },
        { 'i', 6 },
        { 'a', 6 },
    };
    for (auto& p : vals) {
        // std::println("=========");
        t.insert(std::get<0>(p), std::move(std::get<1>(p)));
        // t.print_traverse();
        // std::println("=========");
    }

    for (auto& p : vals) {
        if (auto* v = t.search(std::get<0>(p)); v) {
            std::println("{} : {}", std::get<0>(p), *v);
        }
    }
    while(!vals.empty()){
        auto [k, v] = vals.back();
        vals.pop_back();
        std::println("delete {}", k);
        t.delete_element(k);
        for (auto& p : vals) {
            auto key = std::get<0>(p);
            std::println("check {}", key);
            std::flush(std::cout);
            assert(t.search(key));
        }
    }
}
