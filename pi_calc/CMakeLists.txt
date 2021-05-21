cmake_minimum_required(VERSION 3.6)

project(pi)

set(CMAKE_CXX_STANDART 20)

add_executable(calculator pi.cpp)
target_link_libraries(calculator pthread)
