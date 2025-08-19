#include <graph.hpp>
#include <common.hpp>
#include <iostream>
#include <format>
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

void test_dijkstra() {
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
    std::stack<node_t*> path{};
    node_t* v = start;
    while(v){
        //push the node to the path stack
        path.push(v);
        //mark as explored
        v->node_data.explored = true;
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
    node_t* end = path.top();

    std::println("path");
    while(!path.empty()){
        auto x = path.top();
        path.pop();
        std::cout << std::format("{} [ {} ] ", x->node_data.name, x->node_data.len) << std::endl;
    }
    std::println();

    gr::dijkstra(graph, start);
    std::ranges::for_each(graph.nodes, [&](node_t& n){
        std::println("{} [ {} ]", n.node_data.name, n.node_data.len);
    });
    std::println();

    auto d_path = gr::dijkstra_shortest_path(graph, start, end);
    std::println("d_path");
    while(!d_path.empty()){
        auto x = d_path.top();
        d_path.pop();
        assert(x && "x was null");
        std::cout << std::format("{} [ {} ] ", x->node_data.name, x->node_data.len) << std::endl;
    }
    std::println();

    gr::dijkstra_h(graph, start);
    std::ranges::for_each(graph.nodes, [&](node_t& n){
        std::println("{} [ {} ]", n.node_data.name, n.node_data.len);
    });
    std::println();

    d_path = gr::dijkstra_shortest_path_h(graph, start, end);
    std::println("dh_path");
    while(!d_path.empty()){
        auto x = d_path.top();
        d_path.pop();
        std::cout << std::format("{} [ {} ] ", x->node_data.name, x->node_data.len) << std::endl;
    }
    std::println();
}

int main(void) {
    // using graph_t = gr::Graph<NodeData, EdgeData>;
    // using mat_t = graph_t::vmatrix_e;
    //
    // mat_t mtx = {{
    //     {'s', { {0, {}}, {1, 1}, {1, 6}, {0, {}}, {0, {}} }},
    //     {'v', { {0, {}}, {0, {}}, {1, 2}, {1, 8}, {1, 10} }},
    //     {'t', { {0, {}}, {0, {}}, {0, {}}, {0, {}}, {1, 3} }},
    //     {'x', { {0, {}}, {0, {}}, {0, {}}, {0, {}}, {0, {}} }},
    //     {'w', { {0, {}}, {0, {}}, {0, {}}, {1, 3}, {0, {}} }},
    // }};
    //
    // graph_t graph = graph_t::from_matrix(mtx);
    // auto start = &graph.nodes.front();
    // auto end = &graph.nodes.back();
    // std::cout << std::format(" start: {}, end: {}", start->node_data.name, end->node_data.name) << std::endl;
    // auto path = gr::dijkstra_shortest_path<NodeData, EdgeData>(graph, start, end);
    // while(!path.empty()) {
    //     auto v = path.top();
    //     path.pop();
    //     std::cout << std::format("{} [ {} ] ", v->node_data.name, v->node_data.len) << std::endl;
    // }
    // std::println();
    // gr::dijkstra(graph, start);
    // for(auto& node : graph.nodes) {
    //     std::println("{} [ {} ]", node.node_data.name, node.node_data.len);
    // }
    // gr::dijkstra_h<NodeData, EdgeData>(graph, start);
    // std::println();
    // for(auto& node : graph.nodes) {
    //     std::println("{} [ {} ]", node.node_data.name, node.node_data.len);
    // }
    // path = gr::dijkstra_shortest_path_h<NodeData, EdgeData>(graph, start, end);
    // std::println();
    // while(!path.empty()) {
    //     auto v = path.top();
    //     path.pop();
    //     std::cout << std::format("{} [ {} ] ", v->node_data.name, v->node_data.len) << std::endl;
    // }
    test_dijkstra();
}
