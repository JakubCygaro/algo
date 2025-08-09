#include <cassert>
#include <ctime>
#include <datatypes.hpp>
#include <functional>
#include <print>
#include <vector>

void heapify() {
    std::vector<std::tuple<int, int>> arr {  { 8, 1 }, { 7, 2 },{ 6, 3 },{ 5, 4 },{ 4, 5 },};
    auto heap = dt::MinHeap<int>::heapify(std::move(arr));
    // while(!heap.empty()) {
    //     std::print("{} ", heap.extract());
    // }
    // std::println("");
    auto [key, v] = heap.search(3);
    if(v){
        std::println("{} {}", key, *v);
        heap.delete_element(v);
        while(!heap.empty()){
            std::print("{} ", heap.extract());
        }
        std::println("");
    }
}

void validate_max(std::vector<int>& arr){
    for (std::size_t i = 1; i < arr.size(); i++){
        assert(arr[i] <= arr[i-1]);
    }
}
void heap_sort_max(std::vector<int>& v) {
    dt::MaxHeap<int> heap(v.size());
    auto trans = std::bind(&dt::MaxHeap<int>::insert, &heap, std::placeholders::_1, std::placeholders::_1);
    std::ranges::for_each(v, trans);
    std::ranges::for_each(v, [&](auto& e){ e = std::get<0>(heap.extract()); });
}
void validate_min(std::vector<int>& arr){
    for (std::size_t i = 1; i < arr.size(); i++){
        assert(arr[i] >= arr[i-1]);
    }
}
void heap_sort_min(std::vector<int>& v) {
    dt::MinHeap<int> heap(v.size());
    auto trans = std::bind(&dt::MinHeap<int>::insert, &heap, std::placeholders::_1, std::placeholders::_1);
    std::ranges::for_each(v, trans);
    std::ranges::for_each(v, [&](auto& e){ e = std::get<0>(heap.extract()); });
}
int main(void) {
    heapify();
    for (auto i = 0; i < 100; i++){
        std::srand(time(0));
        std::vector<int> input{};
        input.resize(rand() % 100);
        std::for_each(input.begin(), input.end(), [&](auto& v){
            v = rand() % 1000;
        });
        heap_sort_min(input);
        validate_min(input);
    }
    for (auto i = 0; i < 100; i++){
        std::srand(time(0));
        std::vector<int> input{};
        input.resize(rand() % 100);
        std::for_each(input.begin(), input.end(), [&](auto& v){
            v = rand() % 1000;
        });
        heap_sort_max(input);
        validate_max(input);
    }
}
