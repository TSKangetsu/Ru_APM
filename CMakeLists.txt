#base--
cmake_minimum_required(VERSION 3.10)
IF(UNIX)
	MESSAGE(STATUS "current platform: Linux ")
	set(CMAKE_C_COMPILER "/usr/bin/gcc")
	set(CMAKE_CXX_COMPILER "/usr/bin/g++")
	set(OpenCV_DIR "/opt/intel/openvino/opencv/cmake")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,/opt/intel/openvino/inference_engine/lib/armv7l")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
	add_definitions(-Dlinux)
ELSEIF(WIN32)
	#set(CMAKE_C_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.22.27905/bin/Hostx64/x64/cl.exe")
	#set(CMAKE_CXX_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.22.27905/bin/Hostx64/x64/cl.exe")
	add_definitions(-Dwindows)
ENDIF()

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