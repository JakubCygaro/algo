#include <algorithm>
#include <common.hpp>
#include <format>
#include <graph.hpp>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

struct GraphData : gr::SCCGraphData {
    int name{};

    GraphData(int n): name(std::move(n)) {}
    GraphData() {}
};

struct SCCGroupData {
    using node_t = gr::Graph<GraphData>::node_t;

    std::vector<node_t*> nodes{};

    /// SCC this one connects to
    std::vector<SCCGroupData*> connections{};
};

void test_scc() {
    auto graph = gr::Graph<GraphData>();
    using node_t = gr::Graph<GraphData>::node_t;

    // generate sc group metadata
    auto group_n = common::get_random_in_range(1, 10);
    std::vector<SCCGroupData> groups(group_n);
    // generate each group and their internal connections (a cycle)
    for(auto& group : groups) {
        auto group_size = common::get_random_in_range(1, 10);
        group.nodes.resize(group_size);
        // create nodes
        for(auto i = 0; i < group_size; i++) {
            auto node = node_t{
                .edges = {},
                .node_data = { i }
            };
            graph.nodes.push_back(node);
            group.nodes[i] = &graph.nodes.back();
        }
    }
    //
    //
    // connect the groups in established order
    //
    // run gr::strongly_connected on the graph
    //
    // test if assigned groups match those in metadata
}

int main(void) {
    gr::Graph<GraphData>::vmatrix mtx = {{
//              1  2  3  4  5  6  7  8  9  10 11
        {1, { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 }},
        {2, { 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0 }},
        {3, { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 }},
        {4, { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 }},
        {5, { 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0 }},
        {6, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 }},
        {7, { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 }},
        {8, { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 }},
        {9, { 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0 }},
        {10, { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }},
        {11, { 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0 }},
    }};

    auto graph = gr::Graph<GraphData>::from_matrix(mtx);

    gr::strongly_connected(graph);

    std::for_each(graph.nodes.begin(), graph.nodes.end(), [&](gr::Graph<GraphData>::node_t& gd){
        std::cout << 
            std::format("[ name = {}, explored = {}, f_val = {}, scc_n = {} ]", gd.node_data.name, gd.node_data.explored, gd.node_data.f_value, gd.node_data.scc_n) 
            << std::endl;

        // for(auto e : gd.edges) {
        //     std::cout << 
        //         std::format("\t[ {} -> {} ]", e->tail->node_data.name, e->head->node_data.name) 
        //         << std::endl;
        // }
    });
}
