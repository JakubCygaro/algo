#include <algorithm>
#include <cassert>
#include <ctime>
#include <vector>
#include "sort.hpp"

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
        sort::insertion_sort(input);
        validate_min(input);
    }
}
