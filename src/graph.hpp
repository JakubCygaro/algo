#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <cstddef>
#include <algorithm>
#include <limits>
#include <deque>
#include <list>
#include <tuple>
#include <vector>
#include <cstdio>
#include <stdexcept>
#include <stack>
namespace gr {

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
                        nodes_v[node_b]->edges.push_back(edge);
                    }
                }
            }
            return Graph<T>{
                .nodes = std::move(nodes),
                .edges = std::move(edges)
            };
        }
    };
    class ExplorableGraphData {
    public:
        bool explored{};
    };
    class TopoSortableGraphData : public ExplorableGraphData {
    public:
        std::size_t f_value {std::numeric_limits<std::size_t>::max()};
    };
    class SCCGraphData : public TopoSortableGraphData {
    public:
        std::size_t scc_n {0};
    };
    template <typename T, typename N = Graph<T>::node_t>
    inline void dfs(N* start) {
        static_assert(std::is_convertible<T*, ExplorableGraphData*>::value, "T must be derived from ExplorableGraphData");
        std::stack<N*> nodes{};
        nodes.push(start);

        while(!nodes.empty()) {
            auto v = nodes.top();
            auto* e_ex = static_cast<ExplorableGraphData*>(&v->node_data);
            nodes.pop();
            if (!e_ex->explored) {
                e_ex->explored = true;
                for(auto& edge : v->edges) {
                    nodes.push(edge->head);
                }
            }
        }
    }
    template <typename T, typename N = Graph<T>::node_t>
    inline void dfs_recursive(N* v) {
        static_assert(std::is_convertible<T*, ExplorableGraphData*>::value, "T must be derived from ExplorableGraphData");

        auto* e_ex = static_cast<ExplorableGraphData*>(&v->node_data);
        e_ex->explored = true;

        for(auto& edge : v->edges) {
            if (!static_cast<ExplorableGraphData*>(&edge->head->node_data)->explored) {
                dfs_recursive<T>(edge->head);
            }
        }
    }
    template <typename T, typename N = Graph<T>::node_t>
    inline void dfs_topo(N* v, std::size_t& label, bool rev = false) {
        static_assert(std::is_convertible<T*, TopoSortableGraphData*>::value, "T must be derived from ExplorableGraphData");

        auto* e_ex = static_cast<TopoSortableGraphData*>(&v->node_data);
        e_ex->explored = true;

        for(auto& edge : v->edges) {
            auto endpoint = (rev ? edge->tail : edge->head);
            if (!static_cast<TopoSortableGraphData*>(&endpoint->node_data)->explored) {
                dfs_topo<T>(endpoint, label);
            }
        }
        e_ex->f_value = label;
        label--;
    }
    template <typename T, typename N = Graph<T>::node_t>
    inline void topo_sort(Graph<T>& graph) {
        static_assert(std::is_convertible<T*, TopoSortableGraphData*>::value, "T must be derived from ExplorableGraphData");

        auto current_label = graph.nodes.size() - 1;

        for (auto& v : graph.nodes) {
            if (!static_cast<TopoSortableGraphData*>(&v.node_data)->explored) {
                dfs_topo<T>(&v, current_label);
            }
        }
    }
    namespace {
        template <typename T, typename N = Graph<T>::node_t>
        inline void dfs_topo_rev_for_scc(N* v, std::size_t& label, std::vector<N*>& order) {
            static_assert(std::is_convertible<T*, SCCGraphData*>::value, "T must be derived from ExplorableGraphData");

            auto* e_ex = static_cast<SCCGraphData*>(&v->node_data);
            e_ex->explored = true;

            for(auto& edge : v->edges) {
                auto endpoint = edge->tail;
                if (!static_cast<SCCGraphData*>(&endpoint->node_data)->explored) {
                    dfs_topo_rev_for_scc<T>(endpoint, label, order);
                }
            }
            e_ex->f_value = label;
            order[label] = v;
            label--;
        }
        template <typename T, typename N = Graph<T>::node_t>
        inline void dfs_scc(N* v) {
            static_assert(std::is_convertible<T*, SCCGraphData*>::value, "T must be derived from ExplorableGraphData");

            auto* e_ex = static_cast<SCCGraphData*>(&v->node_data);
            e_ex->explored = true;

            for(auto& edge : v->edges) {
                auto endpoint = edge->head;
                if (auto scc_data_ptr = static_cast<SCCGraphData*>(&endpoint->node_data); !scc_data_ptr->explored) {
                    scc_data_ptr->scc_n = e_ex->scc_n;
                    dfs_scc<T>(endpoint);
                }
            }
        }
    }
    template <typename T, typename N = Graph<T>::node_t>
    inline void strongly_connected(Graph<T>& graph) {
        static_assert(std::is_convertible<T*, SCCGraphData*>::value, "T must be derived from ExplorableGraphData");

        auto current_label = graph.nodes.size();
        std::vector<N*> order(current_label--);

        for (auto& v : graph.nodes) {
            if (!static_cast<SCCGraphData*>(&v.node_data)->explored) {
                dfs_topo_rev_for_scc<T>(&v, current_label, order);
            }
        }
        std::for_each(order.begin(), order.end(), [&](auto& v){
            v->node_data.explored = false;
        });
        std::size_t scc = 0;
        for (auto v_ptr : order) {
            if (auto nd_as_scc = static_cast<SCCGraphData*>(&v_ptr->node_data); !nd_as_scc->explored) {
                nd_as_scc->scc_n = scc;
                dfs_scc<T>(v_ptr);
                scc++;
            }
        }
    }

    template <typename T, typename N = Graph<T>::node_t>
    inline void bfs(N* start) {
        static_assert(std::is_convertible<T*, ExplorableGraphData*>::value, "T must be derived from ExplorableGraphData");
        std::deque<N*> nodes{};
        nodes.push_back(start);

        while(!nodes.empty()) {
            auto v = nodes.front();
            auto* e_ex = static_cast<ExplorableGraphData*>(&v->node_data);
            nodes.pop_front();
            if (!e_ex->explored) {
                e_ex->explored = true;
                for(auto& edge : v->edges) {
                    nodes.push_back(edge->head);
                }
            }
        }
    }
}


#endif
