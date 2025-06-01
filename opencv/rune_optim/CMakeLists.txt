cmake_minimum_required(VERSION 3.6)

project(rune_optim)

find_package(OpenCV)
include_directories(${OpenCV_DIRS})
link_libraries(${OpenCV_LIBS})

add_executable(rune_optim rune_optim.cpp)
