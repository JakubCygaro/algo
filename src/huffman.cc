#include "datatypes.hpp"
#include <algorithm>
#include <cstddef>
#include <print>
#include <string>
#include <vector>

using prefix_t = std::string;
using encode_map_t = dt::HashMap<char, prefix_t>;
using decode_map_t = dt::HashMap<prefix_t, char>;

namespace dt {
template <>
struct Hash<char> {
    std::size_t operator()(const char& t) const noexcept
    {
        return static_cast<std::size_t>(t);
    }
};
template <>
struct Hash<prefix_t> {
    std::size_t operator()(const prefix_t& s) const noexcept
    {
        const char* str = s.c_str();
        unsigned long hash = 5381;
        int c;

        while ((c = *str++))
            hash = ((hash << 5) + hash) + c;

        return hash;
    }
};
}
struct Node {
    static constexpr const char INTERNAL_NODE_SYMBOL = -1;
    char symbol = 0;
    float freq = 0.0;
    Node* parent = nullptr;
    Node* left = nullptr;
    Node* right = nullptr;

    Node(char s, float f)
        : symbol(s)
        , freq(f)
    {
    }
    Node()
        : symbol(INTERNAL_NODE_SYMBOL) { };
};

void make_prefix_map(Node* node, prefix_t prefix, encode_map_t& map)
{
    if (!node)
        return;
    if (node->symbol != Node::INTERNAL_NODE_SYMBOL) {
        map.insert({ node->symbol, std::move(prefix) });
    }
    make_prefix_map(node->right, prefix + "1", map);
    make_prefix_map(node->left, prefix + "0", map);
}

int main(void)
{

    std::vector<Node*> input = {
        new Node('a', 3),
        new Node('b', 2),
        new Node('c', 6),
        new Node('d', 8),
        new Node('e', 2),
        new Node('f', 6)
    };
    std::vector<Node*> forest = input;

    while (forest.size() >= 2) {
        auto min = std::min_element(forest.begin(), forest.end(), [](Node*& node_a, Node*& node_b) {
            return node_a->freq < node_b->freq;
        });
        Node* t1 = *min;
        forest.erase(min, min + 1);
        min = std::min_element(forest.begin(), forest.end(), [](Node*& node_a, Node*& node_b) {
            return node_a->freq < node_b->freq;
        });
        Node* t2 = *min;
        forest.erase(min, min + 1);

        Node* internal = new Node();

        internal->left = t1;
        internal->right = t2;

        internal->freq = t1->freq + t2->freq;

        forest.push_back(internal);
    }
    encode_map_t encode_map;
    decode_map_t decode_map;
    make_prefix_map(forest[0], std::string {}, encode_map);

    for (auto elem : input) {
        assert(encode_map.contains(elem->symbol));
        std::println("{} -> {}", elem->symbol, *encode_map.find(elem->symbol));
        decode_map.insert({ *encode_map.find(elem->symbol), std::move(elem->symbol) });
    }

    std::string text = "afbedc";
    std::string encoded;

    for (auto it = text.begin(); it != text.end(); it++) {
        encoded += *encode_map.find(*it);
    }

    std::println("{}", encoded);

    std::string decoded;
    std::string buf;
    for (auto it = encoded.begin(); it != encoded.end(); it++) {
        buf += *it;
        auto c = decode_map.find(buf);
        if(c) {
            decoded += c;
            buf.clear();
        }
    }
    std::println("{}", decoded);

    std::for_each(input.begin(), input.end(), [](auto n) { delete n; });


    // test suite idea
    // generate random alphabet
    // generate codes
    // generate random strings with the alphabet
    // encode and then decode
    // verify
}
