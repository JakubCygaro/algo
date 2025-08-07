#include <graph.hpp>
#include <iostream>
#include <format>

struct NodeData : public gr::DijkstraData {
    char name{};
    NodeData(char n) : name(n) {}
    NodeData(){}
};
struct EdgeData : public gr::DijkstraEdge {
    EdgeData(decltype(gr::DijkstraEdge::dijkstra_score) s) : gr::DijkstraEdge(s) {}
    EdgeData() {}
};

int main(void) {
    using graph_t = gr::Graph<NodeData, EdgeData>;
    using mat_t = graph_t::vmatrix_e;

    mat_t mtx = {{
        {'s', { {0, {}}, {1, 1}, {1, 4}, {0, {}} }},
        {'v', { {0, {}}, {0, {}}, {1, 2}, {1, 6} }},
        {'w', { {0, {}}, {0, {}}, {0, {}}, {1, 3} }},
        {'t', { {0, {}}, {0, {}}, {0, {}}, {0, {}} }},
    }};

    graph_t graph = graph_t::from_matrix(mtx);
    auto start = &graph.nodes.front();
    auto end = &graph.nodes.back();
    std::cout << std::format(" start: {}, end: {}", start->node_data.name, end->node_data.name) << std::endl;
    auto const path = gr::dijkstra_shortest_path<NodeData, EdgeData>(graph, start, end);

    for (auto* v : path) {
        std::cout << std::format("{} [ {} ] ", v->node_data.name, v->node_data.len);
    }
    std::cout << std::endl;
}
