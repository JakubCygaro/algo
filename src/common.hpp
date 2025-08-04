#ifndef COMMON_HPP
#define COMMON_HPP

#include <cassert>
#include <random>

namespace common {
    inline static std::random_device rd;
    inline static std::mt19937 gen(rd());

    inline static int get_random_in_range(int low, int high) {
        // return std::uniform_int_distribution<int> (low, high)(gen);
        return (::rand() + low) % high + 1;
    }
}

#endif
