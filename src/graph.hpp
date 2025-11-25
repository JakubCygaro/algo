#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdio>
#include <datatypes.hpp>
#include <deque>
#include <limits>
#include <list>
#include <stack>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <vector>
namespace gr {
class NamedGraphData {
public:
    char name{};
};
class ExplorableGraphData {
public:
    bool explored {};
};
class TopoSortableGraphData : public ExplorableGraphData {
public:
    std::size_t f_value { std::numeric_limits<std::size_t>::max() };
};
class SCCGraphData : public TopoSortableGraphData {
public:
    std::size_t scc_n { 0 };
};
struct edge_empty_data { };
template <typename T, typename E = edge_empty_data>
class Graph {
public:
    struct Node;
    struct Edge;

    struct Node {
        std::list<Edge*> edges {};
        T node_data;
    };
    struct Edge {
        Node* tail {};
        Node* head {};
        E edge_data;
        inline bool operator==(const Edge& other) const noexcept
        {
            auto ret = (tail == other.tail && head == other.head);
            if constexpr (std::equality_comparable<E>) {
                ret &= edge_data == other.edge_data;
            }
            return ret;
        }
        // get a new edge in the reverse direction to this one
        inline  Edge rev() const noexcept {
            // hihi
            return std::remove_pointer_t<decltype(this)> {
                .tail = this->head,
                .head = this->tail,
                .edge_data = this->edge_data,
            };
        }
    };

    using node_t = Node;
    using edge_t = Edge;
    using node_data_t = T;
    using edge_data_t = E;
    using vmatrix = std::vector<std::tuple<node_data_t, std::vector<int>>>;
    using vmatrix_e = std::vector<std::tuple<T, std::vector<std::tuple<int, edge_data_t>>>>;

    struct DijkstraData : public ExplorableGraphData {
        std::size_t len {};
        node_t* prev {};
        bool in_path { false };
    };
    using dijkstra_data_t = DijkstraData;

public:
    std::list<Node> nodes {};
    std::list<Edge> edges {};

    template <int N>
    inline static Graph<T> from_matrix(std::array<std::tuple<T, std::array<int, N>>, N> mtx)
    {
        static_assert(std::is_default_constructible<T>(), "T is not default constructible");
        std::vector<Graph<T, E>::Node*> nodes_v(N);
        std::list<Graph<T, E>::Node> nodes {};
        std::list<Graph<T, E>::Edge> edges {};

        // create nodes
        for (auto i = 0; i < N; i++) {
            nodes.push_back(Graph<T, E>::Node { .edges = {}, .node_data = std::get<0>(mtx[i]) });
            nodes_v[i] = &nodes.back();
        }

        for (auto node_a = 0; node_a < N; node_a++) {
            for (auto node_b = 0; node_b < N; node_b++) {
                // skip if this is the same node
                if (node_a == node_b)
                    continue;

                if (std::get<1>(mtx[node_a])[node_b] == 1) {
                    edges.push_back(Graph<T, E>::Edge { .tail = nodes_v[node_a], .head = nodes_v[node_b] });
                    const auto edge = &edges.back();
                    nodes_v[node_a]->edges.push_back(edge);
                }
            }
        }
        return Graph<T> {
            .nodes = std::move(nodes),
            .edges = std::move(edges)
        };
    }
    inline static Graph<T> from_matrix(std::vector<std::tuple<T, std::vector<int>>> mtx)
    {
        static_assert(std::is_default_constructible<T>(), "T is not default constructible");
        static_assert(std::is_default_constructible<E>(), "E is not default constructible");
        if (mtx.size() != std::get<1>(mtx[0]).size()) {
            throw std::runtime_error("the matrix must be a square matrix");
        }
        auto const N = mtx.size();
        std::vector<Graph<T, E>::Node*> nodes_v(N);
        std::list<Graph<T, E>::Node> nodes {};
        std::list<Graph<T, E>::Edge> edges {};

        // create nodes
        for (auto i = 0; i < N; i++) {
            nodes.push_back(Graph<T, E>::Node { .edges = {}, .node_data = std::get<0>(mtx[i]) });
            nodes_v[i] = &nodes.back();
        }

        for (auto node_a = 0; node_a < N; node_a++) {
            for (auto node_b = 0; node_b < N; node_b++) {
                // skip if this is the same node
                if (node_a == node_b)
                    continue;

                if (std::get<1>(mtx[node_a])[node_b] == 1) {
                    edges.push_back(Graph<T, E>::Edge { .tail = nodes_v[node_a], .head = nodes_v[node_b] });
                    const auto edge = &edges.back();
                    nodes_v[node_a]->edges.push_back(edge);
                    nodes_v[node_b]->edges.push_back(edge);
                }
            }
        }
        return Graph<T> {
            .nodes = std::move(nodes),
            .edges = std::move(edges)
        };
    }
    inline static Graph<T, E> from_matrix(std::vector<std::tuple<T, std::vector<std::tuple<int, E>>>> mtx)
    {
        static_assert(std::is_default_constructible<T>(), "T is not default constructible");
        static_assert(std::is_default_constructible<E>(), "E is not default constructible");
        if (mtx.size() != std::get<1>(mtx[0]).size()) {
            throw std::runtime_error("the matrix must be a square matrix");
        }
        auto const N = mtx.size();
        std::vector<Graph<T, E>::Node*> nodes_v(N);
        std::list<Graph<T, E>::Node> nodes {};
        std::list<Graph<T, E>::Edge> edges {};

        // create nodes
        for (std::size_t i = 0; i < N; i++) {
            nodes.push_back(Graph<T, E>::Node { .edges = {}, .node_data = std::get<0>(mtx[i]) });
            nodes_v[i] = &nodes.back();
        }

        for (std::size_t node_a = 0; node_a < N; node_a++) {
            for (std::size_t node_b = 0; node_b < N; node_b++) {
                // skip if this is the same node
                if (node_a == node_b)
                    continue;

                if (std::get<0>(std::get<1>(mtx[node_a])[node_b]) == 1) {
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
        return Graph<T, E> {
            .nodes = std::move(nodes),
            .edges = std::move(edges)
        };
    }

    Node* add_node(Node&& n) {
        this->nodes.push_back(std::move(n));
        return &this->nodes.back();
    }
    Node* add_node(Node n) {
        return add_node(std::move(n));
    }
    Edge* connect_nodes(const Node* a, const Node* b){
        Edge e = {
            .tail = a,
            .head = b,
        };
        this->edges.push_back(std::move(e));
        return &this->edges.back();
    }
};
template <typename T, typename N = Graph<T>::node_t>
inline void dfs(N* start)
{
    static_assert(std::is_convertible<T*, ExplorableGraphData*>::value, "T must be derived from ExplorableGraphData");
    std::stack<N*> nodes {};
    nodes.push(start);

    while (!nodes.empty()) {
        auto v = nodes.top();
        auto* e_ex = static_cast<ExplorableGraphData*>(&v->node_data);
        nodes.pop();
        if (!e_ex->explored) {
            e_ex->explored = true;
            for (auto& edge : v->edges) {
                nodes.push(edge->head);
            }
        }
    }
}
template <typename T, typename N = Graph<T>::node_t>
inline void dfs_recursive(N* v)
{
    static_assert(std::is_convertible<T*, ExplorableGraphData*>::value, "T must be derived from ExplorableGraphData");

    auto* e_ex = static_cast<ExplorableGraphData*>(&v->node_data);
    e_ex->explored = true;

    for (auto& edge : v->edges) {
        if (!static_cast<ExplorableGraphData*>(&edge->head->node_data)->explored) {
            dfs_recursive<T>(edge->head);
        }
    }
}
template <typename T, typename N = Graph<T>::node_t>
inline void dfs_topo(N* v, std::size_t& label, bool rev = false)
{
    static_assert(std::is_convertible<T*, TopoSortableGraphData*>::value, "T must be derived from ExplorableGraphData");

    auto* e_ex = static_cast<TopoSortableGraphData*>(&v->node_data);
    e_ex->explored = true;

    for (auto& edge : v->edges) {
        auto endpoint = (rev ? edge->tail : edge->head);
        if (!static_cast<TopoSortableGraphData*>(&endpoint->node_data)->explored) {
            dfs_topo<T>(endpoint, label);
        }
    }
    e_ex->f_value = label;
    label--;
}
template <typename T, typename N = Graph<T>::node_t>
inline void topo_sort(Graph<T>& graph)
{
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
    inline void dfs_topo_rev_for_scc(N* v, std::size_t& label, std::vector<N*>& order)
    {
        static_assert(std::is_convertible<T*, SCCGraphData*>::value, "T must be derived from ExplorableGraphData");

        auto* e_ex = static_cast<SCCGraphData*>(&v->node_data);
        e_ex->explored = true;

        for (auto& edge : v->edges) {
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
    inline void dfs_scc(N* v)
    {
        static_assert(std::is_convertible<T*, SCCGraphData*>::value, "T must be derived from ExplorableGraphData");

        auto* e_ex = static_cast<SCCGraphData*>(&v->node_data);
        e_ex->explored = true;

        for (auto& edge : v->edges) {
            auto endpoint = edge->head;
            if (auto scc_data_ptr = static_cast<SCCGraphData*>(&endpoint->node_data); !scc_data_ptr->explored) {
                scc_data_ptr->scc_n = e_ex->scc_n;
                dfs_scc<T>(endpoint);
            }
        }
    }
}
template <typename T, typename N = Graph<T>::node_t>
inline void strongly_connected(Graph<T>& graph)
{
    static_assert(std::is_convertible<T*, SCCGraphData*>::value, "T must be derived from ExplorableGraphData");

    auto current_label = graph.nodes.size();
    std::vector<N*> order(current_label--);

    for (auto& v : graph.nodes) {
        if (!static_cast<SCCGraphData*>(&v.node_data)->explored) {
            dfs_topo_rev_for_scc<T>(&v, current_label, order);
        }
    }
    std::for_each(order.begin(), order.end(), [&](auto& v) {
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
inline void bfs(N* start)
{
    static_assert(std::is_convertible<T*, ExplorableGraphData*>::value, "T must be derived from ExplorableGraphData");
    std::deque<N*> nodes {};
    nodes.push_back(start);

    while (!nodes.empty()) {
        auto v = nodes.front();
        auto* e_ex = static_cast<ExplorableGraphData*>(&v->node_data);
        nodes.pop_front();
        if (!e_ex->explored) {
            e_ex->explored = true;
            for (auto& edge : v->edges) {
                nodes.push_back(edge->head);
            }
        }
    }
}
struct DijkstraEdge {
    std::size_t dijkstra_score {};
    inline DijkstraEdge(std::size_t s)
        : dijkstra_score(s)
    {
    }
    inline DijkstraEdge() { }
};
template <typename T, typename E,
    typename G = Graph<T, E>,
    typename N = G::node_t,
    typename ED = G::edge_t,
    typename DData = G::dijkstra_data_t>
inline void dijkstra(Graph<T, E>& graph, N* start)
{
    static_assert(std::is_convertible<T*, DData*>::value, "T must be derived from DijkstraData");
    static_assert(std::is_convertible<E*, DijkstraEdge*>::value, "E must be derived from DijkstraEdge");

    constexpr auto INF = std::numeric_limits<decltype(DData::len)>::max();

    static_cast<DData*>(&start->node_data)->len = 0;
    static_cast<DData*>(&start->node_data)->in_path = true;
    for (auto& v : graph.nodes) {
        if (&v == start)
            continue;
        v.node_data.len = INF;
        v.node_data.in_path = false;
    }

    for (std::size_t i = 1; i < graph.nodes.size(); i++) {
        T* v_d = nullptr;
        N* w = nullptr;
        ED* edge = nullptr;
        auto min_score = INF;

        for (auto& e : graph.edges) {
            if (!e.tail->node_data.in_path || e.head->node_data.in_path)
                continue;
            if (e.tail->node_data.len == INF)
                continue;

            auto candidate = e.tail->node_data.len + e.edge_data.dijkstra_score;

            if (candidate < min_score) {
                v_d = &e.tail->node_data;
                w = e.head;
                min_score = candidate;
                edge = &e;
            }
        }
        if (!w)
            break;
        w->node_data.len = v_d->len + edge->edge_data.dijkstra_score;
        w->node_data.in_path = true;
    }
}
template <typename T, typename E,
    typename G = Graph<T, E>,
    typename N = G::node_t,
    typename ED = G::edge_t,
    typename DData = G::DijkstraData>
inline const std::vector<N*> dijkstra_shortest_path(Graph<T, E>& graph, N* start, N* end)
{
    static_assert(std::is_convertible<T*, DData*>::value, "T must be derived from DijkstraData");
    static_assert(std::is_convertible<E*, DijkstraEdge*>::value, "E must be derived from DijkstraEdge");

    constexpr auto INF = std::numeric_limits<decltype(DData::len)>::max();

    std::vector<N*> path {};

    if (start == end) {
        path.push_back(start);
        return path;
    }

    static_cast<DData*>(&start->node_data)->len = 0;
    static_cast<DData*>(&start->node_data)->in_path = true;
    static_cast<DData*>(&start->node_data)->prev = nullptr;

    for (auto& v : graph.nodes) {
        if (&v == start)
            continue;
        v.node_data.len = INF;
        v.node_data.prev = nullptr;
        v.node_data.in_path = false;
    }
    for (auto i = graph.nodes.size(); i >= 0; i--) {
        T* v_d = nullptr;
        N* w = nullptr;
        ED* edge = nullptr;
        auto min_score = INF;

        for (auto& e : graph.edges) {
            if (!e.tail->node_data.in_path || e.head->node_data.in_path)
                continue;
            if (e.tail->node_data.len == INF)
                continue;

            auto candidate = e.tail->node_data.len + e.edge_data.dijkstra_score;

            if (candidate < min_score) {
                v_d = &e.tail->node_data;
                w = e.head;
                min_score = candidate;
                edge = &e;
                w->node_data.prev = e.tail;
            }
        }
        if (!w) {
            return path;
        }
        w->node_data.len = v_d->len + edge->edge_data.dijkstra_score;
        w->node_data.in_path = true;
        if (w == end) {
            while (w) {
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
inline void dijkstra_h(Graph<T, E>& graph, N* start)
{
    static_assert(std::is_convertible<T*, DData*>::value, "T must be derived from DijkstraData");
    static_assert(std::is_convertible<E*, DijkstraEdge*>::value, "E must be derived from DijkstraEdge");

    constexpr auto INF = std::numeric_limits<decltype(DData::len)>::max();

    dt::MinHeap<decltype(DData::len), N*> heap {};

    static_cast<DData*>(&start->node_data)->len = 0;
    heap.insert(0, start);
    for (N& v : graph.nodes) {
        v.node_data.in_path = false;
        if (&v == start)
            continue;
        v.node_data.len = INF;
        heap.insert(INF, &v);
    }
    while (!heap.empty()) {
        auto [k, w] = heap.extract();
        assert(w && "w was null");
        w->node_data.len = k;
        w->node_data.in_path = true;
        for (auto e : w->edges) {
            assert(e->head && "head was null");
            if (e->head->node_data.in_path)
                continue;
            assert(!e->head->node_data.in_path && "head in path");
            auto [len, found] = heap.search(e->head);
            assert(found && "found was not in the heap");
            if (w->node_data.len == INF)
                continue;
            e->head->node_data.len = std::min(len, w->node_data.len + e->edge_data.dijkstra_score);
            assert(heap.delete_element(found) && "attempted to delete element that was not found");
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
inline std::vector<N*> dijkstra_shortest_path_h(Graph<T, E>& graph, N* start, N* end)
{
    static_assert(std::is_convertible<T*, DData*>::value, "T must be derived from DijkstraData");
    static_assert(std::is_convertible<E*, DijkstraEdge*>::value, "E must be derived from DijkstraEdge");

    constexpr auto INF = std::numeric_limits<decltype(DData::len)>::max();

    dt::MinHeap<decltype(DData::len), N*> heap {};
    std::vector<N*> path {};

    static_cast<DData*>(&start->node_data)->len = 0;
    if (start == end) {
        path.push_back(start);
        return path;
    }

    heap.insert(0, start);
    for (N& v : graph.nodes) {
        v.node_data.in_path = false;
        v.node_data.prev = nullptr;
        if (&v == start)
            continue;
        v.node_data.len = INF;
        heap.insert(INF, &v);
    }
    N* prev = nullptr;
    while (!heap.empty()) {
        auto [k, w] = heap.extract();
        w->node_data.len = k;
        w->node_data.in_path = true;
        for (auto e : w->edges) {
            if (w->node_data.len == INF)
                break;
            if (e->head->node_data.in_path)
                continue;
            auto [len, found] = heap.search(e->head);
            e->head->node_data.len = std::min(len, w->node_data.len + e->edge_data.dijkstra_score);
            heap.delete_element(found);
            heap.insert(e->head->node_data.len, e->head);
        }
        w->node_data.prev = prev;
        if (w->node_data.len != INF)
            prev = w;
        if (w == end)
            break;
    }
    if (prev != end)
        return path;
    while (prev) {
        path.push_back(prev);
        prev = prev->node_data.prev;
    }
    return path;
}
template <typename T, typename E,
    typename G = Graph<T, E>,
    typename N = G::node_t,
    typename ED = G::edge_t,
    typename DData = G::dijkstra_data_t>
inline std::vector<ED*> prim_mst_heap(Graph<T, E>& gr, N* start)
{
    static_assert(std::is_convertible<T*, gr::ExplorableGraphData*>::value, "T must be derived from gr::ExplorableGraphData");
    static_assert(std::is_convertible<E*, DijkstraEdge*>::value, "E must be derived from gr::DijkstraEdge");
    using edge_t = ED;
    using node_t = N;
    constexpr const size_t INF = std::numeric_limits<size_t>::max();

    std::vector<edge_t*> tree {};
    std::vector<node_t*> x = { start };
    dt::MinHeap<size_t, node_t*> heap;
    x[0]->node_data.explored = true;

    for (edge_t* edge : x[0]->edges) {
        auto* v = edge->head != x[0] ? edge->head : edge->tail;
        assert(v && "edge with null endpoint");
        v->node_data.winner = edge;
        heap.insert(edge->edge_data.dijkstra_score, v);
    }
    for (node_t& node : gr.nodes) {
        if (&node != x[0] && !node.node_data.winner) {
            heap.insert(INF, &node);
        }
    }
    while (!heap.empty()) {
        auto [score, w] = heap.extract();
        assert(w && "extracted node was null");
        if (w->node_data.explored) continue;
        x.push_back(w);
        tree.push_back(w->node_data.winner);
        w->node_data.explored = true;

        for (auto* e : w->edges) {
            auto* y = e->head != w ? e->head : e->tail;
            if (y) {
                // if the winner of y is NULL, cost is INF
                size_t cost = y->node_data.winner ? y->node_data.winner->edge_data.dijkstra_score : INF;

                if (e->edge_data.dijkstra_score < cost) {
                    heap.delete_element(&y);
                    y->node_data.winner = e;
                    heap.insert(e->edge_data.dijkstra_score, y);
                }
            }
        }
    }
    return tree;
}
template <typename T, typename E,
    typename G = Graph<T, E>,
    typename N = G::node_t,
    typename ED = G::edge_t,
    typename DData = G::dijkstra_data_t>
inline std::vector<ED*> kruskal_mst(Graph<T, E>& gr)
{
    static_assert(std::is_convertible<T*, gr::ExplorableGraphData*>::value, "T must be derived from gr::ExplorableGraphData");
    static_assert(std::is_convertible<E*, DijkstraEdge*>::value, "E must be derived from gr::DijkstraEdge");
    std::vector<N*> vertices { gr.nodes.size() };
    size_t i = 0;
    for (auto& n : gr.nodes) {
        vertices[i++] = &n;
    }
    std::vector<ED*> tree {};
    dt::UnionFind<N*> union_find { vertices };

    gr.edges.sort([](ED a, ED b) { return a.edge_data.dijkstra_score < b.edge_data.dijkstra_score; });

    for (ED& e : gr.edges) {
        auto v = union_find.find(e.tail);
        auto w = union_find.find(e.head);
        if (v != w) {
            tree.push_back(&e);
        }
        union_find.unionize(*v, *w);
    }
    return tree;
}
struct FWEdge {
    int len {};
    FWEdge(int l)
        : len(l)
    {
    }
    FWEdge() { };
};
struct FWNode {
    char name {};
    Graph<FWNode, FWEdge>::Node* pred {};
    FWNode(char n)
        : name(n) { };
    FWNode() { };
};
namespace {
    template<typename T>
    T add_with_no_over_under_flow(const T& a, const T& b){
        constexpr const T PLUS_INF = std::numeric_limits<T>::max();
        constexpr const T MINUS_INF = std::numeric_limits<T>::min();
        T sum = a + b;
        if(a > 0 && b > 0 && sum < 0) return PLUS_INF;
        if(a < 0 && b < 0 && sum > 0) return MINUS_INF;
        return sum;
    }
}
template <typename T, typename E,
    typename G = Graph<T, E>,
    typename N = G::node_t,
    typename ED = G::edge_t,
    typename DData = G::dijkstra_data_t>
inline std::optional<std::vector<ED*>> floyd_warshall_paths(Graph<T, E>& graph, N* src_n, N* dst_n)
{
    using node_t = N;
    using edge_t = ED;
    const auto Num = graph.nodes.size();
    constexpr const auto INF = std::numeric_limits<int>::max();
    std::vector<node_t*> nodes {};
    size_t src {}, dst {};
    size_t count = 0;
    for (auto& v : graph.nodes) {
        nodes.push_back(&v);
        if(&v == src_n) src = count;
        if(&v == dst_n) dst = count;
        count++;
    }

    int cache[Num + 1][Num][Num];
    for (auto i = 0ul; i < Num + 1; i++) {
        for (auto j = 0ul; j < Num; j++) {
            for (auto k = 0ul; k < Num; k++) {
                cache[i][j][k] = INF;
            }
        }
    }
    // base case, k = 0
    for (auto v = 0ul; v < Num; v++) {
        for (auto w = 0ul; w < Num; w++) {
            if (v == w) {
                cache[0][v][w] = 0;
            } else if (auto f = std::find_if(graph.edges.begin(), graph.edges.end(), [&](edge_t& e) {
                           return e.tail == nodes[v] && e.head == nodes[w];
                       });
                f != graph.edges.end()) {
                cache[0][v][w] = f->edge_data.len;
            } else {
                cache[0][v][w] = INF;
            }
        }
    }
    for (auto k = 1ul; k <= Num; k++) {
        for (auto v = 0ul; v <= Num; v++) {
            for (auto w = 0ul; w <= Num; w++) {
                auto tmp = add_with_no_over_under_flow<int>(cache[k - 1][v][k], cache[k - 1][k][w]);
                cache[k][v][w] = std::min(
                    cache[k - 1][v][w],
                    tmp);
            }
        }
    }
    for (auto v = 0ul; v < Num; v++) {
        if (cache[Num][v][v] < 0) {
            return std::nullopt;
        }
    }
    auto sol = cache[Num][src][dst];
}
}

#endif
