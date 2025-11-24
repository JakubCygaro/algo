#include <algorithm>
#include <graph.hpp>
#include <print>

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

template<typename T>
T add_with_no_over_under_flow(const T& a, const T& b){
    constexpr const T PLUS_INF = std::numeric_limits<T>::max();
    constexpr const T MINUS_INF = std::numeric_limits<T>::min();
    if(b > 0 && a > PLUS_INF - b) return PLUS_INF;
    if(b < 0 && a > PLUS_INF - b) return MINUS_INF;
    return a + b;
}

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
                // auto tmp = cache[k - 1][k][w] == INF || cache[k - 1][v][k] == INF ? INF : cache[k - 1][v][k] + cache[k - 1][k][w];
                auto tmp = add_with_no_over_under_flow<int>(cache[k - 1][v][k], cache[k - 1][k][w]);
                cache[k][v][w] = std::min(
                    cache[k - 1][v][w],
                    tmp);
                std::println("cache[k][v][w] = std::min("
                             "{},"
                             "{} + {} <{}>",
                cache[k - 1][v][w], cache[k - 1][v][k], cache[k - 1][k][w], tmp);
            }
        }
    }
    size_t src {}, dst {2};
    for (auto v = 0ul; v < N; v++) {
        if (cache[N][v][v] < 0) {
            assert(false && "negative cycle");
        }
    }
    auto sol = cache[N][src][dst];
    std::println("{}", sol);
}
