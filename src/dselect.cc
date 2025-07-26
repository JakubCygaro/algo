
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <format>
#include <iostream>
#include <random>
#include <vector>
#include "common.hpp"

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
std::vector<int> merge_sort(std::vector<int>& arr)
{
    const auto len = arr.size();
    const auto mid = (len / 2) - 1;
    const auto r = mid + 1;

    if (len <= 1)
        return arr;
    std::vector<int> output{};
    output.resize(len);

    std::vector<int> left{};
    left.resize(r);
    std::copy(arr.begin(), arr.begin() + r, left.begin());

    std::vector<int> right{};
    right.resize(len - r);
    std::copy(arr.begin() + r, arr.end(), right.begin());

    left = merge_sort(left);
    right = merge_sort(right);

    size_t i{0}, j{0};
    for(size_t k = 0; k < len; k++){
        if(i < r && j < (len - r) && left[i] < right[j]){
            output[k] = left[i];
            i++;
        } else if (j < (len - r)){
            output[k] = right[j];
            j++;
        } else if (i < r){
            output[k] = left[i];
            i++;
        }
    }
    return output;
}

int dselect(std::vector<int>& array, const int ith) {
    if(array.size() == 1) {
        return array[0];
    }

    std::vector<int> first_round{};

    first_round.resize((int)std::ceil(array.size() / 5.));

    assert(first_round.size() != 0);

    for(auto h = 0; h < first_round.size(); h++) {
        auto start = h * 5;
        auto end = start + 5;
        if (end > array.size()) {
            end -= end - array.size();
        }
        std::vector<int> to_sort{};

        auto size = end - start;

        to_sort.resize(size);


        std::copy(array.begin() + start, array.begin() + end, to_sort.begin());

        auto sorted = merge_sort(to_sort);

        first_round[h] = sorted[size / 2];
    }

    auto p = dselect(first_round, (array.size() / 10));

    int j;
    for(auto i = 0; i < array.size(); i++) {
        if(array[i] == p){
            j = i;
        }
    }

    j = partition(array.data(), 0, array.size() - 1, j);

    if (j == ith){
        return p;
    }
    else if (j > ith){
        std::vector<int> first_part{};
        first_part.resize(j);
        std::copy(array.begin(), array.begin() + j, first_part.begin());
        return dselect(first_part, ith);
    }
    else {
        std::vector<int> second_part{};
        second_part.resize(std::ceil(array.size() - j - 1));
        std::copy(array.begin() + j + 1, array.end(), second_part.begin());
        return dselect(second_part, ith - j - 1);
    }
}



int main(void) {
    std::vector<int> input{};

    for (auto i = 0; i < 100; i++){

        input.resize(common::get_random_in_range(1, 1000));

        auto random_pos = common::get_random_in_range(0, input.size() - 1);

        std::ranges::iota(input, 0);

        auto random_val = input[random_pos];

        std::ranges::shuffle(input, std::mt19937 { std::random_device {}() });

        assert(dselect(input, random_pos) == random_val && "wrong selection");

    }
}
