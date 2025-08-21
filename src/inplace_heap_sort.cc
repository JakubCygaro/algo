#include <algorithm>
#include <cassert>
#include <cstddef>
#include <optional>
#include <print>
#include <vector>


std::optional<size_t> parent(size_t i){
    if (i == 0) return std::nullopt;
    if (i % 2 == 0){
        i--;
    }
    return i / 2;
}
std::optional<size_t> left_child(size_t sz, size_t i){
    auto ret = i * 2 + 1;
    if(ret < sz)
        return ret;
    else
        return std::nullopt;
}
std::optional<size_t> right_child(size_t sz, size_t i){
    auto ret = i * 2 + 2;
    if(ret < sz)
        return ret;
    else
        return std::nullopt;
}
std::optional<size_t> sift_down(std::vector<int>& arr, size_t sz, size_t i){
    auto rc = right_child(sz, i);
    auto lc = left_child(sz, i);

    if(rc && lc){
        auto min_child = arr[lc.value()] < arr[rc.value()] ? lc.value() : rc.value();
        if (arr[i] > arr[min_child]){
            std::swap(arr[i], arr[min_child]);
            return min_child;
        }
    } else if (lc){
        if (arr[i] > arr[lc.value()]){
            std::swap(arr[i], arr[lc.value()]);
            return lc.value();
        }
    } else if (rc){
        if (arr[i] > arr[rc.value()]){
            std::swap(arr[i], arr[rc.value()]);
            return rc.value();
        }
    }
    return std::nullopt;
}
void heapify(std::vector<int>& arr){
    for (auto i = arr.size() - 1; i != 0; i--){
        if(auto p = parent(i); p.has_value()){
            while(auto j = sift_down(arr, arr.size(), p.value())){
                p = j;
            }
        }
    }
}
size_t extract_min(std::vector<int>& arr, size_t sz){
    if (sz == 1) return --sz;
    std::swap(arr[0], arr[--sz]);
    std::optional<size_t> i = 0;
    while(auto j = sift_down(arr, sz, i.value())){
        i = j;
    }
    return sz;
}
void inplace_heap_sort(std::vector<int>& arr){
    heapify(arr);

    auto sz = arr.size();
    while(sz != 0){
        sz = extract_min(arr, sz);
    }
}
void validate_min(std::vector<int>& arr){
    for (std::size_t i = 1; i < arr.size(); i++){
        assert(arr[i] >= arr[i-1]);
    }
}
int main(void){
    for (auto i = 0; i < 100; i++){
        std::srand(time(0));
        std::vector<int> input{};
        input.resize(rand() % 100);
        std::for_each(input.begin(), input.end(), [&](auto& v){
            v = rand() % 1000;
        });
        inplace_heap_sort(input);
        std::ranges::reverse(input);
        validate_min(input);
    }
}
