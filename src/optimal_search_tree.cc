#include <algorithm>
#include <cassert>
#include <print>
#include <vector>
struct Node {
    int key{};
    int freq{};
    Node(int k, int f) : key{k}, freq{f} {}
};

int main(void) {
    std::vector<Node*> nodes = {
        new Node(1, 2),
        new Node(2, 3),
        new Node(3, 6),
        new Node(4, 1),
        new Node(5, 12),
    };
    const size_t cache_sz = nodes.size() + 1;
    const size_t N = nodes.size();

    std::sort(nodes.begin(), nodes.end(), [](auto a, auto b) { return a->key < b->key; });

    int cache[cache_sz][cache_sz];

    for(auto i = 0ul; i < cache_sz; i++) {
        for(auto j = 0ul; j < cache_sz; j++) {
            cache[i][j] = 0;
        }
    }

    for(size_t i = 1; i <= cache_sz; i++)
        cache[i][i - 1] = 0;

    for(size_t s = 0; s < N; s++){
        for(size_t i = 1; i <= N - s; i++){
            int freq_sum{};
            for(auto k = i; k <= i + s; k++){
                freq_sum += nodes[k - 1]->freq;
            }
            int min = cache[i][i];
            for(auto r = i; r < i + s; r++){
                assert(i < cache_sz);
                assert(r-1 < cache_sz);
                assert(r+1 < cache_sz);
                assert(i+s < cache_sz);
                auto tmp = cache[i][r-1] + cache[r+1][i+s];
                min = std::min(min, tmp);
            }
            cache[i][i+s] = freq_sum + min;
        }
    }

    for(auto i = 0ul; i < cache_sz; i++) {
        for(auto j = 0ul; j < cache_sz; j++) {
            std::print("\t{} ", cache[i][j]);
        }
        std::println();
    }
    auto sol = cache[1][N];

    std::println("{}", sol);

    std::for_each(nodes.begin(), nodes.end(), [](auto n){ delete n; });
}
