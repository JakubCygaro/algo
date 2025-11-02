#include "common.hpp"
#include <algorithm>
#include <datatypes.hpp>
#include <graph.hpp>
#include <vector>

struct Node : public gr::ExplorableGraphData {
    char name = '\0';

    Node(char n)
        : name(n)
    {
    }
    Node() { };
};

using graph_t = gr::Graph<Node, gr::DijkstraEdge>;
using edge_t = graph_t::edge_t;
using node_t = graph_t::node_t;

std::vector<edge_t*> setup_graph_with_answer(graph_t& gr)
{
    const auto v_count = common::get_random_in_range(5, 25);

    std::vector<node_t*> vertices {};

    for (int i = 0; i < v_count; i++) {
        auto n = node_t {
            .edges = std::list<edge_t*> {},
            .node_data = Node('a' + i)
        };
        gr.nodes.push_back(n);
        auto* v = &gr.nodes.back();
        vertices.push_back(v);
    }
    std::shuffle(vertices.begin(), vertices.end(), std::mt19937 { std::random_device {}() });

    std::vector<edge_t*> answer {};

    for (int i = 0; i < v_count - 1; i++) {
        edge_t e = edge_t {
            .tail = vertices[i],
            .head = vertices[i + 1],
            .edge_data = gr::DijkstraEdge(i)
        };
        gr.edges.push_back(e);
        auto* e_ptr = &gr.edges.back();
        e.tail->edges.push_back(e_ptr);
        e.head->edges.push_back(e_ptr);
        answer.push_back(e_ptr);
    }

    // now create new suboptimal connections
    std::shuffle(vertices.begin(), vertices.end(), std::mt19937 { std::random_device {}() });

    for (int i = 0; i < v_count - 1; i++) {
        edge_t e = edge_t {
            .tail = vertices[i],
            .head = vertices[i + 1],
            .edge_data = gr::DijkstraEdge(i + 2 * v_count)
        };
        if (std::find_if(gr.edges.begin(), gr.edges.end(), [&](edge_t& gr_e) {
                return e == gr_e;
            })
            == gr.edges.end()) {

            gr.edges.push_back(e);
            auto* e_ptr = &gr.edges.back();
            e.tail->edges.push_back(e_ptr);
            e.head->edges.push_back(e_ptr);
        }
    }

    return answer;
}

void test_kruskal()
{
    graph_t gr;
    auto ans = setup_graph_with_answer(gr);
    auto guess = gr::kruskal_mst(gr);
    std::sort(ans.begin(), ans.end(), [](edge_t* a , edge_t* b) {
                return a->head->node_data.name < b->head->node_data.name;
            });
    std::sort(guess.begin(), guess.end(), [](edge_t* a , edge_t* b) {
                return a->head->node_data.name < b->head->node_data.name;
            });
    // std::ranges::for_each(ans, [](edge_t* e) { std::println("{}", e->edge_data.dijkstra_score); });
    // std::ranges::for_each(guess, [](edge_t* e) { std::println("{}", e->edge_data.dijkstra_score); });

    assert(ans.size() == guess.size());
    assert(std::equal(guess.begin(), guess.end(), ans.begin(), ans.end(), [](auto a, auto b) { return a == b; }));
}

int main(void)
{
    for(auto i = 0; i < 100; i++)
        test_kruskal();
}
