#base--
cmake_minimum_required(VERSION 2.8)
# SET(CMAKE_C_COMPILER "/usr/bin/gcc")
# SET(CMAKE_CXX_COMPILER "/usr/bin/g++")
project(Opencv_Test)

#Opencv--
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

#CameraControl--
add_subdirectory(CameraInclude)

#Tools
add_subdirectory(ToolsInclude)

#main-include
add_executable(imviewer main.cpp)
#
#target_link
target_link_libraries( imviewer ${OpenCV_LIBS})
target_link_libraries( imviewer CameraControl)
target_link_libraries( imviewer Tools)