#include <cassert>
#include <ctime>
#include <datatypes.hpp>
#include <functional>
#include <vector>
void validate(std::vector<int>& arr){
    for (std::size_t i = 1; i < arr.size(); i++){
        assert(arr[i] >= arr[i-1]);
    }
}
void heap_sort(std::vector<int>& v) {
    dt::Heap<int> heap(v.size());
    auto trans = std::bind(&dt::Heap<int>::insert, &heap, std::placeholders::_1);
    std::ranges::for_each(v, trans);
    std::ranges::for_each(v, [&](auto& e){ e = heap.extract_min(); });
}
int main(void) {
    for (auto i = 0; i < 100; i++){
        std::srand(time(0));
        std::vector<int> input{};
        input.resize(rand() % 100);
        std::for_each(input.begin(), input.end(), [&](auto& v){
            v = rand() % 1000;
        });
        heap_sort(input);
        validate(input);
    }
}
