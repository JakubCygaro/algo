#include "common.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <ranges>
#include <span>
#include <utility>
#include <vector>
template <std::size_t Dim>
struct point;

template <>
struct point<3> {
    union {
        struct {
            float x { }, y { }, z { };
        };
        float arr_[3];
    };
};

template <class T>
using comparer_fn_t = std::function<
    bool(const T&, const T&)>;
template <class T>
constexpr bool default_comparer(const T& a, const T& b)
{
    return a >= b;
}
template <>
constexpr bool default_comparer(const point<3>& a, const point<3>& b)
{
    return a.x >= b.x;
}

template <class T>
size_t partition(
    T* array,
    int left,
    int right,
    int pivot,
    comparer_fn_t<T> comp = nullptr)
{

    std::swap(array[pivot], array[left]); // pivot goes at the front

    pivot = left;

    int i { pivot + 1 }, j { i };

    for (; j < right + 1; j++) {
        if (
            comp
                ? comp(array[pivot], array[j])
                : default_comparer(array[pivot], array[j])) {
            std::swap(array[i], array[j]);
            i++;
        }
    }
    std::swap(array[pivot], array[i - 1]);

    return i - 1;
}
// comp like left >= right
template <class T>
void quick_sort(
    T* array,
    int left,
    int right,
    comparer_fn_t<T> comp = nullptr)
{
    if (left >= right)
        return;

    auto pivot = common::get_random_in_range(left, right);

    pivot = partition<T>(array, left, right, pivot);

    quick_sort<T>(array, left, pivot - 1, comp);
    quick_sort<T>(array, pivot + 1, right, comp);
}

template <class T>
std::pair<T&, std::size_t> median(std::span<T> xs,
    comparer_fn_t<T> comp = nullptr)
{
    quick_sort<T>(xs.data(), 0, xs.size() - 1, comp);
    return { xs[xs.size() / 2], xs.size() / 2 };
}


template <std::size_t Dim>
class KDTree {
    using point_t = point<Dim>;
    struct Node {
        point_t location;
        std::shared_ptr<Node> left { }, right { };
    };
    std::shared_ptr<Node> root { };
    static_assert(Dim >= 2, "Dim too small");
    static std::shared_ptr<Node> _build_node(
        std::span<point<Dim>> points,
        std::size_t depth)
    {
        const auto axis = depth % Dim;
        const auto by_axis = [=](const point_t& a, const point_t& b) {
            return a.arr_[axis] >= b.arr_[axis];
        };
        auto [med, med_idx] = median<point<Dim>>(points, by_axis);
        auto this_node = std::make_shared<Node>();
        this_node->location = med;
        std::span<point_t> left_span = points
                                           .subspan(0, med_idx - 1);
        std::span<point_t> right_span = points
                                            .subspan(med_idx + 1, points.size() - med_idx);
        if (left_span.size() > 0)
            this_node->left = _build_node(left_span, depth + 1);
        if (right_span.size() > 0)
            this_node->right = _build_node(right_span, depth + 1);
        return this_node;
    }

public:
    static KDTree build_tree(std::span<point<Dim>> points)
    {
        auto tree = KDTree<Dim>();
        tree.root = _build_node(points, 0);
        return tree;
    }
};

int main(void)
{
    std::vector<point<3>> points = {

    };
    KDTree t = KDTree<3>::build_tree(points);
}
