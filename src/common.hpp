#ifndef COMMON_HPP
#define COMMON_HPP

#include <cassert>
#include <random>

namespace common {
    inline static std::random_device rd;
    inline static std::mt19937 gen(rd());

    inline static int get_random_in_range(int low, int high) {
        // assert((low <= high) && "ya pierdole");
        return std::uniform_int_distribution<int> (low, high)(gen);
        // return (::rand()) % (high ) + low;
    }
}

#endif
