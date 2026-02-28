#include <algorithm>
#include <cassert>
#include <common.hpp>
#include <cstddef>
#include <limits>
#include <print>
#include <vector>
int main(void)
{
    const std::size_t n = common::get_random_in_range(8, 24);
    std::vector<int> p(n + 1);
    std::ranges::for_each(p, [](auto& e) {
        e = common::get_random_in_range(1, 32);
    });
    p[0] = 0;
    std::vector<int> r(n + 1);
    r[0] = 0;
    std::vector<int> s(n + 1);
    std::println("{}", p);
    for (auto j = 1ul; j < r.size(); j++) {
        auto q = std::numeric_limits<int>::min();
        for (auto i = 1ul; i <= j; i++) {
            if (q < p[i] + r[j - i]){
                s[j] = i;
                q = p[i] + r[j - i];
            }
        }
        r[j] = q;
    }
    std::vector<int> sol;
    auto total_r = 0;
    for(auto len = (int)n; len > 0; ){
        sol.push_back(s[len]);
        len = len - s[len];
    }
    for(auto idx : sol){
        total_r += p[idx];
    }
    assert(total_r == r[n]);
}
