#include "common.hpp"
#include "cum_assert.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <print>
#include <ranges>
#include <set>
#include <span>
#include <sstream>
#include <unordered_set>
#include <utility>
#include <vector>
template <std::size_t Dim>
struct point;

template <>
struct point<3> {
    using value_t = float;
    static constexpr std::size_t size = 3;
    union {
        struct {
            value_t x { }, y { }, z { };
        };
        value_t arr_[3];
    };
    point() = default;
    point(value_t x, value_t y, value_t z)
        : x { x }
        , y { y }
        , z { z }
    {
    }
    constexpr value_t& operator[](const std::size_t& i)
    {
        return arr_[i];
    }
    constexpr const value_t& operator[](const std::size_t& i) const
    {
        return arr_[i];
    }
    constexpr auto operator==(const point& other) const
    {
        return other.x == x && other.y == y && other.z == z;
    }
};
template <std::size_t Dim,
    class point_t = point<Dim>,
    class value_t = point_t::value_t,
    class index = std::make_index_sequence<Dim>>
constexpr value_t squared_distance(const point_t& a, const point_t& b)
{
    return [&]<std::size_t... I>(std::index_sequence<I...>) {
        return (((a.arr_[I] - b.arr_[I]) * (a.arr_[I] - b.arr_[I])) + ...);
    }(index { });
}
template <>
struct std::formatter<point<3>, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        auto it = ctx.begin();
        return it;
    }
    template <class FmtContext>
    FmtContext::iterator format(point<3> p, FmtContext& ctx) const
    {
        std::ostringstream out;
        out << std::format("{{ .x = {}, .y = {}, .z = {} }}", p.x, p.y, p.z);

        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};
template <>
struct std::formatter<point<2>, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        auto it = ctx.begin();
        return it;
    }
    template <class FmtContext>
    FmtContext::iterator format(point<3> p, FmtContext& ctx) const
    {
        std::ostringstream out;
        out << std::format("{{ {} {} }}", p.x, p.y);

        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};
using point3d = point<3>;
using vec3 = point<3>;
using point2d = point<2>;
using vec2 = point<2>;

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

    pivot = partition<T>(array, left, right, pivot, comp);

    quick_sort<T>(array, left, pivot - 1, comp);
    quick_sort<T>(array, pivot + 1, right, comp);
}

template <class T>
std::pair<T&, std::size_t> median(std::span<T> xs,
    comparer_fn_t<T> comp = nullptr)
{
    if (xs.size() == 1) {
        return { xs[0], 0 };
    }
    quick_sort<T>(xs.data(), 0, xs.size() - 1, comp);
    return { xs[xs.size() / 2], xs.size() / 2 };
}

template <std::size_t Dim>
class KDTree {
public:
    using point_t = point<Dim>;
    struct current_best_t {
        point_t point { };
        typename point_t::value_t sq_dist { };
    };

private:
    static_assert(Dim >= 2, "Dim too small");
    struct Node {
        Node() = default;
        Node(point_t pt)
            : location { pt }
        {
        }
        point_t location;
        std::shared_ptr<Node> left { }, right { };
    };
    std::shared_ptr<Node> root { };
    std::size_t m_size { 0 };
    static std::shared_ptr<Node> _build_node(
        std::span<point<Dim>> points,
        std::size_t depth)
    {
        if (points.size() == 0)
            return nullptr;
        const auto axis = depth % Dim;
        const auto by_axis = [=](const point_t& a, const point_t& b) {
            return a.arr_[axis] >= b.arr_[axis];
        };
        auto [med, med_idx] = median<point<Dim>>(points, by_axis);
        for (auto i = med_idx + 1; i < points.size();) {
            if (points[i].arr_[axis] == med.arr_[axis]) {
                i++;
            } else {
                med_idx = i - 1;
                break;
            }
        }
        // std::println("points: {}", points);
        auto this_node = std::make_shared<Node>();
        this_node->location = points[med_idx];
        if (points.size() == 1)
            return this_node;
        std::span<point_t> left_span = points
                                           .subspan(0, med_idx);
        std::span<point_t> right_span = points
                                            .subspan(med_idx + 1);
        if (left_span.size() > 0)
            this_node->left = _build_node(left_span, depth + 1);
        if (right_span.size() > 0)
            this_node->right = _build_node(right_span, depth + 1);
        return this_node;
    }
    void _add(point_t pt,
        std::shared_ptr<Node>& n,
        std::size_t depth)
    {
        const auto axis = depth % Dim;
        std::shared_ptr<Node>* into { };
        if (pt == n->location) {
            return;
        } else if (pt.arr_[axis] <= n->location.arr_[axis]) {
            into = &n->left;
        } else {
            into = &n->right;
        }
        // std::println("add {} on {} of {} | axis {}",
        // pt,
        // into == &(n->left) ? "left" : "right",
        // n->location,
        // axis);
        if (*into != nullptr) {
            _add(pt, *into, depth + 1);
        } else {
            *into = std::make_shared<Node>(pt);
            m_size++;
        }
    }
    std::pair<std::shared_ptr<Node>*, std::size_t> find_smallest(
        std::shared_ptr<Node>& n,
        const std::size_t on_axis,
        const std::size_t depth)
    {
        const auto axis = depth % Dim;
        if (axis == on_axis) {
            if (n->left)
                return find_smallest(n->left, on_axis, depth + 1);
            else
                return { &n, depth };
        }
        std::pair<std::shared_ptr<Node>*, std::size_t> smallest;
        if (n->left && n->right) {
            auto left_min = find_smallest(n->left, on_axis, depth + 1);
            auto right_min = find_smallest(n->right, on_axis, depth + 1);
            smallest = (*std::get<0>(left_min))->location.arr_[on_axis]
                    <= (*std::get<0>(right_min))->location.arr_[on_axis]
                ? left_min
                : right_min;
        } else if (n->right) {
            smallest = find_smallest(n->right, on_axis, depth + 1);
        } else if (n->left) {
            smallest = find_smallest(n->left, on_axis, depth + 1);
        } else {
            return { &n, depth };
        }
        if (n->location.arr_[on_axis]
            <= (*std::get<0>(smallest))->location.arr_[on_axis])
            return { &n, depth };
        else
            return smallest;
    }
    std::pair<std::shared_ptr<Node>*, std::size_t> find_biggest(
        std::shared_ptr<Node>& n,
        const std::size_t on_axis,
        const std::size_t depth)
    {
        const auto axis = depth % Dim;
        if (axis == on_axis) {
            if (n->right)
                return find_biggest(n->right, on_axis, depth + 1);
            else
                return { &n, depth };
        }
        std::pair<std::shared_ptr<Node>*, std::size_t> biggest;
        if (n->left && n->right) {
            auto left_min = find_biggest(n->left, on_axis, depth + 1);
            auto right_min = find_biggest(n->right, on_axis, depth + 1);
            biggest = (*std::get<0>(left_min))->location.arr_[on_axis]
                    >= (*std::get<0>(right_min))->location.arr_[on_axis]
                ? left_min
                : right_min;
        } else if (n->right) {
            biggest = find_biggest(n->right, on_axis, depth + 1);
        } else if (n->left) {
            biggest = find_biggest(n->left, on_axis, depth + 1);
        } else {
            return { &n, depth };
        }
        if (n->location.arr_[on_axis]
            >= (*std::get<0>(biggest))->location.arr_[on_axis])
            return { &n, depth };
        else
            return biggest;
    }
    bool _remove(const point_t& pt,
        std::shared_ptr<Node>* n,
        std::size_t depth)
    {
        const auto axis = depth % Dim;
        // no match case
        if ((*n)->location != pt) {
            const auto left = (*n)->location.arr_[axis] >= pt.arr_[axis];
            if (left && (*n)->left)
                return _remove(pt, &(*n)->left, depth + 1);
            else if ((*n)->right)
                return _remove(pt, &(*n)->right, depth + 1);
            else
                return false;
        }
        // match case
        // no children case
        if (!(*n)->left && !(*n)->right) {
            *n = nullptr;
            m_size--;
            return true;
        }
        // right child case
        if ((*n)->right) {
            auto [smol, d] = find_smallest((*n)->right, axis, depth + 1);
            // std::println("smol {}", (*smol)->location);
            (*n)->location = (*smol)->location;
            return _remove((*n)->location, smol, d);
        }
        // only left child case
        else {
            auto [big, d] = find_biggest((*n)->left, axis, depth + 1);
            // std::println("big {}", (*big)->location);
            (*n)->location = (*big)->location;
            return _remove((*n)->location, big, d);
        }
    }
    void _nn_search(
        const point_t& pt,
        std::shared_ptr<Node> n,
        std::optional<current_best_t>* current_best,
        const std::size_t depth)
    {
        const auto axis = depth % Dim;
        std::shared_ptr<Node>* into { };
        if (pt.arr_[axis] <= n->location.arr_[axis]) {
            into = &n->left;
        } else {
            into = &n->right;
        }
        const auto save_if_best = [&](std::shared_ptr<Node> node)
            -> decltype(squared_distance<Dim>(pt, pt)) {
            const auto dist = squared_distance<Dim>(
                node->location, pt);
            if (!current_best->has_value()
                || current_best->value().sq_dist > dist) {
                (*current_best) = current_best_t {
                    .point = node->location,
                    .sq_dist = dist,
                };
            }
            return current_best->value().sq_dist;
        };
        if (*into != nullptr) {
            save_if_best(*into);
            _nn_search(pt, *into, current_best, depth + 1);
            // unwind
            save_if_best(n);
            // plane crossing
            // simple comparison to see whether the distance between
            // the splitting coordinate of the search point and current node is less than
            // the distance (overall coordinates) from the search point to the current best.
            // if (squared_distance<Dim>(pt, n->location)
            //     < squared_distance<Dim>(pt, (*current_best)->point)) {
            //     auto other_branch = *into == n->left ? n->right : n->left;
            //     if (!other_branch)
            //         return;
            //     _nn_search(pt, other_branch, current_best, depth + 1);
            // }
            if (std::pow(pt.arr_[axis] - n->location.arr_[axis], 2.0)
                < squared_distance<Dim>(pt, (*current_best)->point)) {
                auto other_branch = *into == n->left ? n->right : n->left;
                if (!other_branch)
                    return;
                _nn_search(pt, other_branch, current_best, depth + 1);
            }
            return;
        } else {
            // leaf reached
            // std::println("leaf reached {}", n->location);
            save_if_best(n);
            return;
        }
    }

private:
    void print_tree_impl(std::shared_ptr<Node> x, int depth)
    {
        const auto axis = (depth / 2) % Dim;
        if (x->right)
            print_tree_impl(x->right, depth + 2);
        std::printf("%*d", x == root ? 0 : depth, x == root ? 0 : (int)axis);
        std::print("{}", x->location);
        std::printf("\n");
        if (x->left)
            print_tree_impl(x->left, depth + 2);
    }

public:
    KDTree() = default;
    static KDTree build_tree(std::span<point<Dim>> points)
    {
        auto tree = KDTree<Dim>();
        tree.m_size = points.size();
        tree.root = _build_node(points, 0);
        return tree;
    }
    void add(point_t pt)
    {
        if (root == nullptr) {
            root = std::make_shared<Node>();
            root->location = pt;
            m_size++;
        } else
            _add(pt, root, 0);
    }
    bool remove(const point_t& pt)
    {
        if (root != nullptr) {
            return _remove(pt, &root, 0);
        }
        return false;
    }
    std::optional<point_t> nn_search(const point_t& pt)
    {
        std::optional<current_best_t> current_best;
        if (!root)
            return { };
        _nn_search(pt, root, &current_best, 0);
        return current_best->point;
    }
    bool empty() const
    {
        return !!root;
    }
    void clear()
    {
        root = nullptr;
    }
    std::size_t size() const
    {
        return m_size;
    }
    void print_tree()
    {
        std::printf("======\n");
        print_tree_impl(root, 0);
        std::printf("======\n");
    }
};

template <>
struct std::hash<point3d> {
    constexpr std::size_t
    operator()(const point3d& a)
        const noexcept
    {
        return std::hash<float>()(a.x)
            ^ std::hash<float>()(a.y)
            ^ std::hash<float>()(a.z);
    }
};

constexpr std::size_t INPUT_POINT_ARR_SZ = 30;
constexpr auto INPUT_POINT_MAX = 50;

std::vector<point3d> make_test_input(void)
{
    std::vector<point3d> input;
    std::unordered_set<point3d> set;
    for (auto i = 0UL; i < INPUT_POINT_ARR_SZ; i++) {
        const auto p = point3d {
            (float)common::get_random_in_range(-INPUT_POINT_MAX, INPUT_POINT_MAX),
            (float)common::get_random_in_range(-INPUT_POINT_MAX, INPUT_POINT_MAX),
            (float)common::get_random_in_range(-INPUT_POINT_MAX, INPUT_POINT_MAX),
        };
        set.insert(p);
    }
    input.assign_range(set);
    return input;
}

void test_kd_tree_1(void)
{
    auto input = make_test_input();
    auto tree = KDTree<3>::build_tree(input);
    for (const point3d& p : input) {
        auto found = tree.nn_search(p);
        cum_assert(found.has_value(), "");
        cum_assert(*found == p, "wanted {} but got {}", p, *found);
    }
}

void test_kd_tree_2(void)
{
    auto input = make_test_input();
    auto tree = KDTree<3>();
    std::ranges::for_each(input,
        std::bind(&KDTree<3>::add, &tree, std::placeholders::_1));
    for (const point3d& p : input) {
        auto found = tree.nn_search(p);
        cum_assert(found.has_value(), "");
        cum_assert(*found == p, "wanted {} but got {}", p, *found);
    }
}
// TODO: only this test sometimes fails and the tree is fucked up with
// either equal elements to the right of a node or bigger to the left
// REMOVE must be broken - the problem is in the splitting plane,
// when the removed element has multiple valid and EQUAL (in respect to the axis)
// replacement points to its right, only one is chosen and the other remains on the right
// breaking the invariant. We have to fish all these points out and build a new left 
// substree with them to preserve it
void test_kd_tree_3(void)
{
    auto input = make_test_input();
    auto tree = KDTree<3>::build_tree(input);
    for (const point3d& p : input) {
        auto found = tree.nn_search(p);
        cum_assert(found.has_value(), "");
        cum_assert(*found == p, "wanted {} but got {}",
            p,
            *found,
            cum_eval(tree.print_tree(); std::flush(std::cout)));
        tree.remove(p);
        found = tree.nn_search(p);
        cum_assert(!found.has_value() || *found != p,
            "Looked for {}", p);
    }
}
void test_kd_tree_4(void)
{
    struct closest_t {
        point3d p;
        point3d::value_t sq_dist;
    };
    auto input1 = make_test_input();
    std::unordered_map<point3d, closest_t> closest_to;
    auto input2 = make_test_input();
    std::ranges::for_each(input1, [&](const auto& pa) {
        std::optional<closest_t> closest;
        for (const auto& pb : input2) {
            const auto dist = squared_distance<3>(pa, pb);
            if (!closest || closest->sq_dist > dist) {
                closest = closest_t {
                    .p = pb,
                    .sq_dist = dist,
                };
            }
        }
        closest_to[pa] = *closest;
    });
    auto tree = KDTree<3>::build_tree(input1);
    for (const point3d& p : input1) {
        auto found = tree.nn_search(p);
        cum_assert(found.has_value(), "");
        cum_assert(*found == closest_to[p].p
                || squared_distance<3>(closest_to[p].p, *found) == closest_to[p].sq_dist,
            "wanted {} (dist :: {}) but got {} (dist :: {})",
            closest_to[p].p, squared_distance<3>(closest_to[p].p, p),
            *found, squared_distance<3>(closest_to[p].p, *found),
            cum_eval(tree.print_tree(); std::flush(std::cout)));
    }
}

int main(void)
{
    test_kd_tree_1();
    test_kd_tree_2();
    test_kd_tree_3();
    test_kd_tree_4();
    return 0;
    std::vector<point<3>> points = {
        { 1, 2, 3 },
        { 2, 1, 3 },
        { 8, 6, 0 },
        { 4, 4, 4 },
        { 4, 4, 9 },
        { 4, 1, 4 },
        // { 0, 2, 3 },
        // { 1, 1, 1 },
    };
    std::flush(std::cout);
    KDTree t = KDTree<3>::build_tree(points);
    t.add({ 0, 2, 3 });
    // assert(t.remove(points[0]) && "element to remove not found in tree");
    // assert(t.remove(points[1]) && "element to remove not found in tree");
    // assert(t.remove(points[2]) && "element to remove not found in tree");
    // assert(t.remove(points[3]) && "element to remove not found in tree");
    // assert(t.remove(points[4]) && "element to remove not found in tree");
    // assert(t.remove(points[5]) && "element to remove not found in tree");
    // assert(t.remove({ 0, 2, 3 }) && "element to remove not found in tree");
    // t.nn_search({ 0, 2, 3 });
    // return 0;
    point3d query = { 9, 6, 1 };
    std::optional<point3d> found = t.nn_search(query);
    std::println("nearest to {} -> {} :: {}",
        query, *found, squared_distance<3>(*found, query));
    query = { 4, 4, 8 };
    found = t.nn_search(query);
    std::println("nearest to {} -> {} :: {}",
        query, *found, squared_distance<3>(*found, query));
    query = { 0, 0, 0 };
    found = t.nn_search(query);
    std::println("nearest to {} -> {} :: {}",
        query, *found, squared_distance<3>(*found, query));
    t.add({ 1, 1, 1 });
    t.print_tree();
    t.remove({ 1, 2, 3 });
    t.add({ 10, 5, 0 });
    t.add({ 10, 5, 1 });
    t.print_tree();
    t.remove({ 4, 1, 4 });
    t.print_tree();
    found = t.nn_search(query);
    std::println("nearest to {} -> {} :: {}",
        query, *found, squared_distance<3>(*found, query));
    std::flush(std::cout);
}
