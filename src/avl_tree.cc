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
        while (z && z->balance != BALANCED) {
            Node *n, *g;
            auto* x = z->parent;
            auto d = z->get_child_dir();
            // update balance factor of parent
            if (d == Child::RIGHT) {
                if (x->balance == RIGHT_HEAVY) {
                    g = x->parent;
                    if (z->balance == LEFT_HEAVY) {
                        rotate_right(z);
                        n = rotate_left(x);
                    } else {
                        n = rotate_left(x);
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
                        rotate_left(z);
                        n = rotate_right(x);
                    } else {
                        n = rotate_right(x);
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
                g->_ord[(int)d] = n;
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
            } else if (key < (*to_insert)->data) {
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
            auto n_d = n->get_child_dir();
            if (n_d == Child::LEFT) {
                if (x->balance == RIGHT_HEAVY) {
                    z = x->right;
                    b = z->balance;
                    if (b < 0) {
                        rotate_right(z);
                        n = rotate_left(x);
                    } else {
                        n = rotate_left(x);
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
                        rotate_left(z);
                        n = rotate_right(x);
                    } else {
                        n = rotate_right(x);
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
        Node* v = x;
        if (z->left || z->right)
            v = transplant(z, z->left ? z->left : z->right);
        else if (auto z_d = z->get_child_dir(); z_d != Child::ROOT) {
            x->_ord[(int)z_d] = nullptr;
            delete_fixup(v);
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
        std::println("{} : {}", n->key, n->data);
        if (n->left)
            _print_traverse(n->left, depth + 2);
        else {
            std::printf("%*c", depth, ' ');
            std::println("null");
        }
        std::flush(std::cout);
    }

    pub void print_traverse()
    {
        _print_traverse(m_root, 0);
    }

private:
    Node* rotate_right(Node* node_x)
    {
        if (!node_x)
            return nullptr;
        if (!node_x->left)
            return nullptr;

        auto node_y = node_x->left;

        node_y->parent = node_x->parent;

        // replace node_x in node_x's parent
        if (node_y->parent) {
            Node** child_ptr = node_y->parent->right == node_x ? &node_y->parent->right
                                                               : &node_y->parent->left;
            *child_ptr = node_y;
        } else {
            m_root = node_y;
        }

        node_x->left = node_y->right;
        if (node_x->left)
            node_x->left->parent = node_x;

        node_x->parent = node_y;
        node_y->right = node_x;

        return node_y;
    }
    Node* rotate_left(Node* node_x)
    {
        if (!node_x)
            return nullptr;
        if (!node_x->right)
            return nullptr;

        auto node_y = node_x->right;

        node_y->parent = node_x->parent;

        if (node_y->parent) {
            Node** child_ptr = node_y->parent->right == node_x ? &node_y->parent->right
                                                               : &node_y->parent->left;
            *child_ptr = node_y;
        } else {
            m_root = node_y;
        }
        node_x->right = node_y->left;
        if (node_x->right)
            node_x->right->parent = node_x;

        node_x->parent = node_y;
        node_y->left = node_x;

        return node_y;
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
        { 'a', 0 },
        { 'b', 1 },
        { 'c', 2 },
        { 'd', 3 },
        { 'e', 4 },
        { 'f', 5 },
        { 'g', 6 },
    };
    for (auto& p : vals) {
        t.insert(std::get<0>(p), std::move(std::get<1>(p)));
    }

    for (auto& p : vals) {
        if (auto* v = t.search(std::get<0>(p)); v) {
            std::println("{} : {}", std::get<0>(p), *v);
        }
    }
    t.print_traverse();
    for (auto& p : vals) {
        t.delete_element(std::get<0>(p));
    }
    for (auto& p : vals) {
        if (auto* v = t.search(std::get<0>(p)); v) {
            std::println("{} : {}", std::get<0>(p), *v);
        }
    }
}
