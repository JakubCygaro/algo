#include <algorithm>
#include <cassert>
#include <ctime>
#include <datatypes.hpp>
#include <functional>
#include <vector>
#include <common.hpp>

void heapify_with_search_and_delete() {
    auto const size = common::get_random_in_range(1, 100);
    std::vector<std::tuple<int, int>> arr(size);
    auto i = 0;
    std::ranges::for_each(arr, [&](auto& elem){
        elem = { common::get_random_in_range(-1000, 1000), i++ };
    });
    auto heap = dt::MinHeap<int, int>::heapify(arr);

    for(i = 0; i < size; i++){
        auto [k, v] = heap.search(std::get<1>(arr[i]));
        assert(v != nullptr && "inserted value not found in heap");
        assert(*v == std::get<1>(arr[i]) && "found value is different than what was inserted");
        assert(k == std::get<0>(arr[i]) && "key was different");
    }
    for(auto i = 0; i < size; i++){
        if(common::get_random_in_range(1, 100) < 50){
            assert(heap.delete_element(std::get<1>(heap.search(std::get<1>(arr[i])))) && "failed to delete element");
        }
    }
}

void validate_max(std::vector<int>& arr){
    for (std::size_t i = 1; i < arr.size(); i++){
        assert(arr[i] <= arr[i-1]);
    }
}
void heap_sort_max(std::vector<int>& v) {
    dt::MaxHeap<int, int> heap(v.size());
    auto trans = std::bind(&dt::MaxHeap<int, int>::insert, &heap, std::placeholders::_1, std::placeholders::_1);
    std::ranges::for_each(v, trans);
    std::ranges::for_each(v, [&](auto& e){ e = std::get<0>(heap.extract()); });
}
void validate_min(std::vector<int>& arr){
    for (std::size_t i = 1; i < arr.size(); i++){
        assert(arr[i] >= arr[i-1]);
    }
}
void heap_sort_min(std::vector<int>& v) {
    dt::MinHeap<int, int> heap(v.size());
    auto trans = std::bind(&dt::MinHeap<int, int>::insert, &heap, std::placeholders::_1, std::placeholders::_1);
    std::ranges::for_each(v, trans);
    std::ranges::for_each(v, [&](auto& e){ e = std::get<0>(heap.extract()); });
}
int main(void) {
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
    for(auto i = 0; i < 100; i++ )
        heapify_with_search_and_delete();
}
