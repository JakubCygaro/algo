#include <graph.hpp>
#include <common.hpp>
#include <iostream>
#include <format>
#include <ostream>
#include <print>
#include <unordered_map>

struct EdgeData : public gr::DijkstraEdge {
    EdgeData(decltype(gr::DijkstraEdge::dijkstra_score) s) : gr::DijkstraEdge(s) {}
    EdgeData() {}
};
struct NodeData : public gr::Graph<NodeData, EdgeData>::DijkstraData {
    char name{};
    NodeData(char n) : name(n) {}
    NodeData(){}
};

struct SCCGroupData {
    using node_t = gr::Graph<NodeData, EdgeData>::node_t;

    std::vector<node_t*> nodes{};

    /// SCC this one connects to
    std::vector<SCCGroupData*> connections{};
};

void test_dijkstra(bool broken = false) {
    auto graph = gr::Graph<NodeData, EdgeData>();
    using node_t = decltype(graph)::node_t;
    using edge_t = decltype(graph)::edge_t;

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
                .node_data = i + 'a'
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
                //make the scores super large so they are sure to be exlucded from the shortest path
                .edge_data = { static_cast<std::size_t>(common::get_random_in_range(100, 1000)) }
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
                if(common::get_random_in_range(0, 100) <= 60 || ensure_one_conn) {
                    ensure_one_conn = false;
                    auto peer = conn->nodes[common::get_random_in_range(0, conn->nodes.size()) % conn->nodes.size()];
                    auto edge = edge_t {
                        .tail = node,
                        .head = peer,
                        //make the scores super large so they are sure to be exlucded from the shortest path
                        .edge_data = { static_cast<std::size_t>(common::get_random_in_range(100, 1000)) }
                    };
                    graph.edges.push_back(edge);
                    auto edge_p = &graph.edges.back();
                    node->edges.push_back(edge_p);
                    peer->edges.push_back(edge_p);
                }
            }
        }
    }
    std::unordered_map<node_t*, bool> check{};
    for(auto& node : graph.nodes) {
        check[&node] = true;
    }
    for(auto& node : graph.nodes) {
        for(auto& edge : node.edges) {
            assert(check[edge->head]);
            assert(check[edge->tail]);
        }
    }
    //select random node as the start of the path
    auto start_i = common::get_random_in_range(0, graph.nodes.size() - 1);
    node_t* start = nullptr;
    for (auto it = graph.nodes.begin(); start_i >= 0; start_i--){
        start = &(*it);
    }
    std::vector<node_t*> path{};
    node_t* v = start;
    node_t* end;
    start->node_data.name = 'S';
    std::println("pathgen");
    if(broken){
        std::println("broken path variant");
        start = groups.front().nodes.front();
        path.push_back(start);
        end = groups.back().nodes.back();
        path.push_back(end);
    } else {
        while(v){
            //push the node to the path stack
            path.push_back(v);
            //mark as explored
            v->node_data.explored = true;
            if(v->edges.empty()) break;
            for(edge_t* e : v->edges){
                //random walk through the connected nodes
                if(common::get_random_in_range(1, 100) <= 80 && !e->head->node_data.explored) {
                    //set a small dijkstra score for the connecting edge so it is included in the path
                    e->edge_data.dijkstra_score = common::get_random_in_range(1, 10);
                    v = e->head;
                    break;
                } else {
                    v = nullptr;
                }
            }
        }
        end = path.back();
    }
    end->node_data.name = 'X';

    std::println("path");
    for(auto x : path){
        std::cout << std::format("{} [ {} ] ", x->node_data.name, x->node_data.len) << std::endl;
    }
    std::println();

    std::println("dijkstra");
    gr::dijkstra(graph, start);
    std::ranges::for_each(graph.nodes, [&](node_t& n){
        std::println("{} [ {} ]", n.node_data.name, n.node_data.len);
    });
    std::println();
    auto dijkstra_backup = graph.nodes;

    std::println("dijkstra_shortest_path");
    auto d_sh_p = gr::dijkstra_shortest_path(graph, start, end);
    for(auto x : d_sh_p){
        std::cout << std::format("{} [ {} ] ", x->node_data.name, x->node_data.len) << std::endl;
    }
    std::println();

    std::println("dijkstra_h");
    std::flush(std::cout);
    gr::dijkstra_h(graph, start);
    std::ranges::for_each(graph.nodes, [&](node_t& n){
        std::println("{} [ {} ]", n.node_data.name, n.node_data.len);
    });
    std::println();
    std::flush(std::cout);
    auto dijkstra_h_backup = graph.nodes;

    auto d_it = dijkstra_backup.begin();
    auto d_h_it = dijkstra_h_backup.begin();
    for(; d_it != dijkstra_backup.end() && d_h_it != dijkstra_h_backup.end() && !broken;){
        assert(d_it->node_data.len == d_h_it->node_data.len && "differing results");
        d_it++;
        d_h_it++;
    }

    std::println("dijkstra_shortest_path_h");
    auto d_sh_p_h = gr::dijkstra_shortest_path_h(graph, start, end);
    for(auto x : d_sh_p_h){
        std::cout << std::format("{} [ {} ] ", x->node_data.name, x->node_data.len) << std::endl;
    }
    std::println();
    std::flush(std::cout);

    if(broken && path.size() != 1){
        assert(d_sh_p.size() == 0 && "dijkstra_shortest_path was not empty for broken path variant");
        assert(d_sh_p_h.size() == 0 && "dijkstra_shortest_path_h was not empty for broken path variant");
        return;
    }
    std::ranges::reverse(path);
    for(std::size_t i = 0; i < path.size(); i++){
        assert(path[i] == d_sh_p[i] && "dijkstra_shortest_path incorrect");
        assert(path[i] == d_sh_p_h[i] && "dijkstra_shortest_path_h incorrect");
    }

}

int main(void) {
    test_dijkstra();
    // test_dijkstra(true);
}
