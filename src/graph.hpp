#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <list>

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
public:
    std::list<Node> nodes{};
    std::list<Edge> edges{};

    template<int N>
    inline Graph<T> from_matrix(std::array<int, N> mtx) {
        static_assert<
    }
};


#endif
