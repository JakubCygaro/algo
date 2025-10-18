#include "common.hpp"
#include "datatypes.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <format>
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
    bool visited = false;
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
namespace std {
    template<>
    struct formatter<Node> {
        constexpr auto parse(std::format_parse_context& ctx) {
            return ctx.begin();
        }
        auto format(const Node& node, std::format_context& ctx) const {
            return std::format_to(ctx.out(), "{{ '{}' {} {} }}", node.symbol, node.freq, node.visited ? "VISITED" : "UNVISITED");
        }
    };
}
void make_prefix_map(Node* node, prefix_t prefix, encode_map_t& map)
{
    if (!node)
        return;
    assert(!node->visited && "this node has already been visited");
    node->visited = true;
    if (node->symbol != Node::INTERNAL_NODE_SYMBOL) {
        map.insert({ node->symbol, std::move(prefix) });
    }
    if(node->right)
        make_prefix_map(node->right, prefix + "1", map);
    if(node->left)
        make_prefix_map(node->left, prefix + "0", map);
}

void test_huffman_code(){
    dt::HashMap<int, bool> alphabet_unique_guard{};
    std::vector<Node*> alphabet(common::get_random_in_range(5, 'Z' - 'A' - 5));
    for(size_t i = 0; i < alphabet.size(); i++){
        auto n = new Node(common::get_random_in_range((int)'A', (int)'Z'), common::get_random_in_range(1, 100));
        if(!alphabet_unique_guard.contains(static_cast<int>(n->symbol))){
            alphabet[i] = n;
            alphabet_unique_guard.insert(static_cast<int>(n->symbol), true);
        } else {
            i--;
        }
    }
    dt::MinHeap<float, Node*> forest;
    std::ranges::for_each(alphabet, [&](auto n) { forest.insert(n->freq, n); });

    while (forest.size() >= 2) {
        auto [f1, t1] = forest.extract();
        auto [f2, t2] = forest.extract();

        assert(t1 != t2 && "same node extracted twice");
        Node* internal = new Node();
        assert(internal != t1 && "internal node same as left child");
        assert(internal != t2 && "internal node same as right child");

        internal->left = t1;
        internal->right = t2;

        internal->freq = f1 + f2;

        forest.insert(internal->freq, internal);
    }
    encode_map_t encode_map;
    decode_map_t decode_map;
    make_prefix_map(std::get<1>(forest.extract()), std::string {}, encode_map);

    for (auto elem : alphabet) {
        assert(encode_map.contains(elem->symbol));
        decode_map.insert({ *encode_map.find(elem->symbol), std::move(elem->symbol) });
    }
    const auto text_len = common::get_random_in_range(5, 30);
    std::string text;
    for(auto i = 0; i < text_len; i++){
        text += alphabet[rand() % alphabet.size()]->symbol;
    }
    std::string encoded;

    for (auto it = text.begin(); it != text.end(); it++) {
        encoded += *encode_map.find(*it);
    }

    std::string decoded;
    std::string buf;
    for (auto it = encoded.begin(); it != encoded.end(); it++) {
        buf += *it;
        auto c = decode_map.find(buf);
        if(c) {
            decoded += *c;
            buf.clear();
        }
    }
    assert(decoded == text);
    std::for_each(alphabet.begin(), alphabet.end(), [](auto n) { delete n; });
}

int main(void)
{
    for(size_t i = 0; i < 100; i++){
        test_huffman_code();
    }
    return 0;
}

