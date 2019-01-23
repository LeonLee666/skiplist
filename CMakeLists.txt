cmake_minimum_required(VERSION 3.13)
project(skiplist)

set(CMAKE_CXX_STANDARD 14)

add_executable(skiplist1 skiplist1.cpp)

find_package(Threads)
add_executable(skiplist2 skiplist2.cpp)
target_link_libraries(skiplist2 ${CMAKE_THREAD_LIBS_INIT})

add_executable(skiplist3 skiplist3.cpp)

add_executable(skiplist4 skiplist4.cpp)
target_link_libraries(skiplist4 ${CMAKE_THREAD_LIBS_INIT})