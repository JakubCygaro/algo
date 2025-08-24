#include "datatypes.hpp"
#include <iostream>
#include <print>
int main(void){
    dt::RBTree<int, int> tree;
    tree.insert(1, 100);
    tree.insert(2, 101);
    tree.insert(3, 102);
    std::cout << *tree.search(3) << std::endl;
    std::cout << *std::get<1>(tree.min()) << std::endl;
    std::cout << *std::get<1>(tree.max()) << std::endl;

    auto sorted = tree.output_sorted();

    std::println("{}", sorted);

}
