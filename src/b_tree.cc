#include "common.hpp"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <random>
#include <set>

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
    BTree(BTree&& other)
    {
        this->root = other.root;
        other->root = nullptr;
    }
    BTree& operator=(BTree&& other) noexcept
    {
        this->root = other.root;
        other.root = nullptr;
        return *this;
    }
    BTree(const BTree& other) = delete;
    BTree operator=(const BTree& other) = delete;

    auto descend_free(Node* x) -> void
    {
        // if (!x->is_leaf) {
        //     for (auto i = 0; i < x->n + 1; i++) {
        //         descend_free(x->c[i]);
        //     }
        // }
        // delete x->c;
        // delete x->k;
        // delete x;
    }

    ~BTree()
    {
        descend_free(root);
        root = nullptr;
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
        y->n = B - 1;
        x->n++;
        rsh_c(x, i + 1);
        x->c[i + 1] = z;
        rsh_k(x, i);
        x->k[i] = y->k[B - 1];
        norm(x);
        norm(z);
        norm(y);
    }
    void insert(K key)
    {
        auto r = root;
        if (r->n == 2 * B - 1) {
            auto s = Node::make();
            root = s;
            s->is_leaf = false;
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
            norm(x);
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
        while (j >= 0 && x->k[j] >= key) {
            j--;
        }
        return j;
    }
    void lsh_k(Node* x, int start)
    {
        for (auto j = start + 1; j < x->n; j++) {
            x->k[j - 1] = x->k[j];
        }
    }
    void lsh_c(Node* x, int start)
    {
        for (auto j = start + 1; j < x->n + 1; j++) {
            x->c[j - 1] = x->c[j];
        }
    }
    void rsh_k(Node* x, int low, size_t n = 1)
    {
        for (auto i = 0; i < n; i++)
            for (int j = x->n - 1; j >= low; j--) {
                x->k[j + 1] = x->k[j];
            }
    }
    void rsh_c(Node* x, int low, size_t n = 1)
    {
        for (auto i = 0; i < n; i++)
            for (int j = x->n; j >= low; j--) {
                x->c[j + 1] = x->c[j];
            }
    }
    void norm(Node* x)
    {
        std::memset(x->k + x->n, 0, sizeof(K) * (2 * B - 1 - x->n));
        // std::memset(x->c + x->n + 1, 0, sizeof(K) * (2 * B - x->n + 1));
    }
    void rm_key(Node* x, size_t idx)
    {
        lsh_k(x, idx);
        x->n--;
        norm(x);
    }
    void remove_impl(Node* x, K key)
    {
        auto i = 0;
        while (i < x->n && key > x->k[i]) {
            i++;
        }
        // found case
        if (i < x->n && key == x->k[i]) {
            // case 1
            if (x->is_leaf) {
                rm_key(x, i);
            }
            // case 2
            else {
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
                        lsh_k(x, i);
                        lsh_c(x, i + 1);
                        x->n--;
                        // merge k into y
                        y->k[y->n] = key;
                        y->n++;
                        // merge keys of z into y
                        for (auto l = 0; l < z->n; l++) {
                            y->k[y->n + l] = z->k[l];
                        }
                        // merge children of z into y
                        for (auto l = 0; l < z->n + 1; l++) {
                            y->c[y->n + l] = z->c[l];
                        }
                        y->n += z->n;
                        norm(x);
                        delete z->k;
                        delete z->c;
                        delete z;
                        remove_impl(y, key);
                        if (x == root && x->n == 0)
                            root = y;
                    }
                }
            }
        }
        // case 3
        else {
            auto x_c = x->c[i];
            if (x_c->n == B - 1) {
                int sibling = -1;
                bool right = false;
                bool none = true;
                // check right sibling
                if (i + 1 <= x->n && x->c[i + 1]->n >= B) {
                    sibling = i + 1;
                    right = true;
                    none = false;
                }
                // check left sibling
                if (!right && i - 1 >= 0 && x->c[i - 1]->n >= B) {
                    sibling = i - 1;
                    none = false;
                }
                // case 3b
                if (none) {
                    // has right sibling ?
                    right = i + 1 <= x->n;
                    // right = !(i-1 >= 0);
                    sibling = right ? i + 1 : i - 1;
                    auto s = x->c[sibling];
                    // merge right sibling
                    if (right) {
                        // move key from root into x_c
                        x_c->k[x_c->n] = x->k[i];
                        x_c->n++;

                        // shift k and c of x left, so that sibling pointer is lost
                        lsh_k(x, i);
                        lsh_c(x, sibling);
                        x->n--;

                        // merging right sibling into x_c
                        for (auto j = 0; j < s->n; j++) {
                            x_c->k[x_c->n + j] = s->k[j];
                        }
                        for (auto j = 0; j < s->n + 1; j++) {
                            x_c->c[x_c->n + j] = s->c[j];
                        }
                        x_c->n += s->n;
                    } else {
                        x_c->n++;
                        rsh_k(x_c, 0);
                        // move key from root into x_c
                        x_c->k[0] = x->k[sibling];

                        // shift k and c of x left, so that sibling pointer is lost
                        lsh_k(x, sibling);
                        lsh_c(x, sibling);

                        // shrink x
                        x->n--;

                        // shift x_c to the right making place for merged s
                        x_c->n += s->n;
                        rsh_k(x_c, 0, s->n);
                        rsh_c(x_c, 0, s->n + 1);

                        // merging left sibling into x_c
                        for (auto j = 0; j < s->n; j++) {
                            x_c->k[j] = s->k[j];
                        }
                        for (auto j = 0; j < s->n + 1; j++) {
                            x_c->c[j] = s->c[j];
                        }
                    }
                    delete s->k;
                    delete s->c;
                    delete s;

                    norm(x_c);
                    norm(x);

                    if (x == root && x->n == 0)
                        root = x_c;
                } else if (sibling != -1) {
                    auto s = x->c[sibling];
                    if (right) {
                        // move key from root into x_c
                        x_c->k[x_c->n] = x->k[i];
                        x_c->n++;

                        // move key from right sibling into root
                        x->k[i] = s->k[0];
                        // move child from right sibling into x_c
                        x_c->c[x_c->n] = s->c[0];

                        lsh_k(s, 0);
                        lsh_c(s, 0);

                        s->n--;
                        norm(x_c);
                        norm(s);
                    } else {
                        x_c->n++;
                        rsh_k(x_c, 0);
                        rsh_c(x_c, 0);
                        // move key from root into x_c
                        x_c->k[0] = x->k[sibling];

                        // move key from left sibling into root
                        x->k[sibling] = s->k[s->n - 1];

                        // move child pointer from left sibling into x_c
                        x_c->c[0] = s->c[s->n];

                        s->n--;
                        norm(x_c);
                        norm(s);
                    }
                }
            }
            remove_impl(x_c, key);
        }
    }
    void print_tree()
    {
        print_tree_impl(root, 0);
        std::printf("======\n");
    }
    void print_tree_impl(Node* x, int depth)
    {
        if (x->is_leaf) {
            std::printf("%*s", x == root ? 0 : depth, x == root ? "" : "-");
            for (auto i = 0; i < x->n; i++) {
                std::printf("%c ", x->k[i]);
            }
            std::printf("\n");
        } else {
            for (auto i = 0; i < x->n; i++) {
                print_tree_impl(x->c[i], depth + 2);
                std::printf("%*s%c\n", x == root ? 0 : depth, x == root ? "" : "-", x->k[i]);
            }
            if (x->n > 0)
                print_tree_impl(x->c[x->n], depth + 2);
        }
    }
};

int main(void)
{
    auto tree = BTree<char, 3>();
    std::set<char> s { };

    std::optional<char> last_remove { };
    const auto verify = [&] {
        for (const auto e : s) {
            auto [p, i] = tree.search(e);
            if (!p) {
                std::printf("Key '%c' not present in tree\n", e);
                if (last_remove)
                    std::printf("Last remove was '%c'\n", *last_remove);
                tree.print_tree();
            }
            assert(p);
        }
    };
    const auto remove_verify = [&](char k) {
        // std::printf("%c ", k);
        tree.remove(s.extract(k).value());
        // tree.print_tree();
        auto [p, v] = tree.search(k);
        assert(!p);
        verify();
        last_remove = k;
    };
    const auto insert = [&](char c) {
        s.insert(c);
        tree.insert(c);
    };
    const auto insert_l = [&](std::initializer_list<char> cs) {
        for (const auto c : cs) {
            insert(c);
            tree.print_tree();
        }
    };

    std::vector<char> input_data { };
    for (auto i = 'A'; i <= 'Z'; i++)
        input_data.push_back(i);

    for (auto i = 0; i < 1; i++) {
        s.clear();
        // tree = BTree<char, 3>();
        insert_l(
            { 'V', 'P', 'S', 'Y', 'I', 'R', 'Q', 'A', 'D', 'F', 'H', 'W', 'E', 'M', 'N', 'T', 'G', 'X', 'B', 'Z', 'K', 'U', 'C', 'L', 'O', 'J' });
        // insert_l({'A', 'D', 'F', 'H', 'L', 'N', 'P'});
        // remove_verify('D');
        // // tree.print_tree();
        // remove_verify('K');
        // // tree.print_tree();
        // remove_verify('F');
        // tree.print_tree();
        // remove_verify('B');
        tree.print_tree();
        remove_verify('P');
        // std::random_shuffle(input_data.begin(), input_data.end());
        // std::printf("inserting: ");
        // for(auto e : input_data){
        //     std::printf("%c ", e);
        //     insert(e);
        // }
        // std::printf("\n");
        // tree.print_tree();
        // auto s_cpy = std::set<char>(s);
        auto rnd = std::vector<char>();

        std::copy(s.begin(), s.end(), std::back_inserter(rnd));
        std::random_shuffle(rnd.begin(), rnd.end());

        std::printf("removing: ");
        for (auto it = rnd.begin(); it != rnd.end(); it++) {
            remove_verify(*it);
        }
        std::printf("\n");
    }

    // std::printf("======\n");
    // tree.print_tree();
}
