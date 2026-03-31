#include "common.hpp"
#include <cstddef>
#include <memory>
#include <vector>

template <typename K, size_t B>
class BTree {
    static_assert(B >= 2);
    struct Node {
        bool is_leaf {};
        size_t n;
        K* k {};
        Node* c {};
    };
    Node* root {};
    BTree()
    {
        root = new Node;
        root->is_leaf = true;
        root->k = new K[2 * B - 1];
        root->c = new K[2 * B];
    }
    Node* search(K key)
    {
        if (!root)
            return nullptr;
        return search_impl(key, root);
    }
    Node* search_impl(K key, Node* x)
    {
        auto i = 0;
        while (i < x->n && key > x->k[i]) {
            i++;
        }
        if (i < x->n && key == x->k[i]) {
            return x;
        } else if (x->is_leaf) {
            return nullptr;
        } else {
            return search_impl(key, x->c[i]);
        }
    }
    void split_child(Node* x, size_t i)
    {
        auto z = new Node;
        auto y = x->c[i];
        z->is_leaf = y->is_leaf;
        z->k = new K[2 * B - 1];
        for (auto j = 1; j <= B - 1; j++) {
            z->k[j - 1] = y->k[j - 1 + B];
        }
        if (!y->is_leaf) {
            for (auto j = 1; j <= B; j++) {
                z->c[j - 1] = y->c[j - 1 + B];
            }
        }
        y->n = B - 1;
        for (auto j = x->n + 1; j >= i + 1; j--) {
            x->c[j - 1 + 1] = x->c[j - 1];
        }
        x->c[i - 1 + 1] = z;
        for (auto j = x->n; j >= i; j--) {
            x->k[j - 1 + 1] = x->key[j - 1];
        }
        x->k[i] = y->key[B];
        x->n = x->n + 1;
    }
    void insert(K key)
    {
        insert_impl(key, root);
    }
    void insert_impl(K key, Node* r)
    {
        if (r->n == 2 * B - 1){

        }
    }
};

int main(void)
{
}
