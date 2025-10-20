#include <iostream>
#include <print>
#include <algorithm>
#include <cassert>
#include <ctime>
#include <datatypes.hpp>
#include <functional>
#include <vector>
#include <common.hpp>

void cherry_pick_heapify(){
    std::vector<std::tuple<int, int>> arr = {
        {877, 0}, {993, 1}, {-930, 2}, {789, 3}, {-216, 4}, {-646, 5}, {-132, 6}, {631, 7}, {-726, 8}, {-254, 9}, {-192, 10}, {619, 11}, {334, 12}, {809, 13}, {-204, 14}, {-224, 15}, {-707, 16}, {-382, 17}, {702, 18}, {981, 19}, {499, 20}, {-298, 21}, {-436, 22}, {-316, 23}, {-792, 24}, {-426, 25}, {684, 26}, {-499, 27}, {375, 28}, {-63, 29}, {-921, 30}, {-468, 31}, {-482, 32}, {-737, 33}, {-313, 34}, {-900, 35}, {-679, 36}, {-511, 37}, {-855, 38}, {-28, 39}, {458, 40}, {-300, 41}, {635, 42}, {-314, 43}, {941, 44}, {-245, 45}, {-423, 46}, {42, 47}, {-901, 48}, {-890, 49}, {-475, 50}, {325, 51}, {-483, 52}, {-807, 53}, {-862, 54}, {-590, 55}, {447, 56}, {-742, 57}, {-638, 58}, {-810, 59}, {47, 60}, {-620, 61}
    };
    const auto size = arr.size();
    auto heap = dt::MinHeap<int, int>(size);
    decltype(arr)::reverse_iterator it = arr.rbegin();
    for(; it != arr.rend(); it++){
        auto [k, v] = *it;
        heap.insert(k, v);
    }
    auto backup = heap;
    assert(heap.size() == arr.size());
    for(auto i = size; i > 0; i--){
        auto [k, v] = heap.search(std::get<1>(arr[i - 1]));
        assert(v != nullptr && "inserted value not found in heap");
        assert(*v == std::get<1>(arr[i - 1]) && "found value is different than what was inserted");
        assert(k == std::get<0>(arr[i - 1]) && "key was different");
        assert(heap.delete_element(v) && "failed to delete element");
    }
    assert(heap.empty());
}

void verify_heapify(){
    auto const size = common::get_random_in_range(1, 20);
    std::vector<std::tuple<int, int>> arr(size);
    auto i = 0;
    std::ranges::for_each(arr, [&](auto& elem){
        elem = { common::get_random_in_range(-1000, 1000), i++ };
    });
    auto heap = dt::MinHeap<int, int>::heapify(arr);

    std::sort(arr.begin(), arr.end());
    for(auto i = 0; i < size; i++){
        auto [k, v] = heap.extract();
        auto [rk, rv] = arr[i];
        assert(k == rk);
    }
}

void test_heap_duplicates(){
    std::vector<std::tuple<int, int>> arr = {
        {1, 1},
        {1, 1},
        {1, 1},
        {1, 1},
        {1, 1},
    };
    auto heap = dt::MinHeap<int, int>::heapify(arr);
    assert(heap.delete_element(std::get<1>(heap.search(1))));
    assert(heap.delete_element(std::get<1>(heap.search(1))));
    assert(heap.delete_element(std::get<1>(heap.search(1))));
    assert(heap.delete_element(std::get<1>(heap.search(1))));
    assert(heap.delete_element(std::get<1>(heap.search(1))));
    assert(heap.empty());
}

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
            auto [k, v] = heap.search(std::get<1>(arr[i]));
            assert(heap.delete_element(v) && "failed to delete element");
        }
    }

    // searching for every element and deleting till empty
    heap = dt::MinHeap<int, int>::heapify(arr);
    assert(heap.size() == arr.size());
    for(auto i = 0; i < size; i++){
        auto [k, v] = heap.search(std::get<1>(arr[i]));
        assert(v != nullptr && "inserted value not found in heap");
        assert(heap.delete_element(v) && "failed to delete element");
    }
    assert(heap.empty() && "heap was supposed to be empty");

    heap = decltype(heap)::heapify(arr);
    for(auto i = size; i > 0; i--){
        auto [k, v] = heap.search(std::get<1>(arr[i - 1]));
        assert(v != nullptr && "inserted value not found in heap");
        assert(heap.delete_element(v) && "failed to delete element");
    }
    assert(heap.empty());



    heap = decltype(heap)();
    decltype(arr)::reverse_iterator it = arr.rbegin();
    for(; it != arr.rend(); it++){
        auto [k, v] = *it;
        heap.insert(k, v);
    }
    auto backup = heap;
    assert(heap.size() == arr.size());
    for(auto i = size; i > 0; i--){
        auto [k, v] = heap.search(std::get<1>(arr[i - 1]));
        assert(v != nullptr && "inserted value not found in heap");
        assert(*v == std::get<1>(arr[i - 1]) && "found value is different than what was inserted");
        assert(k == std::get<0>(arr[i - 1]) && "key was different");
        assert(heap.delete_element(v) && "failed to delete element");
    }
    assert(heap.empty());
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
    cherry_pick_heapify();
    for(auto i = 0; i < 100; i++)
        verify_heapify();
    for(auto i = 0; i < 100; i++ )
        heapify_with_search_and_delete();
    test_heap_duplicates();
}
