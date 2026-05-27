cmake_minimum_required(VERSION 3.90)
enable_testing()
project(algo)

include_directories(BEFORE src)
add_library(cpp_std_23 INTERFACE)

target_compile_features(cpp_std_23 INTERFACE cxx_std_23)
target_compile_options(cpp_std_23
    INTERFACE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
)
target_compile_definitions(cpp_std_23
    INTERFACE $<$<CONFIG:Testing>:-DTEST_INTERNALS>
)
add_library(common STATIC
    ${CMAKE_SOURCE_DIR}/src/common.cc
)
target_link_libraries(common
    PUBLIC cpp_std_23
)

add_executable(merge src/merge.cc)
target_link_libraries(merge PRIVATE common)
add_test(NAME merge_sort COMMAND merge)

add_executable(quick src/quick.cc)
target_link_libraries(quick PRIVATE common)
add_test(NAME quick_sort COMMAND quick)

add_executable(rselect src/rselect.cc)
target_link_libraries(rselect PRIVATE common)
add_test(NAME rselect_selection COMMAND rselect)

add_executable(dselect src/dselect.cc)
target_link_libraries(dselect PRIVATE common)
add_test(NAME dselect_selection COMMAND dselect)

add_executable(graph_connection_dfs_bfs src/graph_connection.cc)
target_link_libraries(graph_connection_dfs_bfs PRIVATE common)
add_test(NAME graph_connection_dfs_bfs COMMAND graph_connection_dfs_bfs)

add_executable(dfs_topo src/dfs_topo.cc)
target_link_libraries(dfs_topo PRIVATE common)
add_test(NAME dfs_topo COMMAND dfs_topo)

add_executable(graph_scc src/graph_scc.cc)
target_link_libraries(graph_scc PRIVATE common)
add_test(NAME graph_scc COMMAND graph_scc)

add_executable(dijkstra src/dijkstra.cc)
target_link_libraries(dijkstra PRIVATE common)
add_test(NAME dijkstra COMMAND dijkstra)

add_executable(heap src/heap.cc)
target_link_libraries(heap PRIVATE common)
add_test(NAME heap COMMAND heap)

add_executable(inplace_heap_sort src/inplace_heap_sort.cc)
target_link_libraries(inplace_heap_sort PRIVATE common)
add_test(NAME inplace_heap_sort COMMAND inplace_heap_sort)

add_executable(tree src/tree.cc)
target_link_libraries(tree PRIVATE common)
add_test(NAME tree COMMAND tree)

add_executable(hashmap src/hashmap.cc)
target_link_libraries(hashmap PRIVATE common)
add_test(NAME hashmap COMMAND hashmap)

add_executable(bloom_filter src/bloom_filter.cc)
target_link_libraries(bloom_filter PRIVATE common)
add_test(NAME bloom_filter COMMAND bloom_filter)

add_executable(huffman src/huffman.cc)
target_link_libraries(huffman PRIVATE common)
add_test(NAME huffman COMMAND huffman)

add_executable(prim src/prim.cc)
target_link_libraries(prim PRIVATE common)
add_test(NAME prim COMMAND prim)

add_executable(kruskal src/kruskal.cc)
target_link_libraries(kruskal PRIVATE common)
add_test(NAME kruskal COMMAND kruskal)

add_executable(wis src/wis.cc)
target_link_libraries(wis PRIVATE common)
add_test(NAME wis COMMAND wis)

add_executable(knapsack src/knapsack.cc)
target_link_libraries(knapsack PRIVATE common)
add_test(NAME knapsack COMMAND knapsack)

add_executable(sequence_alignment src/sequence_alignment.cc)
target_link_libraries(sequence_alignment PRIVATE common)
add_test(NAME sequence_alignment COMMAND sequence_alignment)

add_executable(optimal_search_tree src/optimal_search_tree.cc)
target_link_libraries(optimal_search_tree PRIVATE common)
add_test(NAME optimal_search_tree COMMAND optimal_search_tree)

add_executable(bellman_ford src/bellman_ford.cc)
target_link_libraries(bellman_ford PRIVATE common)
add_test(NAME bellman_ford COMMAND bellman_ford)

add_executable(floyd_warshall src/floyd_warshall.cc)
target_link_libraries(floyd_warshall PRIVATE common)
add_test(NAME floyd_warshall COMMAND floyd_warshall)

add_executable(insertion src/insertion.cc)
target_link_libraries(insertion PRIVATE common)
add_test(NAME insertion COMMAND insertion)

add_executable(maximum_subarray src/maximum_subarray.cc)
target_link_libraries(maximum_subarray PRIVATE common)
add_test(NAME maximum_subarray COMMAND maximum_subarray)

add_executable(maximum_subarray_linear src/maximum_subarray_linear.cc)
target_link_libraries(maximum_subarray_linear PRIVATE common)
add_test(NAME maximum_subarray_linear COMMAND maximum_subarray_linear)

# add_executable(matrix src/matrix.cc)
# target_link_libraries(matrix PRIVATE common)
# add_test(NAME matrix COMMAND matrix)

add_executable(counting_sort src/counting_sort.cc)
target_link_libraries(counting_sort PRIVATE common)
add_test(NAME counting_sort COMMAND counting_sort)

add_executable(linked_list src/linked_list.cc)
target_link_libraries(linked_list PRIVATE common)
add_test(NAME linked_list COMMAND linked_list)

add_executable(rod_cutting src/rod_cutting.cc)
target_link_libraries(rod_cutting PRIVATE common)
add_test(NAME rod_cutting COMMAND rod_cutting)

add_executable(b_tree src/b_tree.cc)
target_link_libraries(b_tree PRIVATE common)
add_test(NAME b_tree COMMAND b_tree)

add_executable(avl_tree src/avl_tree.cc)
target_link_libraries(avl_tree PRIVATE common)
add_test(NAME avl_tree COMMAND avl_tree)

add_executable(splay_tree src/splay_tree.cc)
target_link_libraries(splay_tree PRIVATE common)
add_test(NAME splay_tree COMMAND splay_tree)
