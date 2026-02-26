#include "datatypes.hpp"
#include <cstddef>
#include <print>

int main(void)
{
    dt::LinkedList<int> list;
    if (auto [head, ok] = list.front(); ok) {
        std::println("head {}", *head);
    }
    list.append_back(69);
    if (auto [head, ok] = list.front(); ok) {
        std::println("head {}", *head);
    }
    list.append_back(420);
    list.append_back(1337);
    list.append_back(80085);
    list.remove(list.find(420).first);
    for (size_t i = 0; i < list.size(); ++i) {
        auto [e, _] = list[i];
        std::println("{}", *e);
    }
}
