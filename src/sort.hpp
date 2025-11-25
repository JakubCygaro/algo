#ifndef SORT_HPP
#define SORT_HPP
#include <cstddef>
#include <utility>
#include <common.hpp>

namespace sort {
    namespace {
        template<typename T>
        size_t partition(T* array, int left, int right, int pivot){
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
    }
template<typename T>
void quick_sort(T* array, int left, int right) {
    if (left >= right) return;

    auto pivot = common::get_random_in_range(left, right);

    pivot = partition(array, left, right, pivot);

    quick_sort(array, left, pivot - 1);
    quick_sort(array, pivot + 1, right);
}
template<typename T>
std::vector<T> merge_sort(std::vector<T>& arr)
{
    const auto len = arr.size();
    const auto mid = (len / 2) - 1;
    const auto r = mid + 1;

    if (len <= 1)
        return arr;
    std::vector<T> output{};
    output.resize(len);

    std::vector<T> left{};
    left.resize(r);
    std::copy(arr.begin(), arr.begin() + r, left.begin());

    std::vector<T> right{};
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
namespace {
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
    template<typename T>
    std::optional<size_t> sift_down(std::vector<T>& arr, size_t sz, size_t i){
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
    template<typename T>
    void heapify(std::vector<T>& arr){
        for (auto i = arr.size() - 1; i != 0; i--){
            if(auto p = parent(i); p.has_value()){
                while(auto j = sift_down(arr, arr.size(), p.value())){
                    p = j;
                }
            }
        }
    }
    template<typename T>
    size_t extract_min(std::vector<T>& arr, size_t sz){
        if (sz == 1) return --sz;
        std::swap(arr[0], arr[--sz]);
        std::optional<size_t> i = 0;
        while(auto j = sift_down(arr, sz, i.value())){
            i = j;
        }
        return sz;
    }
}
template<typename T>
void inplace_heap_sort(std::vector<T>& arr){
    heapify(arr);

    auto sz = arr.size();
    while(sz != 0){
        sz = extract_min(arr, sz);
    }
}
}

#endif
