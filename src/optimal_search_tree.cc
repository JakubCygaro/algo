#include <algorithm>
#include <limits>
#include <print>
#include <vector>
struct Node {
    int key{};
    int freq{};
    Node(int k, int f) : key{k}, freq{f} {}
};

int main(void) {
    std::vector<Node*> nodes = {
        new Node(10, 2),
        new Node(2, 3),
        new Node(20, 6),
        new Node(5, 1),
        new Node(1, 0),
    };

    const size_t cache_sz = nodes.size() + 1;
    const size_t N = nodes.size();

    int cache[cache_sz][cache_sz];

    for(size_t i = 1; i <= cache_sz; i++)
        cache[i][i - 1] = 0;

    for(size_t s = 0; s <= N - 1; s++){
        for(size_t i = 1; i <= N - s; i++){
            float freq_sum{};
            for(auto k = i; k < i + s; k++){
                freq_sum += nodes[k]->freq;
            }
            int min = std::numeric_limits<int>::max();
            for(auto r = i; r < i + s; r++){
                min = std::min(min, cache[i][r-1] + cache[r+1][i+s]);
            }
            cache[i][i+s] = freq_sum + min;
        }
    }

    auto sol = cache[1][N];

    std::println("{}", sol);

    std::for_each(nodes.begin(), nodes.end(), [](auto n){ delete n; });
}
