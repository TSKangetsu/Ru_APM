#base--
cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(OpenCV_DIR "/opt/intel/openvino/opencv/cmake")
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
project(Ru_APM)

#Opencv----4.1.0
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

#json------3.7.0
add_subdirectory(SRC/_thirdparty/json)

#CameraControl
add_subdirectory(SRC/CameraInclude)

#Tools
add_subdirectory(SRC/ToolsInclude)

#APM_Client
add_subdirectory(SRC/APM_Client)

#RaspberrySensors
add_subdirectory(SRC/RaspberrySensors)

#Config-Setter
add_subdirectory(SRC/ConfigSetter)

#main-include
add_executable(RuAPM_Cli SRC/TestModule.cpp)
#
#target_link
target_link_libraries( RuAPM_Cli Tools)
target_link_libraries( RuAPM_Cli CameraControl)
target_link_libraries( RuAPM_Cli APM_Client)
target_link_libraries( RuAPM_Cli RaspberrySensors)
target_link_libraries( RuAPM_Cli ConfigSetter)
#target_link_thirdparty
target_link_libraries( RuAPM_Cli ${OpenCV_LIBS})
target_link_libraries( RuAPM_Cli nlohmann_json::nlohmann_json)