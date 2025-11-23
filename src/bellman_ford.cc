#include <algorithm>
#include <graph.hpp>
#include <limits>
#include <print>
#include <string>
#include <unordered_map>

struct BFEdge {
    int len {};
    BFEdge(int l)
        : len(l)
    {
    }
    BFEdge() { };
};
struct BFNode {
    char name {};
    gr::Graph<BFNode, BFEdge>::Node * pred{};
    BFNode(char n)
        : name(n) { };
    BFNode() { };
};

using graph_t = gr::Graph<BFNode, BFEdge>;
using edge_t = graph_t::edge_t;
using node_t = graph_t::node_t;

int main(void)
{
    graph_t::vmatrix_e mtx = { {
        { 's', { { 0, 0 }, { 1, 4 }, { 1, 2 }, { 0, 0 }, { 0, 0 } } },
        { 'v', { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 1, 4 } } },
        { 'u', { { 0, 0 }, { 1, -1 }, { 0, 0 }, { 1, 2 }, { 0, 0 } } },
        { 'w', { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 1, 2 } } },
        { 't', { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } },
    } };

    auto graph = graph_t::from_matrix(mtx);

    const auto N = graph.nodes.size();
    constexpr auto INF = std::numeric_limits<int>::max();
    std::unordered_map<const node_t*, size_t> node_index_map(N);
    std::vector<node_t*> nodes {};
    size_t count = 0;
    for (auto& v : graph.nodes) {
        nodes.push_back(&v);
        node_index_map[&v] = count++;
    }

    auto s = 0ul;
    auto dest = 4ul;
    assert(nodes[s]->node_data.name == 's');

    int cache[N + 1][N];
    for (auto i = 0ul; i < N; i++) {
        for (auto j = 0ul; j < N; j++) {
            cache[i][j] = INF;
        }
    }
    cache[0][s] = 0;
    for (auto v = 0ul; v < N; v++) {
        if (v != s) {
            cache[0][v] = INF;
        }
    }
    std::optional<int> sol;
    for (auto i = 1ul; i <= N - 1; i++) {
        bool stable = true;
        for (auto v = 0ul; v < N; v++) {
            auto min = INF;
            for (const edge_t* e : nodes[v]->edges) {
                // incoming edge
                if (e->head == nodes[v]) {
                    node_t* w = e->tail;
                    auto prev = cache[i - 1][node_index_map[w]];
                    if (prev != INF && prev + e->edge_data.len < min) {
                        min = prev + e->edge_data.len;
                        nodes[v]->node_data.pred = w;
                    }
                }
            }
            cache[i][v] = std::min(cache[i - 1][v], min);
            if (cache[i][v] != cache[i - 1][v]) {
                stable = false;
            }
        }
        if (stable) {
            sol = cache[i - 1][dest];
            break;
        }
    }
    std::println("answer: {}", !sol.has_value() ? "<negative cycle>" : std::to_string(sol.value()));
    if (!sol)
        return 0;
    std::vector<node_t*> path {};
    path.push_back(nodes[dest]);
    auto pred = nodes[dest]->node_data.pred;
    while(pred) {
        path.push_back(pred);
        pred = pred->node_data.pred;
    }
    for(node_t* v : path){
        std::print("<- [ {} ] ", v->node_data.name);
    }
    std::println();
}
