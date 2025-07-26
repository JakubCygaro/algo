#include "graph.hpp"
#include <iostream>
#include <format>
#include <array>
#include <ostream>
#include <stack>
#include <tuple>
#include "common.hpp"
#include <type_traits>

struct GraphData {
    char name{};
    bool explored{};

    GraphData(char n) : name(n) {}
    GraphData(){}

    decltype(std::cout)& operator<<(decltype(std::cout)& os) {
        os << std::format("[ {} = {} ]", this->name, this->explored);
        return os;
    }
    decltype(std::cout)& operator>>(decltype(std::cout)& os) {
        os << this;
        return os;
    }
};

void dpf(const Graph<GraphData>& graph, Graph<GraphData>::node_t* start) {
    std::stack<Graph<GraphData>::node_t*> nodes{};
    nodes.push(start);

    while(!nodes.empty()) {
        auto v = nodes.top();
        nodes.pop();
        if (!v->node_data.explored) {
            v->node_data.explored = true;
            for(auto& edge : v->edges) {
                nodes.push(edge->head);
            }
        }
    }
}
void test_if_connected() {
    const auto n = common::get_random_in_range(3, 50);

    Graph<GraphData>::vmatrix matrix(n);
}
int main(void) {
    std::array<std::tuple<GraphData, std::array<int, 4>>, 4> matrix = {{
        {'1', { 0, 1, 0, 0 }},
        {'2', { 1, 0, 1, 1 }},
        {'3', { 0, 1, 0, 1 }},
        {'4', { 0, 1, 1, 0 }}
        }};
    auto gr = Graph<GraphData>::from_matrix<4>(matrix);

    for (auto& node : gr.nodes) {
        std::cout << std::format("[ {} = {} ]", node.node_data.name, node.node_data.explored) << std::endl;
        for (auto& edge : node.edges) {
            std::cout << std::format("\t[ {} -> {} ]", edge->tail->node_data.name, edge->head->node_data.name) << std::endl;
        }
    }

    dpf(gr, &gr.nodes.front());

    for (auto& node : gr.nodes) {
        std::cout << std::format("[ {} = {} ]", node.node_data.name, node.node_data.explored) << std::endl;
    }
}
