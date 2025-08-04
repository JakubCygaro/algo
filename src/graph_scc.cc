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
    using edge_t = gr::Graph<GraphData>::edge_t;

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
        // connect them into a cycle
        for(auto i = 0; i < group_size; i++) {
            auto node = group.nodes[i];
            auto peer_node = group.nodes[ (i + 1) % group_size ];
            if(node == peer_node) break;
            auto edge = edge_t {
                .tail = node,
                .head = peer_node,
            };
            graph.edges.push_back(edge);
            auto edge_p = &graph.edges.back();
            node->edges.push_back(edge_p);
            peer_node->edges.push_back(edge_p);
        }
    }
    //generate connections for groups
    for(auto i = 0; i < group_n; i++) {
        auto group = &groups[i];
        for (auto j = i + 1; j < group_n; j++) {
            // 30% chance to connect with any of the subsequent groups
            if(common::get_random_in_range(0, 100) <= 30 ) {
                group->connections.push_back(&groups[j]);
            }
        }
    }
    // actually connect them
    for(auto& group : groups) {
        for (auto conn : group.connections) {
            auto ensure_one_conn = true;
            for (auto node : group.nodes) {
                //each node gets a random chance to connect to a random node from the peer group
                if(common::get_random_in_range(0, 100) <= 30 || ensure_one_conn) {
                    ensure_one_conn = false;
                    auto peer = conn->nodes[common::get_random_in_range(0, conn->nodes.size()) % conn->nodes.size()];
                    auto edge = edge_t {
                        .tail = node,
                        .head = peer
                    };
                    graph.edges.push_back(edge);
                    auto edge_p = &graph.edges.back();
                    node->edges.push_back(edge_p);
                    peer->edges.push_back(edge_p);
                }
            }
        }
    }
    // run gr::strongly_connected on the graph
    gr::strongly_connected(graph);
    //
    // test if assigned groups match those in metadata
    for(auto& group : groups) {
        auto scc_n = group.nodes[0]->node_data.scc_n;
        auto res = std::all_of(group.nodes.begin(), group.nodes.end(), [=](node_t* node_ptr){
            return node_ptr->node_data.scc_n == scc_n;
        });
        auto error_msg = std::format("nodes of group not in the same scc");
        assert(res && error_msg.c_str());
    }
}

int main(void) {
//     gr::Graph<GraphData>::vmatrix mtx = {{
// //              1  2  3  4  5  6  7  8  9  10 11
//         {1, { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 }},
//         {2, { 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0 }},
//         {3, { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 }},
//         {4, { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 }},
//         {5, { 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0 }},
//         {6, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 }},
//         {7, { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 }},
//         {8, { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 }},
//         {9, { 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0 }},
//         {10, { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 }},
//         {11, { 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0 }},
//     }};
//
//     auto graph = gr::Graph<GraphData>::from_matrix(mtx);
//
//     gr::strongly_connected(graph);
//
//     std::for_each(graph.nodes.begin(), graph.nodes.end(), [&](gr::Graph<GraphData>::node_t& gd){
//         std::cout <<
//             std::format("[ name = {}, explored = {}, f_val = {}, scc_n = {} ]", gd.node_data.name, gd.node_data.explored, gd.node_data.f_value, gd.node_data.scc_n)
//             << std::endl;

        // for(auto e : gd.edges) {
        //     std::cout <<
        //         std::format("\t[ {} -> {} ]", e->tail->node_data.name, e->head->node_data.name)
        //         << std::endl;
        // }
    // });
    for(auto i = 0; i < 100; i++)
        test_scc();
}
