#include <algorithm>
#include <cassert>
#include <vector>
#include <print>
struct Item {
    double val {};
    size_t sz {};
};
int main(void)
{
    const int capacity = 10;
    Item items[] = { { 3.0, 4 }, { 2.0, 3 }, { 4.0, 2 }, { 4.0, 3 } };
    constexpr const size_t sz = (sizeof items / sizeof(Item));
    std::vector<std::vector<double>> cache;
    cache.resize(sz + 1);
    for (auto& c : cache) {
        c.resize(capacity + 1);
    }

    for (auto& c : cache[0]) {
        c = 0;
    }

    for (size_t i = 1; i <= sz; i++) {
        for (size_t c = 0; c < capacity + 1; c++) {
            assert(i < sz + 1);
            assert(c < capacity + 1);
            if (items[i - 1].sz > c) {
                cache[i][c] = cache[i - 1][c];
            } else {
                cache[i][c] = std::max(
                    cache[i - 1][c],
                    cache[i - 1][c - items[i - 1].sz] + items[i - 1].val);
            }
        }
    }
    auto solution = cache.back().back();
    std::println("{}", cache);
    std::println("{}", solution);
}
