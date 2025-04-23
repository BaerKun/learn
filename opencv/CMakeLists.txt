cmake_minimum_required(VERSION 3.27)

project(learnCV)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    set(OpenCV_DIR "C:\\_myLibs\\opencv-4.10\\build")
endif()

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_DIRS})
link_libraries(${OpenCV_LIBS})

add_executable(main main.cpp)

#add_library(somealg myalg.cpp)
#target_link_libraries(main somealg)