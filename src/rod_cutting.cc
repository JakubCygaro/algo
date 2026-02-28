#include <limits>
#include <print>
#include <vector>
int main(void)
{
    std::vector p = { 0, 2, 3, 1, 10, 5, 6, 7, 10, 12, 11 };
    const auto n = 10;
    std::vector<int> r(n + 1);
    r[0] = 0;
    for (auto j = 1ul; j < r.size(); j++) {
        auto q = std::numeric_limits<int>::min();
        for (auto i = 1ul; i <= j; i++) {
            q = std::max(q, p[i] + p[j - i]);
        }
        r[j] = q;
    }
    std::println("{}", r);
    std::vector<int> sol {};
    for (auto j = (int)r.size() - 1; j > 0;) {
        auto rvn = r[j];
        for (auto i = 0; i <= j; i++) {
            if (p[i] + p[j - i] == rvn) {
                sol.push_back(j-i);
                j = i;
                break;
            }
        }
    }
    std::println("{}", sol);
}
