cmake_minimum_required(VERSION 3.90)
enable_testing()
project(algo)

add_executable(merge src/merge.cc)
add_test(NAME merge_sort COMMAND merge)

add_executable(quick src/quick.cc)
add_test(NAME quick_sort COMMAND quick)
