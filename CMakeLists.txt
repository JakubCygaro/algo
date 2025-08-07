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

add_executable(graph_connection_dfs_bfs src/graph_connection.cc)
target_link_libraries(graph_connection_dfs_bfs PRIVATE cpp_std_23)
add_test(NAME graph_connection_dfs_bfs COMMAND graph_connection_dfs_bfs)

add_executable(dfs_topo src/dfs_topo.cc)
target_link_libraries(dfs_topo PRIVATE cpp_std_23)
add_test(NAME dfs_topo COMMAND dfs_topo)

add_executable(graph_scc src/graph_scc.cc)
target_link_libraries(graph_scc PRIVATE cpp_std_23)
add_test(NAME graph_scc COMMAND graph_scc)

add_executable(dijkstra src/dijkstra.cc)
target_link_libraries(dijkstra PRIVATE cpp_std_23)
add_test(NAME dijkstra COMMAND dijkstra)
