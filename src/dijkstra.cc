#include <cstdio>
#include <graph.hpp>
#include <iostream>
#include <format>

struct EdgeData : public gr::DijkstraEdge {
    EdgeData(decltype(gr::DijkstraEdge::dijkstra_score) s) : gr::DijkstraEdge(s) {}
    EdgeData() {}
};
struct NodeData : public gr::Graph<NodeData, EdgeData>::DijkstraData {
    char name{};
    NodeData(char n) : name(n) {}
    NodeData(){}
};

int main(void) {
    using graph_t = gr::Graph<NodeData, EdgeData>;
    using mat_t = graph_t::vmatrix_e;

    mat_t mtx = {{
        {'s', { {0, {}}, {1, 1}, {1, 6}, {0, {}}, {0, {}} }},
        {'v', { {0, {}}, {0, {}}, {1, 2}, {1, 8}, {1, 10} }},
        {'w', { {0, {}}, {0, {}}, {0, {}}, {1, 3}, {0, {}} }},
        {'t', { {0, {}}, {0, {}}, {0, {}}, {0, {}}, {1, 3} }},
        {'x', { {0, {}}, {0, {}}, {0, {}}, {0, {}}, {0, {}} }},
    }};

    graph_t graph = graph_t::from_matrix(mtx);
    auto start = &graph.nodes.front();
    auto end = &graph.nodes.back();
    std::cout << std::format(" start: {}, end: {}", start->node_data.name, end->node_data.name) << std::endl;
    auto path = gr::dijkstra_shortest_path<NodeData, EdgeData>(graph, start, end);
    while(!path.empty()) {
        auto v = path.top();
        path.pop();
        std::cout << std::format("{} [ {} ] ", v->node_data.name, v->node_data.len) << std::endl;
    }
}
