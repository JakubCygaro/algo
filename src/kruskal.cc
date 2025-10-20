#include <algorithm>
#include <datatypes.hpp>
#include <print>
#include <string>
#include <vector>
#include <graph.hpp>

struct Node : public gr::ExplorableGraphData {
    char name = '\0';
    gr::Graph<Node, gr::DijkstraEdge>::Edge* winner = nullptr;

    Node(char n) : name(n){}
    Node() {};
};

int main(void){
    using graph_t = gr::Graph<Node, gr::DijkstraEdge>;
    using edge_t = graph_t::edge_t;
    using node_t = graph_t::node_t;
    graph_t::vmatrix_e mtx = {{
        //         a       b       c       d        e
        { 'a', { {0, 0}, {1, 1}, {1, 4}, {1, 3}, {0, 0} } },
        { 'b', { {1, 1}, {0, 0}, {0, 0}, {1, 2}, {0, 0} } },
        { 'c', { {1, 4}, {0, 0}, {0, 0}, {1, 5}, {1, 4} } },
        { 'd', { {1, 3}, {1, 2}, {1, 5}, {0, 0}, {1, 6} } },
        { 'e', { {0, 0}, {0, 0}, {1, 4}, {1, 6}, {0, 0} } },
    }};

    graph_t gr = graph_t::from_matrix(mtx);

    std::vector<node_t*> vertices{ gr.nodes.size() };
    size_t i = 0;
    for(auto& n : gr.nodes){
        vertices[i++] = &n;
    }
    std::vector<edge_t*> tree{};
    dt::UnionFind<node_t*> union_find{ vertices };

    gr.edges.sort([](edge_t a, edge_t b) { return a.edge_data.dijkstra_score < b.edge_data.dijkstra_score; });

    for(edge_t& e : gr.edges){
        auto v = union_find.find(e.tail);
        auto w = union_find.find(e.head);
        if(v != w) {
            tree.push_back(&e);
        }
        union_find.unionize(*v, *w);
    }
    std::ranges::for_each(tree, [](edge_t* e) { std::println("{}", e->edge_data.dijkstra_score); });
}
