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

#APM_Client
add_subdirectory(APM_Client)

#main-include
add_executable(RuAPM_Cli main.cpp)
#
#target_link
target_link_libraries( RuAPM_Cli ${OpenCV_LIBS})
target_link_libraries( RuAPM_Cli CameraControl)
target_link_libraries( RuAPM_Cli Tools)
target_link_libraries( RuAPM_Cli APM_Client)