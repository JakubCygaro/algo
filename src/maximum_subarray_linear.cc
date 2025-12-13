#include <array>
#include <print>

typedef std::pair<int, int> slice_t;
typedef std::pair<slice_t, int> max_subarray_t;

max_subarray_t find_maximum_subarray(int* arr, int len)
{
    int low {}, high {};
    int j {};
    int max_sum = 0;
    int ending_here_sum = 0;
    int ending_here_high = 0;
    int ending_here_low = 0;
    for (j = 0; j < len; j++) {
        ending_here_high = j;
        if (ending_here_sum > 0) {
            ending_here_sum += arr[j];
        } else {
            ending_here_low = j;
            ending_here_sum = arr[j];
        }
        if (ending_here_sum > max_sum) {
            max_sum = ending_here_sum;
            low = ending_here_low;
            high = ending_here_high;
        }
    }
    return max_subarray_t { { low, high }, max_sum };
}

int main(void)
{
    std::array input = {
        13, -3, -25, 20, -3, -16, -23, 18, 20, -7, 12, -5, -22, 15, -4, 7
    };
    auto [max_slice, max_sum] = find_maximum_subarray(input.data(), input.size());
    auto [l, h] = max_slice;
    std::println("sum: {}", max_sum);
    for (int i = l; i <= h; i++) {
        std::print("{}, ", input[i]);
    }
    std::println();

}
