#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <cassert>
#include <cstddef>
#include <algorithm>
#include <print>
#include <datatypes.hpp>
#include <format>
#include <iostream>
#include <limits>
#include <deque>
#include <list>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <cstdio>
#include <stdexcept>
#include <stack>
namespace gr {
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
    struct edge_empty_data{};
    template <typename T, typename E = edge_empty_data>
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
            E edge_data;
        };

        using node_t = Node;
        using edge_t = Edge;
        using node_data_t = T;
        using edge_data_t = E;
        using vmatrix = std::vector<std::tuple<node_data_t, std::vector<int>>>;
        using vmatrix_e = std::vector<std::tuple<T, std::vector<std::tuple<int, edge_data_t>>>>;

        struct DijkstraData : public ExplorableGraphData {
            std::size_t len{};
            node_t* prev{};
            bool in_path {false};
        };
        using dijkstra_data_t  = DijkstraData;
    public:
        std::list<Node> nodes{};
        std::list<Edge> edges{};

        template<int N>
        inline static Graph<T> from_matrix(std::array<std::tuple<T, std::array<int, N>>, N> mtx) {
            static_assert(std::is_default_constructible<T>(), "T is not default constructible");
            std::vector<Graph<T, E>::Node*> nodes_v(N);
            std::list<Graph<T, E>::Node> nodes{};
            std::list<Graph<T, E>::Edge> edges{};

            //create nodes
            for (auto i = 0; i < N; i++){
                nodes.push_back(Graph<T, E>::Node{.edges = {}, .node_data = std::get<0>(mtx[i])});
                nodes_v[i] = &nodes.back();
            }


            for (auto node_a = 0; node_a < N; node_a++) {
                for(auto node_b = 0; node_b < N; node_b++) {
                    //skip if this is the same node
                    if (node_a == node_b) continue;

                    if (std::get<1>(mtx[node_a])[node_b] == 1){
                        edges.push_back(Graph<T, E>::Edge { .tail = nodes_v[node_a], .head = nodes_v[node_b] });
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
            static_assert(std::is_default_constructible<E>(), "E is not default constructible");
            if(mtx.size() != std::get<1>(mtx[0]).size()) {
                throw std::runtime_error("the matrix must be a square matrix");
            }
            auto const N = mtx.size();
            std::vector<Graph<T, E>::Node*> nodes_v(N);
            std::list<Graph<T, E>::Node> nodes{};
            std::list<Graph<T, E>::Edge> edges{};

            //create nodes
            for (auto i = 0; i < N; i++){
                nodes.push_back(Graph<T, E>::Node{.edges = {}, .node_data = std::get<0>(mtx[i])});
                nodes_v[i] = &nodes.back();
            }


            for (auto node_a = 0; node_a < N; node_a++) {
                for(auto node_b = 0; node_b < N; node_b++) {
                    //skip if this is the same node
                    if (node_a == node_b) continue;

                    if (std::get<1>(mtx[node_a])[node_b] == 1){
                        edges.push_back(Graph<T, E>::Edge { .tail = nodes_v[node_a], .head = nodes_v[node_b] });
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
        inline static Graph<T, E> from_matrix(std::vector<std::tuple<T, std::vector<std::tuple<int, E>>>> mtx) {
            static_assert(std::is_default_constructible<T>(), "T is not default constructible");
            static_assert(std::is_default_constructible<E>(), "E is not default constructible");
            if(mtx.size() != std::get<1>(mtx[0]).size()) {
                throw std::runtime_error("the matrix must be a square matrix");
            }
            auto const N = mtx.size();
            std::vector<Graph<T, E>::Node*> nodes_v(N);
            std::list<Graph<T, E>::Node> nodes{};
            std::list<Graph<T, E>::Edge> edges{};

            //create nodes
            for (std::size_t i = 0; i < N; i++){
                nodes.push_back(Graph<T, E>::Node{.edges = {}, .node_data = std::get<0>(mtx[i])});
                nodes_v[i] = &nodes.back();
            }


            for (std::size_t node_a = 0; node_a < N; node_a++) {
                for(std::size_t node_b = 0; node_b < N; node_b++) {
                    //skip if this is the same node
                    if (node_a == node_b) continue;

                    if (std::get<0>(std::get<1>(mtx[node_a])[node_b]) == 1){
                        edges.push_back(Graph<T, E>::Edge {
                                .tail = nodes_v[node_a],
                                .head = nodes_v[node_b],
                                .edge_data = std::get<1>(std::get<1>(mtx[node_a])[node_b]),
                                });
                        const auto edge = &edges.back();
                        nodes_v[node_a]->edges.push_back(edge);
                        nodes_v[node_b]->edges.push_back(edge);
                    }
                }
            }
            return Graph<T, E>{
                .nodes = std::move(nodes),
                .edges = std::move(edges)
            };
        }
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
    struct DijkstraEdge {
        std::size_t dijkstra_score{};
        DijkstraEdge(std::size_t s) : dijkstra_score(s){}
        DijkstraEdge() {}
    };
    template <typename T, typename E,
             typename G = Graph<T, E>,
             typename N = G::node_t,
             typename ED = G::edge_t,
             typename DData = G::dijkstra_data_t>
    inline void dijkstra(Graph<T, E>& graph, N* start) {
        static_assert(std::is_convertible<T*, DData*>::value, "T must be derived from DijkstraData");
        static_assert(std::is_convertible<E*, DijkstraEdge*>::value, "E must be derived from DijkstraEdge");

        constexpr auto INF = std::numeric_limits<decltype(DData::len)>::max();


        static_cast<DData*>(&start->node_data)->len = 0;
        static_cast<DData*>(&start->node_data)->in_path = true;
        for(auto& v : graph.nodes) {
            if(&v == start) continue;
            v.node_data.len = INF;
            v.node_data.in_path = false;
        }

        for(std::size_t i = 1; i < graph.nodes.size(); i++) {
            T* v_d = nullptr;
            N* w = nullptr;
            ED* edge = nullptr;
            auto min_score = INF;

            for(auto& e : graph.edges) {
                if(!e.tail->node_data.in_path || e.head->node_data.in_path) continue;
                if(e.tail->node_data.len == INF) continue;

                auto candidate = e.tail->node_data.len + e.edge_data.dijkstra_score;

                if (candidate < min_score) {
                    v_d = &e.tail->node_data;
                    w = e.head;
                    min_score = candidate;
                    edge = &e;
                }
            }
            if(!w) break;
            w->node_data.len = v_d->len + edge->edge_data.dijkstra_score;
            w->node_data.in_path = true;
        }
    }
    template <typename T, typename E,
             typename G = Graph<T, E>,
             typename N = G::node_t,
             typename ED = G::edge_t,
             typename DData = G::DijkstraData>
    inline const std::vector<N*> dijkstra_shortest_path(Graph<T, E>& graph, N* start, N* end) {
        static_assert(std::is_convertible<T*, DData*>::value, "T must be derived from DijkstraData");
        static_assert(std::is_convertible<E*, DijkstraEdge*>::value, "E must be derived from DijkstraEdge");

        constexpr auto INF = std::numeric_limits<decltype(DData::len)>::max();

        std::vector<N*> path{};

        if(start == end){
            path.push_back(start);
            return path;
        }

        static_cast<DData*>(&start->node_data)->len = 0;
        static_cast<DData*>(&start->node_data)->in_path = true;
        static_cast<DData*>(&start->node_data)->prev = nullptr;

        for(auto& v : graph.nodes) {
            if(&v == start) continue;
            v.node_data.len = INF;
            v.node_data.prev = nullptr;
            v.node_data.in_path = false;
        }
        for(auto i = graph.nodes.size(); i >= 0; i--) {
            T* v_d = nullptr;
            N* w = nullptr;
            ED* edge = nullptr;
            auto min_score = INF;

            for(auto& e : graph.edges) {
                if(!e.tail->node_data.in_path || e.head->node_data.in_path) continue;
                if(e.tail->node_data.len == INF) continue;

                auto candidate = e.tail->node_data.len + e.edge_data.dijkstra_score;

                if (candidate < min_score) {
                    v_d = &e.tail->node_data;
                    w = e.head;
                    min_score = candidate;
                    edge = &e;
                    w->node_data.prev = e.tail;
                }
            }
            if(!w){
                return path;
            }
            w->node_data.len = v_d->len + edge->edge_data.dijkstra_score;
            w->node_data.in_path = true;
            if(w == end) {
                while(w) {
                    path.push_back(w);
                    w = w->node_data.prev;
                }
                break;
            };
        }
        // std::reverse(path.begin(), path.end());
        return path;
    }
    template <typename T, typename E,
             typename G = Graph<T, E>,
             typename N = G::node_t,
             typename ED = G::edge_t,
             typename DData = G::dijkstra_data_t>
    inline void dijkstra_h(Graph<T, E>& graph, N* start) {
        static_assert(std::is_convertible<T*, DData*>::value, "T must be derived from DijkstraData");
        static_assert(std::is_convertible<E*, DijkstraEdge*>::value, "E must be derived from DijkstraEdge");

        constexpr auto INF = std::numeric_limits<decltype(DData::len)>::max();

        dt::MinHeap<decltype(DData::len), N*> heap{};

        assert(start);
        static_cast<DData*>(&start->node_data)->len = 0;
        heap.insert(0, start);
        for(N& v : graph.nodes) {
            v.node_data.in_path = false;
            if(&v == start) continue;
            v.node_data.len = INF;
            heap.insert(INF, &v);
        }
        while(!heap.empty()) {
            auto [k, w] = heap.extract();
            assert(w && "w was null");
            w->node_data.len = k;
            w->node_data.in_path = true;
            for(auto e : w->edges) {
                assert(e->head && "head was null");
                if(e->head->node_data.in_path) continue;
                assert(!e->head->node_data.in_path && "head in path");
                auto [len, found] = heap.search(e->head);
                assert(found && "found was not in the heap");
                if(w->node_data.len == INF) continue;
                e->head->node_data.len = std::min(len, w->node_data.len + e->edge_data.dijkstra_score);
                assert (heap.delete_element(found) && "attempted to delete element that was not found");
                assert(e->head && "head was null");
                heap.insert(e->head->node_data.len, e->head);
            }
        }
    }
    template <typename T, typename E,
             typename G = Graph<T, E>,
             typename N = G::node_t,
             typename ED = G::edge_t,
             typename DData = G::dijkstra_data_t>
    inline std::vector<N*> dijkstra_shortest_path_h(Graph<T, E>& graph, N* start, N* end) {
        static_assert(std::is_convertible<T*, DData*>::value, "T must be derived from DijkstraData");
        static_assert(std::is_convertible<E*, DijkstraEdge*>::value, "E must be derived from DijkstraEdge");

        constexpr auto INF = std::numeric_limits<decltype(DData::len)>::max();

        dt::MinHeap<decltype(DData::len), N*> heap{};
        std::vector<N*> path{};

        if(start == end){
            path.push_back(start);
            return path;
        }

        static_cast<DData*>(&start->node_data)->len = 0;
        heap.insert(0, start);
        for(N& v : graph.nodes) {
            if(&v == start) continue;
            heap.insert(INF, &v);
            v.node_data.in_path = false;
        }
        N* prev = nullptr;
        while(!heap.empty()) {
            auto [k, w] = heap.extract();
            w->node_data.len = k;
            w->node_data.in_path = true;
            for(auto e : w->edges) {
                if(e->head->node_data.in_path) continue;
                auto [len, found] = heap.search(e->head);
                if(w->node_data.len == INF) continue;
                e->head->node_data.len = std::min(len, w->node_data.len + e->edge_data.dijkstra_score);
                heap.delete_element(found);
                heap.insert(e->head->node_data.len, e->head);
            }
            w->node_data.prev = prev;
            // path.push_back(w);
            if(w->node_data.len != INF)
                prev = w;
            if (w == end) break;
        }
        if(prev != end) return path;
        while(prev){
            path.push_back(prev);
            prev = prev->node_data.prev;
        }
        return path;
    }
}


#endif
