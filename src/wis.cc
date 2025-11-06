#include "common.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <print>
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

    constexpr const int base = 100;
    int ans = 0;
    std::vector<int> nodes;
    for (auto i = 0; i < sz; i++) {
        nodes.push_back(common::get_random_in_range(0, 80));
    }
    for (auto i = 0; i < sz;){
        if (common::get_random_in_range(0, 100) >= 40){
            nodes[i] += base;
            ans += nodes[i];
            i = i + 2;
        } else {
            nodes[i] /= 2;
            i = i + 1;
        }
    }

    return { nodes, ans };
}

void test_mwis(void){
    auto [n, ans] = gen_nodes_with_answer();
    auto [guess_path, guess_ans] = mwis(n);
    if(n.size() == 0){
        assert(guess_path.size() == 0);
    } else {
        assert(ans <= guess_ans);
        auto ans_from_path = 0;
        size_t prev = guess_path.front() + 2;
        for(size_t step : guess_path){
            assert(step - prev > 1);
            prev = step;
            ans_from_path += n[step];
        }
        assert(ans_from_path == guess_ans);
    }
}
int main(void)
{
    for(auto i = 0; i < 100; i++)
        test_mwis();
    return 0;
}
