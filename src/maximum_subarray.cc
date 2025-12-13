#include <array>
#include <cassert>
#include <limits>
#include <optional>
#include <print>
#include <utility>

typedef std::pair<int, int> slice_t;
typedef std::pair<slice_t, int> max_subarray_t;

const constexpr int INF = std::numeric_limits<int>::max();

max_subarray_t find_max_crossing(int* arr, int low, int mid, int high)
{
    int max_left {}, max_right {};
    std::optional<int> left_sum;
    int sum = 0;
    for (int i = mid; i >= low; i--) {
        sum += arr[i];
        if (sum > left_sum.value_or(-INF)) {
            left_sum = sum;
            max_left = i;
        }
    }
    sum = 0;
    std::optional<int> right_sum;

    for (int i = mid + 1; i <= high; i++) {
        sum += arr[i];
        if (sum > right_sum.value_or(-INF)) {
            right_sum = sum;
            max_right = i;
        }
    }
    sum = left_sum && right_sum ? left_sum.value() + right_sum.value() : -INF;
    return max_subarray_t { { max_left, max_right }, sum };
}

max_subarray_t find_max_subarray(int* arr, slice_t bounds)
{
    auto [low, high] = bounds;
    if (low == high)
        return max_subarray_t { bounds, arr[low] };
    else {
        auto mid = (low + high) / 2;
        assert(low <= high && low >= 0);

        auto [left_slice, left_sum] = find_max_subarray(arr, slice_t { low, mid });
        auto [left_low, left_high] = left_slice;

        auto [right_slice, right_sum] = find_max_subarray(arr, slice_t { mid + 1, high });
        auto [right_low, right_high] = right_slice;

        auto [cross_slice, cross_sum] = find_max_crossing(arr, left_low, mid, right_high);

        if (left_sum >= right_sum && left_sum >= cross_sum) {
            return { left_slice, left_sum };
        } else if (right_sum >= left_sum && right_sum >= cross_sum) {
            return { right_slice, right_sum };
        } else {
            return { cross_slice, cross_sum };
        }
    }
}

int main(void)
{
    std::array input = {
        13, -3, -25, 20, -3, -16, -23, 18, 20, -7, 12, -5, -22, 15, -4, 7
    };
    auto [max_slice, max_sum] = find_max_subarray(input.data(), { 0, input.size() - 1 });
    auto [l, h] = max_slice;
    std::println("sum: {}", max_sum);
    for (int i = l; i <= h; i++) {
        std::print("{}, ", input[i]);
    }
    std::println();
}
