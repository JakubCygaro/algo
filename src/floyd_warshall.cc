#include <algorithm>
#include <graph.hpp>
#include <print>
#include <unordered_map>
#include <utility>


struct FWEdge {
    int len {};
    FWEdge(int l)
        : len(l)
    {
    }
    FWEdge() { };
};
struct FWNode {
    char name {};
    gr::Graph<FWNode, FWEdge>::Node* pred {};
    FWNode(char n)
        : name(n) { };
    FWNode() { };
};

using graph_t = gr::Graph<FWNode, FWEdge>;
using edge_t = graph_t::edge_t;
using node_t = graph_t::node_t;

template <>
struct std::hash<std::pair<node_t*, node_t*>> {
    std::size_t operator()(const std::pair<node_t*, node_t*>& val) const noexcept {
        return std::hash<size_t>{}(reinterpret_cast<size_t>(std::get<0>(val)) - reinterpret_cast<size_t>(std::get<0>(val)));
    }
};

template<typename T>
T add_with_no_over_under_flow(const T& a, const T& b){
    constexpr const T PLUS_INF = std::numeric_limits<T>::max();
    constexpr const T MINUS_INF = std::numeric_limits<T>::min();
    T sum = a + b;
    if(a > 0 && b > 0 && sum < 0) return PLUS_INF;
    if(a < 0 && b < 0 && sum > 0) return MINUS_INF;
    return sum;
}

int main(void)
{
    graph_t::vmatrix_e mtx = { {
        { 's', { { 0, 0 }, { 1, 4 }, { 1, 2 }, { 0, 0 }, { 0, 0 } } },
        { 'v', { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 1, 4 } } },
        { 'u', { { 0, 0 }, { 1, -1 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } },
        { 'w', { { 0, 0 }, { 0, 0 }, { 1, 10 }, { 0, 0 }, { 0, 0 } } },
        { 't', { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 1, 2 }, { 0, 0 } } },
    } };

    auto graph = graph_t::from_matrix(mtx);

    const auto N = graph.nodes.size();
    constexpr auto INF = std::numeric_limits<int>::max();
    std::unordered_map<const node_t*, size_t> node_index_map(N);
    using vertex_pair_t = std::pair<node_t*, node_t*>;
    using path_t = std::vector<node_t*>;
    std::unordered_map<vertex_pair_t, path_t> paths{};
    std::vector<node_t*> nodes {};
    size_t count = 0;
    for (auto& v : graph.nodes) {
        nodes.push_back(&v);
        node_index_map[&v] = count++;
    }

    int cache[N + 1][N][N];
    for (auto i = 0ul; i < N + 1; i++) {
        for (auto j = 0ul; j < N; j++) {
            for (auto k = 0ul; k < N; k++) {
                cache[i][j][k] = INF;
            }
        }
    }
    // base case, k = 0
    for (auto v = 0ul; v < N; v++) {
        for (auto w = 0ul; w < N; w++) {
            if (v == w) {
                cache[0][v][w] = 0;
            } else if (auto f = std::find_if(graph.edges.begin(), graph.edges.end(), [&](edge_t& e) {
                           return e.tail == nodes[v] && e.head == nodes[w];
                       });
                f != graph.edges.end()) {
                cache[0][v][w] = f->edge_data.len;
            } else {
                cache[0][v][w] = INF;
            }
        }
    }
    for (auto k = 1ul; k <= N; k++) {
        for (auto v = 0ul; v <= N; v++) {
            for (auto w = 0ul; w <= N; w++) {
                auto _1 = cache[k - 1][v][w];
                auto _2 = add_with_no_over_under_flow<int>(cache[k - 1][v][k], cache[k - 1][k][w]);
                if (_2 < _1){
                    cache[k][v][w] = _2;
                    if(k < N && w < N){
                        auto hop = vertex_pair_t(nodes[k], nodes[w]);
                        std::println("{} -> {}", nodes[k]->node_data.name, nodes[w]->node_data.name);
                        if(paths.contains(hop)){
                            auto& p = paths[hop];
                            p.push_back(nodes[v]);
                        } else {
                            paths[hop] = path_t{ nodes[v] };
                        }
                    }
                } else {
                    cache[k][v][w] = _1;
                }
            }
        }
    }
    size_t src {}, dst {3};
    for (auto v = 0ul; v < N; v++) {
        if (cache[N][v][v] < 0) {
            assert(false && "negative cycle");
        }
    }
    auto sol = cache[N][src][dst];
    std::println("{}", sol);
    auto& p = paths[vertex_pair_t(nodes[src], nodes[dst])];
    std::ranges::for_each(p, [](node_t* n){
            std::println("{}", n->node_data.name);
            });
}
