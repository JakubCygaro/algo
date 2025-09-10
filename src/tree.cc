#include "datatypes.hpp"
#include <algorithm>
#include <iostream>
#include <print>
#include "common.hpp"

void test_select(){
    const auto size = common::get_random_in_range(5, 50);
    std::vector<int> arr(size);
    auto i = common::get_random_in_range(-1000, 1000);
    std::ranges::for_each(arr, [&](auto& v) {
            v = i;
            i += common::get_random_in_range(1, 50);
        });
    dt::RBTree<int, int> tree;
    std::ranges::shuffle(arr, std::mt19937 { std::random_device {}() });
    std::ranges::for_each(arr, [&](auto v){ tree.insert(v, std::move(v)); });
    std::ranges::sort(arr);
    for(auto i = 0; i < size; i++){
        auto [k, v] = tree.select(i);
        if(!v) continue;
        assert(k == arr[i]);
    }
}

void test_min_max(){
    const auto size = common::get_random_in_range(5, 50);
    std::vector<int> arr(size);
    auto i = common::get_random_in_range(-1000, 1000);
    std::ranges::for_each(arr, [&](auto& v) {
            v = i;
            i += common::get_random_in_range(1, 50);
        });
    dt::RBTree<int, int> tree;
    std::ranges::shuffle(arr, std::mt19937 { std::random_device {}() });
    std::ranges::for_each(arr, [&](auto v){ tree.insert(v, std::move(v)); });
    std::ranges::sort(arr);

    auto [min_k, min_v] = tree.min();
    assert(min_k == *min_v && *min_v == arr[0]);

    auto [max_k, max_v] = tree.max();
    assert(max_k == *max_v && *max_v == arr.back());
}
void test_search_and_delete(){
    const auto size = common::get_random_in_range(5, 50);
    std::vector<int> arr(size);
    auto i = common::get_random_in_range(-1000, 1000);
    std::ranges::for_each(arr, [&](auto& v) {
            v = i;
            i += common::get_random_in_range(1, 50);
        });
    dt::RBTree<int, int> tree;
    std::ranges::shuffle(arr, std::mt19937 { std::random_device {}() });
    std::ranges::for_each(arr, [&](auto v){ tree.insert(v, std::move(v)); });
    std::ranges::sort(arr);

    for(auto elem : arr){
        auto n = tree.search(elem);
        assert(n && "element not found when it was supposed to be");
        assert(elem == *n && "value found in tree does not match the value that was inserted for the same key");
        assert(tree.delete_element(elem) && "failed to delete element that was present in the tree");
    }
    assert(tree.empty() && "tree was not empty");
}

void test_predecessor_and_successor(){
    const auto size = common::get_random_in_range(5, 50);
    std::vector<int> arr(size);
    auto i = common::get_random_in_range(-1000, 1000);
    std::ranges::for_each(arr, [&](auto& v) {
            v = i;
            i += common::get_random_in_range(1, 50);
        });
    dt::RBTree<int, int> tree;
    std::ranges::shuffle(arr, std::mt19937 { std::random_device {}() });
    std::ranges::for_each(arr, [&](auto v){ tree.insert(v, std::move(v)); });
    std::ranges::sort(arr);

    for(auto i = 1; i < size; i++){
        auto [k, n] = tree.predecessor(arr[i]);
        assert(n && "predecessor not found");
        assert(k == arr[i - 1] && *n == arr[i - 1]);
    }
    for(auto i = 0; i < size - 1; i++){
        auto [k, n] = tree.successor(arr[i]);
        assert(n && "successor not found");
        assert(k == arr[i + 1] && *n == arr[i + 1]);
    }
}

void test_search_and_output_sorted(){
    const auto size = common::get_random_in_range(5, 50);
    std::vector<int> v(size);
    auto i = common::get_random_in_range(-1000, 1000);
    std::ranges::for_each(v, [&](auto& v) {
            v = i;
            i += common::get_random_in_range(1, 50);
        });
    dt::RBTree<int, int> tree;
    std::ranges::shuffle(v, std::mt19937 { std::random_device {}() });
    std::ranges::for_each(v, [&](auto v){ tree.insert(v, std::move(v)); });
    std::ranges::sort(v);

    for(auto elem : v){
        auto n = tree.search(elem);
        assert(elem == *n && "value found in tree does not match the value that was inserted for the same key");
    }

    auto sorted = tree.output_sorted();

    assert(sorted.size() == v.size() && "sorted tree output not mathing the size of input vector");

    for(auto i = 0; i < size; i++){
        auto [k, n] = sorted[i];
        assert(v[i] == k && v[i] == n && "sorted output element not matching input");
    }
}
int main(void){
    // std::vector<int> v(15);
    // auto i = 100;
    // std::ranges::for_each(v, [&](auto& v) { v = i++; });
    // dt::RBTree<int, int> tree;
    // std::ranges::shuffle(v, std::mt19937 { std::random_device {}() });
    // std::ranges::for_each(v, [&](auto v){ tree.insert(v, std::move(v)); });
    // // tree.insert(1, 100);
    // // tree.insert(2, 101);
    // // tree.insert(3, 102);
    // std::cout << *tree.search(102) << std::endl;
    // std::cout << *std::get<1>(tree.min()) << std::endl;
    // std::cout << *std::get<1>(tree.max()) << std::endl;
    //
    // tree.delete_element(102);
    //
    // for(auto i = 1; i <= 15; i++){
    //     auto [k, n] = tree.select(i);
    //     if(n){
    //         std::println("( {}, {} )", k, *n);
    //     }
    // }
    //
    // auto sorted = tree.output_sorted();
    //
    // std::println("{}", sorted);

    for(auto i = 0; i < 100; i++){
        test_search_and_output_sorted();
    }
    for(auto i = 0; i < 100; i++){
        test_predecessor_and_successor();
    }
    for(auto i = 0; i < 100; i++){
        test_search_and_delete();
    }
    for(auto i = 0; i < 100; i++){
        test_min_max();
    }
    for(auto i = 0; i < 100; i++){
        test_select();
    }
}
