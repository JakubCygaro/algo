#include <algorithm>
#include <cassert>
#include <print>
#include <vector>
struct Item {
    size_t val {};
    size_t sz {};
};
int main(void)
{
    const int capacity = 6;
    Item items[] = { { 3, 4 }, { 2, 3 }, { 4, 2 }, { 4, 3 } };
    constexpr const size_t sz = (sizeof items / sizeof(Item));
    std::vector<std::vector<decltype(Item::val)>> cache;
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

    std::vector<size_t> path;

    size_t c = capacity;
    int i = sz;
    for (; i >= 1; i--) {
        if (items[i - 1].sz <= c && cache[i - 1][c - items[i - 1].sz] + items[i - 1].val >= cache[i - 1][c]) {
            path.push_back(i - 1);
            c = c - items[i - 1].sz;
        }
    }
    std::println("{}", path);
}
