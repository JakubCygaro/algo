cmake_minimum_required(VERSION 3.90)
enable_testing()
project(algo)

add_executable(merge src/merge.cc)
add_test(NAME merge_sort COMMAND merge)
