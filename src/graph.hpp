#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <list>
#include <tuple>
#include <vector>
#include <cstdio>
#include <stdexcept>

template <typename T>
class Graph {
public:
    struct Node;
    struct Edge;

    struct Node {
        std::list<Edge*> edges{};
        T node_data;
    };
    struct Edge {
        Node* tail{};
        Node* head{};
    };

    using node_t = Node;
    using edge_t = Edge;
    using node_data_t = T;
    using vmatrix = std::vector<std::tuple<node_data_t, std::vector<int>>>;
public:
    std::list<Node> nodes{};
    std::list<Edge> edges{};

    template<int N>
    inline static Graph<T> from_matrix(std::array<std::tuple<T, std::array<int, N>>, N> mtx) {
        static_assert(std::is_default_constructible<T>(), "T is not default constructible");
        std::vector<Graph<T>::Node*> nodes_v(N);
        std::list<Graph<T>::Node> nodes{};
        std::list<Graph<T>::Edge> edges{};

        //create nodes
        for (auto i = 0; i < N; i++){
            nodes.push_back(Graph<T>::Node{.edges = {}, .node_data = std::get<0>(mtx[i])});
            nodes_v[i] = &nodes.back();
        }


        for (auto node_a = 0; node_a < N; node_a++) {
            for(auto node_b = 0; node_b < N; node_b++) {
                //skip if this is the same node
                if (node_a == node_b) continue;

                if (std::get<1>(mtx[node_a])[node_b] == 1){
                    edges.push_back(Graph<T>::Edge { .tail = nodes_v[node_a], .head = nodes_v[node_b] });
                    const auto edge = &edges.back();
                    nodes_v[node_a]->edges.push_back(edge);
                }
            }
        }
        return Graph<T>{
            .nodes = std::move(nodes),
            .edges = std::move(edges)
        };
    }
    inline static Graph<T> from_matrix(std::vector<std::tuple<T, std::vector<int>>> mtx) {
        static_assert(std::is_default_constructible<T>(), "T is not default constructible");
        if(mtx.size() != std::get<1>(mtx[0]).size()) {
            throw std::runtime_error("the matrix must be a square matrix");
        }
        auto const N = mtx.size();
        std::vector<Graph<T>::Node*> nodes_v(N);
        std::list<Graph<T>::Node> nodes{};
        std::list<Graph<T>::Edge> edges{};

        //create nodes
        for (auto i = 0; i < N; i++){
            nodes.push_back(Graph<T>::Node{.edges = {}, .node_data = std::get<0>(mtx[i])});
            nodes_v[i] = &nodes.back();
        }


        for (auto node_a = 0; node_a < N; node_a++) {
            for(auto node_b = 0; node_b < N; node_b++) {
                //skip if this is the same node
                if (node_a == node_b) continue;

                if (std::get<1>(mtx[node_a])[node_b] == 1){
                    edges.push_back(Graph<T>::Edge { .tail = nodes_v[node_a], .head = nodes_v[node_b] });
                    const auto edge = &edges.back();
                    nodes_v[node_a]->edges.push_back(edge);
                }
            }
        }
        return Graph<T>{
            .nodes = std::move(nodes),
            .edges = std::move(edges)
        };
    }
};


#endif
