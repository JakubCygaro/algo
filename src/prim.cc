#include "graph.hpp"
#include <algorithm>
#include <print>
#include <ranges>

struct Node : public gr::ExplorableGraphData {
    char name = '\0';
    gr::Graph<Node, gr::DijkstraEdge>::Edge* winner = nullptr;

    Node(char n) : name(n){}
    Node() {};
};

int main(void) {
    gr::Graph<Node, gr::DijkstraEdge> gr;
    decltype(gr)::vmatrix_e mtx = {{
        //         a       b       c       d        e
        { 'a', { {0, 0}, {1, 1}, {1, 4}, {1, 3}, {0, 0} } },
        { 'b', { {1, 1}, {0, 0}, {0, 0}, {1, 2}, {0, 0} } },
        { 'c', { {1, 4}, {0, 0}, {0, 0}, {1, 5}, {1, 4} } },
        { 'd', { {1, 3}, {1, 2}, {1, 5}, {0, 0}, {1, 6} } },
        { 'e', { {0, 0}, {0, 0}, {1, 4}, {1, 6}, {0, 0} } },
    }};
    gr = decltype(gr)::from_matrix(mtx);

    using edge_t = decltype(gr)::edge_t;
    using node_t = decltype(gr)::node_t;
    dt::MinHeap<size_t, edge_t*> edges;
    dt::MinHeap<size_t, node_t*> heap;

    std::vector<edge_t*> tree{};
    std::vector<node_t*> x = {&gr.nodes.front()};
    x[0]->node_data.explored = true;

    constexpr const size_t INF = std::numeric_limits<size_t>::max();
    for(edge_t* edge : x[0]->edges){
        edge->head->node_data.winner = edge;
        heap.insert(edge->edge_data.dijkstra_score, edge->head);
    }
    for(node_t& node : gr.nodes){
        if(&node != x[0] && !node.node_data.winner){
            heap.insert(INF, &node);
        }
    }
    while(!heap.empty()){
        auto [score, w] = heap.extract();
        if(!w || w->node_data.explored) continue;
        x.push_back(w);
        tree.push_back(w->node_data.winner);
        w->node_data.explored = true;

        for(auto* e : w->edges){
            if(e->tail == w && !e->head->node_data.explored){
                size_t cost = INF;
                // if the winner of head is NULL, cost is INF
                if(!e->head->node_data.winner)
                    cost = INF;
                else
                    cost = e->head->node_data.winner->edge_data.dijkstra_score;
                if(e->edge_data.dijkstra_score < cost){
                    heap.delete_element(&e->head);
                    e->head->node_data.winner = e;
                    heap.insert(e->edge_data.dijkstra_score, e->head);
                }
            }
        }
    }
    std::ranges::for_each(x, [](node_t* n) { std::println("{}", n->node_data.name); });
    std::ranges::for_each(tree, [](edge_t* e) { std::println("{}", e->edge_data.dijkstra_score); });

}
