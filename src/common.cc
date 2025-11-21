#include "common.hpp"
namespace common {
namespace {
    std::random_device rd;
    std::mt19937 gen(rd());
}
int get_random_in_range(int low, int high)
{
    return std::uniform_int_distribution<int>(low, high)(gen);
}

}
