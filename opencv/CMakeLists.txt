cmake_minimum_required(VERSION 3.27)

project(learnCV)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    set(OpenCV_DIR "C:\\_myLibs\\opencv-4.10\\build")
endif()

find_package(OpenCV)
include_directories(${OpenCV_DIRS})
link_libraries(${OpenCV_LIBS})

add_executable(main main.cpp)

set(MVS_SDK_PATH /Library/MVS_SDK)
include_directories(${MVS_SDK_PATH}/Includes)
link_directories(${MVS_SDK_PATH}/lib)

add_executable(video video.cpp)
target_link_libraries(video MvCameraControl)

#add_library(somealg myalg.cpp)
#target_link_libraries(main somealg)