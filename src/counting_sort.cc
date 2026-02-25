#include "common.hpp"
#include <algorithm>
#include <cassert>
#include <print>
#include <random>
#include <vector>
template <typename T>
using Vec = std::vector<T>;

void counting_sort(Vec<int>& a)
{
    if (a.size() == 0)
        return;
    Vec b = Vec<int>(a.size(), int(0));
    int max = a[0];
    for (const auto& elem : a) {
        max = std::max(max, elem);
    }
    Vec c = Vec<int>(max + 1, int(0));
    for (size_t j = 0; j < a.size(); j++) {
        c[a[j]]++;
    }
    for (size_t i = 1; i <= (size_t)max; i++) {
        c[i] = c[i] + c[i - 1];
    }
    for (size_t j = a.size(); j > 0; j--) {
        auto tmp = a[j - 1];
        b[c[tmp] - 1] = tmp;
        c[tmp] = c[tmp] - 1;
    }
    std::copy(b.begin(), b.end(), a.begin());
}

int main(void)
{
    for (auto i = 0; i < 1000; i++) {
        Vec in { common::get_random_in_range(100, 1000) };
        std::for_each(in.begin(), in.end(), [](int& elem) {
            elem = common::get_random_in_range(0, 200);
        });
        counting_sort(in);
        assert(std::is_sorted(in.begin(), in.end()) && "array was not sorted");
    }
    return 0;
}
