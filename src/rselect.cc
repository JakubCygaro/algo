
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>
#include <random>

namespace{
    inline static std::random_device rd;
    inline static std::mt19937 gen(rd());

    int get_random_in_range(int low, int high) {
        return std::uniform_int_distribution<int> (low, high)(gen);
    }
}

int partition(int* arr, int left, int right, int pivot) {
    std::swap(arr[left], arr[pivot]);

    int i{ left + 1 }, j{ i };

    for(; j < right + 1; j++) {
        if (arr[j] <= arr[left]) {
            std::swap(arr[i], arr[j]);
            i++;
        }
    }
    std::swap(arr[left], arr[i - 1]);

    return i - 1;
}

int* rselect(int ith_element, int *arr, int left, int right) {
    if(right <= left) return &arr[left];

    // std::cout << "========\n";

    auto pivot = get_random_in_range(left, right);
    // std::cout << "pivot_v: " << arr[pivot] << std::endl;
    //
    // for (auto i = left; i <= right; i++) {
    //     std::cout << arr[i] << ' ';
    // }
    // std::cout << std::endl;
    // std::cout << "before partition" << std::endl;
    // for (auto i = left; i <= right; i++) {
    //     std::cout << arr[i] << ' ';
    // }
    // std::cout << std::endl;

    auto j = partition(arr, left, right, pivot);

    // std::cout << "after partition" << std::endl;
    // for (auto i = left; i <= right; i++) {
    //     std::cout << arr[i] << ' ';
    // }
    // std::cout << std::endl;
    // std::cout << "j: " << j << std::endl;
    // std::cout << "ith_element: " << ith_element << std::endl;
    // std::cout << "ith_element + left: " << ith_element + left << std::endl;

    if (j == ith_element)
        return &arr[ith_element];

    if (j > ith_element){
        return rselect(ith_element, arr, left, j - 1);
    } else {
        return rselect(ith_element, arr, j + 1, right);
    }

}

int main(void) {
    std::vector<int> input{};

    for (auto i = 0; i < 100; i++){

        input.resize(get_random_in_range(1, 1000));

        auto random_pos = get_random_in_range(0, input.size() - 1);

        std::ranges::iota(input, 0);

        auto random_val = input[random_pos];

        std::ranges::shuffle(input, std::mt19937 { std::random_device {}() });

        if (auto v = rselect(random_pos, input.data(), 0, input.size() - 1); !v || *v != random_val) {
            assert(false);
        }

    }

}
