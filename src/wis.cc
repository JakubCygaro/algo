#include "common.hpp"
#include <algorithm>
#include <cstddef>
#include <numeric>
#include <print>
#include <random>
#include <vector>

template <typename T>
std::pair<std::vector<size_t>, T> mwis(const std::vector<T>& in)
{
    if (in.size() == 0)
        return {};
    else if (in.size() == 1)
        return { { 0 }, in[0] };

    const size_t cache_sz = in.size() + 1;
    std::vector<T> cache(cache_sz);

    cache[0] = 0;
    cache[1] = in[0];
    for (size_t i = 2; i < cache_sz; i++) {
        cache[i] = std::max(cache[i - 1], cache[i - 2] + in[i - 1]);
    }
    std::vector<size_t> solution {};
    for (auto i = cache_sz - 1; i >= 2;) {
        if (cache[i - 1] >= cache[i - 2] + in[i - 1]) {
            i = i - 1;
        } else {
            solution.push_back(i - 1);
            i = i - 2;
        }
        if (i == 1) {
            solution.push_back(0);
        }
    }
    return { solution, cache[cache_sz - 1] };
}

std::pair<std::vector<int>, int> gen_nodes_with_answer()
{
    const auto sz = common::get_random_in_range(0, 25);
    if (!sz)
        return { {}, -1 };

    constexpr const int base = 1000;
    std::vector<int> nodes;
    for (auto i = 0; i < sz; i++) {
        nodes.push_back(base + common::get_random_in_range(0, 50));
    }
    int ans = std::accumulate(nodes.begin(), nodes.end(), 0);

    for (auto i = 0; i < common::get_random_in_range(10, 30); i++){
        nodes.push_back(common::get_random_in_range(1, 25));
    }

    std::shuffle(nodes.begin(), nodes.end(), std::default_random_engine());

    return { nodes, ans };
}

void test_mwis(void){
    auto [n, ans] = gen_nodes_with_answer();
    auto [guess_path, guess_ans] = mwis(n);
    std::println("{} {}", ans, guess_ans);
    assert(ans == guess_ans);
}
int main(void)
{
    test_mwis();
    // const std::vector<int> input = { 1, 2, 5, 1, 5, 6 };
    //
    // auto s = mwis(input);
    //
    // std::println("{}", input);
    // std::println("{}", s);

    return 0;
}
