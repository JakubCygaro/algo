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


size_t partition(int* array, int left, int right, int pivot){

    std::swap(array[pivot], array[left]); // pivot goes at the front

    pivot = left;

    int i { pivot + 1 }, j{ i };

    for(; j < right + 1; j++){
        if(array[j] <= array[pivot]){
            std::swap(array[i], array[j]);
            i++;
        }
    }
    std::swap(array[pivot], array[i - 1]);

    return i - 1;
}

void quick_sort(int* array, int left, int right) {
    if (left >= right) return;

    auto pivot = get_random_in_range(left, right);

    pivot = partition(array, left, right, pivot);

    quick_sort(array, left, pivot - 1);
    quick_sort(array, pivot + 1, right);
}
void validate(std::vector<int>& arr){
    for (auto i = 1; i < arr.size(); i++){
        assert(arr[i] >= arr[i-1]);
    }
}
int main(void) {
    for (auto i = 0; i < 100; i++){
        std::srand(time(0));
        std::vector<int> input{};
        input.resize(rand() % 100);
        std::for_each(input.begin(), input.end(), [&](auto& v){
            v = rand() % 1000;
        });
        quick_sort(input.data(), 0, input.size() - 1);
        validate(input);
    }
}
