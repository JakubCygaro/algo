cmake_minimum_required(VERSION 3.90)
enable_testing()
project(algo)

include_directories(BEFORE src)
add_library(cpp_std_23 INTERFACE)

target_compile_features(cpp_std_23 INTERFACE cxx_std_23)

add_executable(merge src/merge.cc)
add_test(NAME merge_sort COMMAND merge)

add_executable(quick src/quick.cc)
add_test(NAME quick_sort COMMAND quick)

add_executable(rselect src/rselect.cc)
target_link_libraries(rselect PRIVATE cpp_std_23)
add_test(NAME rselect_selection COMMAND rselect)

add_executable(dselect src/dselect.cc)
target_link_libraries(dselect PRIVATE cpp_std_23)
add_test(NAME dselect_selection COMMAND dselect)

add_executable(graph_dfs src/graph_dfs.cc)
target_link_libraries(graph_dfs PRIVATE cpp_std_23)
add_test(NAME graph_dfs COMMAND graph_dfs)
