#include "common.hpp"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <print>
#include <string>
#include <unordered_map>
#include <utility>
constexpr int GAP_PENALTY = 20;

typedef const char* str;

namespace std {
    template <>
    struct hash<std::pair<char, char>> {
        std::size_t operator()(const std::pair<char, char>& val) const noexcept
        {
            return hash<int> {}(std::get<0>(val) - std::get<1>(val));
        }
    };
}

std::unordered_map<std::pair<char, char>, int> penalty_map = {
    { { 'A', 'C' }, 10 },
    { { 'A', 'G' }, 11 },
    { { 'A', 'T' }, 12 },
    { { 'G', 'C' }, 13 },
    { { 'G', 'T' }, 14 },
    { { 'C', 'T' }, 15 },
};

const char ALPHABET[] = {
    'A', 'C', 'G', 'T'
};
std::unordered_map<char, size_t> alphabet_index = {
    {'A', 0 },
    {'C', 1 },
    {'G', 2 },
    {'T', 3 },
};

std::string mutate_string(const std::string& in) {
    auto out = in;
    for(size_t i = 0; i < out.size(); i++){
        if(common::get_random_in_range(1, 100) <= 75){
            out[i] = ALPHABET[(alphabet_index[out[i]] + sizeof(ALPHABET) - 1) % sizeof(ALPHABET)];
        } else {
            out[i] = ' ';
        }
    }
    auto b = std::remove_if(out.begin(), out.end(), isspace);
    out.erase(b, out.end());
    return out;
}
std::string make_random_string() {
    std::string out;
    for(auto i = 0; i < common::get_random_in_range(5, 15); i++){
        out += ALPHABET[common::get_random_in_range(0, sizeof(ALPHABET) - 1)];
    }
    return out;
}

int get_penalty_for(char first, char second)
{
    if (penalty_map.contains({ first, second })) {
        return penalty_map[{ first, second }];
    } else if (penalty_map.contains({ second, first })) {
        return penalty_map[{ second, first }];
    } else {
        return 0;
    }
}

int nw_score(str a, size_t a_sz, str b, size_t b_sz)
{
    int cache[a_sz + 1][b_sz + 1];
    for (size_t i = 0; i <= a_sz; i++)
        cache[i][0] = i * GAP_PENALTY;
    for (size_t j = 0; j <= b_sz; j++)
        cache[0][j] = j * GAP_PENALTY;

    for (size_t i = 1; i <= a_sz; i++) {
        for (size_t j = 1; j <= b_sz; j++) {
            auto one = cache[i - 1][j - 1] + get_penalty_for(a[i - 1], b[j - 1]);
            auto two = cache[i - 1][j] + GAP_PENALTY;
            auto three = cache[i][j - 1] + GAP_PENALTY;
            cache[i][j] = std::min(
                std::min(
                    one, two),
                three);
        }
    }
    return cache[a_sz][b_sz];
}

int main(void)
{
    for (int i = 0; i < 20; i++){
        std::string a = make_random_string();
        std::string b = mutate_string(a);
        std::string c = mutate_string(b);
        auto score_a_b = nw_score(a.data(), std::strlen(a.data()), b.data(), std::strlen(b.data()));
        auto score_b_c = nw_score(b.data(), std::strlen(b.data()), c.data(), std::strlen(c.data()));
        auto score_a_c = nw_score(a.data(), std::strlen(a.data()), c.data(), std::strlen(c.data()));
        std::println("a {}\nb {}\nc {}", a, b, c);
        std::println("a - b {}\nb - c {}\na - c {}", score_a_b, score_b_c, score_a_c);
    }
}
