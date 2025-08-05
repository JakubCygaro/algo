#include "graph.hpp"
#include <algorithm>
#include <common.hpp>
#include <random>

struct GraphData : public gr::TopoSortableGraphData {
    char name{};
    size_t asssigned_f_val{};

    GraphData(char n) : name(n) {}
    GraphData(){}
};

void test_topo() {
    using graph_t = gr::Graph<GraphData>;
    using node_t = graph_t::node_t;
    using edge_t = graph_t::edge_t;
    auto graph = graph_t();
    // generate random number that will determine how many nodes are created
    auto node_n = common::get_random_in_range(2, 20);
    std::vector<node_t*> nodes(node_n);
    for(auto i = 0; i < node_n; i++) {
        auto new_node = node_t {
            .edges = {},
            .node_data = 'A' + i
        };
        graph.nodes.push_back(new_node);
        nodes[i] = &graph.nodes.back();
    }

    std::shuffle(nodes.begin(), nodes.end(), std::mt19937{std::random_device{}()});

    //going from the last one to the first one
    for(auto order = node_n - 1; order > 0; order--) {
        auto* node = nodes[order];
        node->node_data.asssigned_f_val = order;

        auto random_to_connect = common::get_random_in_range(0, order - 1);
        auto peer = nodes[random_to_connect];
        auto edge = edge_t {
            .tail = peer,
            .head = node,
        };
        graph.edges.push_back(edge);
        auto edge_ptr = &graph.edges.back();
        node->edges.push_back(edge_ptr);
        peer->edges.push_back(edge_ptr);
        //visit every preceeding node
        for(auto i = order - 1; i >= 0; i--) {
            if(common::get_random_in_range(1, 100) <= 15 && i != random_to_connect) {
                peer = nodes[i];
                edge = edge_t {
                    .tail = peer,
                    .head = node,
                };
                graph.edges.push_back(edge);
                edge_ptr = &graph.edges.back();
                node->edges.push_back(edge_ptr);
                peer->edges.push_back(edge_ptr);
            }
        }
    }

    gr::topo_sort(graph);

    for(auto& node : graph.nodes) {
        std::size_t parent_max_f_val = 0;
        for(auto* edge : node.edges) {
            if(edge->head == &node) {
                parent_max_f_val = std::max(edge->tail->node_data.f_value, parent_max_f_val);
            }
        }
        assert(parent_max_f_val <= node.node_data.f_value && "node parent f-value not lower than child");
    }
}

int main(void) {
    for(auto i = 0; i < 100; i++)
        test_topo();
}
