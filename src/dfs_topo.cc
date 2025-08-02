#include "graph.hpp"
#include <format>
#include <iostream>

struct GraphData : public gr::TopoSortableGraphData {
    char name{};

    GraphData(char n) : name(n) {}
    GraphData(){}
};

int main(void) {
    gr::Graph<GraphData>::vmatrix mat = {{
        //     s  v  w  t
        {'s', {0, 1, 1, 0}},
        {'v', {0, 0, 0, 1}},
        {'w', {0, 0, 0, 1}},
        {'t', {0, 0, 0, 0}},
    }};

    auto graph = gr::Graph<GraphData>::from_matrix(mat);

    gr::topo_sort<GraphData>(graph);

    for(auto& vertex : graph.nodes) {
        std::cout << std::format("[ name = {}, explored = {}, f = {} ]", vertex.node_data.name, vertex.node_data.explored, vertex.node_data.f_value) << std::endl;
        for(auto& edge : vertex.edges) {
            std::cout << std::format("\t[ {} -> {} ]", edge->tail->node_data.name, edge->head->node_data.name) << std::endl;
        }
    }
}
