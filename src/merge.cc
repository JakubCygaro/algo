#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <vector>
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
void validate_min(std::vector<int>& arr){
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
        input = merge_sort(input);
        validate_min(input);
    }
}

