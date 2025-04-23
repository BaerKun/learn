cmake_minimum_required(VERSION 3.5)
project(CPP)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

add_executable(main0 main0.cpp)

add_executable(main1 main1.cpp)
target_include_directories(main1 PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(main1 PRIVATE ${OpenCV_LIBS})

add_executable(main2 main2.cpp)

add_executable(main3 main3.cpp)
