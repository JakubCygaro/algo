// #include "graph.hpp"
//
// using graph_t = gr::Graph<gr::NamedGraphData>;
// using node_t = graph_t::node_t;
// using edge_t = graph_t::edge_t;

#include <algorithm>
#include <cstddef>
#include <print>
#include <vector>
int main(void)
{
    const int in[] = { 1, 2, 5, 1, 5, 6 };
    constexpr const size_t in_sz = (sizeof(in) / sizeof(int));
    constexpr const size_t cache_sz = in_sz + 1;
    int cache[cache_sz] = {};

    cache[0] = 0;
    cache[1] = in[0];
    for (size_t i = 2; i < cache_sz; i++) {
        cache[i] = std::max(cache[i - 1], cache[i - 2] + in[i - 1]);
    }
    std::vector<int> solution {};
    int i = static_cast<int>(cache_sz) - 1;
    if (cache[i] + cache[i - 2] > cache[i - 1]) {
        solution.push_back(i - 1);
        i = i - 2;
    } else {
        i = i - 1;
    }
    for (;;) {
        solution.push_back(i - 1);
        i = cache[i] + cache[i - 2] > cache[i - 1] ? i - 2 : i - 1;
        if (i < 1) {
            break;
        }
    }

    std::println("{}", cache);
    std::println("{}", solution);
}
