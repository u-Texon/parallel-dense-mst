
cmake_minimum_required(VERSION 3.10)
project(ParallelMST)

add_executable(MST main.cpp)

#MPI
find_package(MPI REQUIRED)
target_link_libraries(MST PUBLIC MPI::MPI_CXX)


#find_package(TBB REQUIRED)
#find_package(OpenMP REQUIRED)
#find_package(Threads REQUIRED)








add_library(timer ${CMAKE_CURRENT_SOURCE_DIR}/include/util/timer.cpp)
target_link_libraries(timer PRIVATE base_target)

