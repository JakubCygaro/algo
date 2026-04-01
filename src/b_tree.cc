#include "common.hpp"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <format>
#include <memory>
#include <print>
#include <set>
#include <vector>

template <typename K, size_t B>
class BTree {
    static_assert(B >= 2);
    struct Node {
        bool is_leaf { };
        size_t n;
        K* k { };
        Node** c { };
        static Node* make()
        {
            Node* n = new Node;
            n->n = 0;
            n->k = new K[2 * B - 1];
            std::memset(n->k, 0, (2 * B - 1) * sizeof(K));
            n->c = new Node*[2 * B];
            std::memset(n->c, 0, 2 * B * sizeof(Node*));
            return n;
        }
    };
    Node* root { };

public:
    BTree()
    {
        root = Node::make();
        root->is_leaf = true;
    }
    std::pair<Node*, size_t> search(K key)
    {
        if (!root)
            return { nullptr, 0 };
        return search_impl(key, root);
    }
    std::pair<Node*, size_t> search_impl(K key, Node* x)
    {
        auto i = 0;
        while (i < x->n && key > x->k[i]) {
            i++;
        }
        if (i < x->n && key == x->k[i]) {
            return { x, i };
        } else if (x->is_leaf) {
            return { nullptr, 0 };
        } else {
            return search_impl(key, x->c[i]);
        }
    }
    void split_child(Node* x, int i)
    {
        auto z = Node::make();
        auto y = x->c[i];
        z->is_leaf = y->is_leaf;
        z->n = B - 1;
        for (auto j = 0; j < B - 1; j++) {
            z->k[j] = y->k[j + B];
        }
        if (!y->is_leaf) {
            for (auto j = 0; j < B; j++) {
                z->c[j] = y->c[j + B];
            }
        }
        y->n = B;
        for (auto j = x->n; j > i; j--) {
            x->c[j + 1] = x->c[j];
        }
        x->c[i + 1] = z;
        for (int j = x->n - 1; j >= i; j--) {
            x->k[j + 1] = x->k[j];
        }
        x->k[i] = y->k[B];
        x->n++;
    }
    void insert(K key)
    {
        auto r = root;
        if (r->n == 2 * B - 1) {
            auto s = Node::make();
            root = s;
            s->n = 0;
            s->c[0] = r;
            split_child(s, 0);
            insert_nonfull(s, key);
        } else
            insert_nonfull(r, key);
    }
    void insert_nonfull(Node* x, K key)
    {
        int i = x->n - 1;
        if (x->is_leaf) {
            while (i >= 0 && key < x->k[i]) {
                x->k[i + 1] = x->k[i];
                i--;
            }
            x->k[i + 1] = key;
            x->n++;
        } else {
            while (i >= 0 && key < x->k[i]) {
                i--;
            }
            i++;
            if (x->c[i]->n == 2 * B - 1) {
                split_child(x, i);
                if (key > x->k[i]) {
                    i++;
                }
            }
            insert_nonfull(x->c[i], key);
        }
    }
    void remove(K key)
    {
        remove_impl(root, key);
    }
    int succ(Node* x, K key)
    {
        int j = 0;
        while (j < x->n && x->k[j] <= key) {
            j++;
        }
        return j;
    }
    int pred(Node* x, K key)
    {
        int j = x->n - 1;
        while (j >= 0 && x->k[j] > key) {
            j--;
        }
        return j;
    }
    void remove_impl(Node* x, K key)
    {
        auto i = 0;
        while (i < x->n && key > x->k[i]) {
            i++;
        }
        // found case
        if (i < x->n && key == x->k[i]) {
            // is leaf case
            if (x->is_leaf) {
                for (auto j = i; j < x->n; j++) {
                    x->k[j] = x->k[j + 1];
                }
                x->n--;
            } else {
                auto y = x->c[i];
                // case 2a
                if (y->n >= B) {
                    auto j = pred(y, key);
                    auto key_ = y->k[j];
                    remove_impl(y, key_);
                    x->k[i] = key_;
                } else {
                    auto z = x->c[i + 1];
                    // case 2b
                    if (z->n >= B) {
                        auto j = succ(z, key);
                        auto key_ = z->k[j];
                        remove_impl(z, key_);
                        x->k[i] = key_;
                    }
                    // case 2c
                    else {
                        for (auto l = i; i < x->n; i++) {
                            x->k[l] = x->k[l + 1];
                            x->c[l + 1] = x->c[l + 2];
                        }
                        x->n--;
                        // merge k into y
                        y->k[y->n] = key;
                        y->n++;
                        // merge keys of z into y
                        for (auto l = 0; l < z->n; l++) {
                            y->k[y->n + l] = z->k[l];
                        }
                        // merge children of z into y
                        for (auto l = 1; l < z->n + 1; l++) {
                            y->c[y->n + l] = z->c[l];
                        }
                        y->n += z->n;
                        delete z->k;
                        delete z->c;
                        delete z;
                        remove_impl(y, key);
                    }
                }
            }
        } else {
            if (x->c[i]->n == B - 1) {
                auto sibling = 0;
                if (i > 0 && x->c[i - 1]->n >= B) {
                    sibling = i - 1;
                } else if (i < x->n - 1 && x->c[i + 1]->n >= B) {
                    sibling = i + 1;
                }
            }
            remove_impl(key, x->c[i]);
        }
    }
};

int main(void)
{
    auto tree = BTree<int, 2>();
    std::set<int> s { };
    for (auto i = 0; i < 50; i++) {
        s.insert(common::get_random_in_range(0, 100));
    }
    for (const auto e : s) {
        tree.insert(e);
    }
    for (const auto e : s) {
        auto [p, v] = tree.search(e);
        if (!p) {
            auto f = std::format("{} was not present inside B-tree", e);
            std::printf("%s\n", f.c_str());
        }
        assert(p);
    }
}
