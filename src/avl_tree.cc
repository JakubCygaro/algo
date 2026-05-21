#include <cstddef>
template <typename K, typename T>
class AVLTree {
private:
    enum class Child {
        LEFT = 0,
        RIGHT = 1,
        ROOT = -1,
    };
    enum class Balance : char {
        LEFT_HEAVY = -1,
        BALANCED = 0,
        RIGHT_HEAVY = 1,
    };
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
        while (z && z->balance != Balance::BALANCED) {
            Node* n, *g;
            auto* x = z->parent;
            auto d = z->get_child_dir();
            // update balance factor of parent
            if (d == Child::RIGTH) {
                if (x->balance == Balance::RIGHT_HEAVY) {
                    g = x->parent;
                    if (z->balance == Balance::LEFT_HEAVY) {
                        rotate_right(z);
                        n = rotate_left(x);
                    } else {
                        n = rotate_left(x);
                    }
                } else {
                    if (x->balance == Balance::LEFT_HEAVY) {
                        x->balance = Balance::BALANCED;
                        break;
                    }
                    x->balance = Balance::RIGHT_HEAVY;
                    z = x;
                    continue;
                }
            } else {
                if (x->balance == Balance::LEFT_HEAVY) {
                    g = x->parent;
                    if (z->balance == Balance::RIGHT_HEAVY) {
                        rotate_left(z);
                        n = rotate_right(x);
                    } else {
                        n = rotate_right(x);
                    }
                } else {
                    if (x->balance == Balance::RIGHT_HEAVY) {
                        x->balance = Balance::BALANCED;
                        break;
                    }
                    x->balance = Balance::LEFT_HEAVY;
                    z = x;
                    continue;
                }
            }
            n->parent = g;
            if(g){
                g->_ord[d] = n;
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
        (*to_insert)->balance = 0;
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

public:
    bool delete_element(const K& key)
    {
        auto z = search_impl(key);
        if (!z)
            return false;

        return false;
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
    void transplant(Node* u, Node* v)
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
    }

public:
};
int main(void)
{
    AVLTree<char, int> t{};
}
