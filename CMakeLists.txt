
cmake_minimum_required(VERSION 3.10)
project(ParallelMST)


find_package(TBB REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Threads REQUIRED)
find_package(MPI REQUIRED)

#add_subdirectory(external/graphs EXCLUDE_FROM_ALL)

add_library(base_target INTERFACE)
target_link_libraries(base_target INTERFACE MPI::MPI_CXX TBB::tbb OpenMP::OpenMP_CXX)



add_library(timer ${CMAKE_CURRENT_SOURCE_DIR}/include/util/timer.cpp)
target_link_libraries(timer PRIVATE base_target)

add_library(generate ${CMAKE_CURRENT_SOURCE_DIR}/generateGraphs/generateGraph.cpp)
target_link_libraries(generate PRIVATE base_target)

add_executable(MST main.cpp)
target_link_libraries(MST PUBLIC timer base_target generate)




