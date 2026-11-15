#include <cstddef>
#include <span>

template <std::size_t Dim>
struct point;

template <>
struct point<3> {
    float x { }, y { }, z { };
};

template <std::size_t Dim>
class KDTree {
    static_assert(Dim >= 2, "Dim too small");
    static KDTree _build_tree(std::span<point<Dim>> points, std::size_t depth)
    {
        const auto axis = depth % Dim;
    }

public:
    static KDTree build_tree(std::span<point<Dim>> points)
    {
    }
};

int main(void)
{
}
