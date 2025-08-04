#include "graph.hpp"
#include <functional>
#include <iostream>
#include <format>
#include <ostream>
#include "common.hpp"

using namespace gr;

struct GraphData : public gr::ExplorableGraphData {
    int id{};

    GraphData(int n) : id(n) {}
    GraphData(){}

    decltype(std::cout)& operator<<(decltype(std::cout)& os) {
        os << std::format("[ {} = {} ]", this->id, this->explored);
        return os;
    }
    decltype(std::cout)& operator>>(decltype(std::cout)& os) {
        os << this;
        return os;
    }
};

void test_if_connected(std::function<void(gr::Graph<GraphData>::node_t*)> check_conn_fn) {

    const auto vertex_n = common::get_random_in_range(3, 50);
    // how many vertices have to be connected
    const auto connected_v = common::get_random_in_range(1, vertex_n);

    gr::Graph<GraphData> graph{};

    using node_t = decltype(graph)::node_t;
    // keep track of all the vertices
    std::vector<node_t*> vertices(vertex_n);

    for (auto i = 0; i < vertex_n; i++) {
        node_t new_node = {
            .edges = {},
            .node_data = GraphData(i),
        };
        graph.nodes.emplace_back(new_node);
        // store this vertex
        vertices[i] = &graph.nodes.back();
    }
    // generate random connections
    for (auto i = 1; i < vertex_n; i++) {
        if (i < connected_v) {
            graph.edges.push_back({
                        .tail = vertices[i - 1],
                        .head = vertices[i]
                        });
            vertices[i]->edges.push_back(&graph.edges.back());
            vertices[i-1]->edges.push_back(&graph.edges.back());
            graph.edges.push_back({
                        .tail = vertices[i],
                        .head = vertices[i - 1]
                        });
            vertices[i]->edges.push_back(&graph.edges.back());
            vertices[i-1]->edges.push_back(&graph.edges.back());

            // for each further vertex there is a 30 percent chance that it will be connected with the current one
            for (auto k = 2; k < connected_v - i ; k++) {
                if(common::get_random_in_range(1, 100) > 15) continue;

                graph.edges.push_back({
                            .tail = vertices[i + k],
                            .head = vertices[i]
                            });
                vertices[i]->edges.push_back(&graph.edges.back());
                vertices[i + k]->edges.push_back(&graph.edges.back());
                graph.edges.push_back({
                            .tail = vertices[i],
                            .head = vertices[i + k]
                            });
                vertices[i]->edges.push_back(&graph.edges.back());
                vertices[i + k]->edges.push_back(&graph.edges.back());
            }
        } else {
            auto random_to_connect = common::get_random_in_range(i + 1, vertex_n);
            if (random_to_connect >= vertex_n || random_to_connect <= i) break;

            if (common::get_random_in_range(1, 100) > 50) {
                graph.edges.push_back({
                            .tail = vertices[random_to_connect],
                            .head = vertices[i]
                            });
                vertices[i]->edges.push_back(&graph.edges.back());
                graph.edges.push_back({
                            .tail = vertices[i],
                            .head = vertices[random_to_connect]
                            });
                vertices[i]->edges.push_back(&graph.edges.back());
            }
        }
    }

    // test random graph
    for (auto v_i = 0; v_i < vertices.size(); v_i++) {
        // mark all as unexplored
        for (auto& vertex : graph.nodes) {
            vertex.node_data.explored = false;
        }

        gr::dfs<GraphData>(vertices[v_i]);

        for (auto i = 0; i < vertices.size(); i++) {
            assert(i < connected_v && v_i < connected_v ?
                    vertices[i]->node_data.explored :
                    i >= connected_v && v_i < connected_v ?
                        !vertices[i]->node_data.explored :
                        true && "node connected but should not be");
        }

    }
    // Graph<GraphData>::vmatrix matrix(n);
    //
    // for (auto i = 0; i < n; i++){
    //     GraphData gd{i};
    //     std::vector<int> row(n);
    //
    //     std::for_each(row.begin() + i + 1, row.end(), [&](auto& c) {
    //         c = common::get_random_in_range(0, 1);
    //     });
    //
    //     matrix[i] = std::make_tuple(gd, std::move(row));
    // }
    // for (auto r = 0; r < n; r++) {
    //     auto& row = std::get<1>(matrix[r]);
    //     for (auto c = 0; c < n; c++) {
    //         std::get<1>(matrix[c])[r] = row[c];
    //     }
    // }
    //
    // auto graph = gr::Graph<GraphData>::from_matrix(matrix);
    //
    // int i{};
    // for (auto it = graph.nodes.begin(); it != graph.nodes.end(); it++) {
    //
    //     i++;
    // }
}
int main(void) {
    for (auto i = 0; i < 100; i++) {
        test_if_connected(gr::dfs<GraphData>);
        test_if_connected(gr::bfs<GraphData>);
        test_if_connected(gr::dfs_recursive<GraphData>);
    }
}
