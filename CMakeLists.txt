 #base--
cmake_minimum_required(VERSION 3.13)
IF(UNIX)
	set(CMAKE_C_COMPILER "/usr/bin/gcc")
	set(CMAKE_CXX_COMPILER "/usr/bin/g++")
	set(OpenCV_DIR "/opt/intel/openvino/opencv/cmake")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,/opt/intel/openvino/inference_engine/lib/armv7l")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -march=armv7-a")
	add_definitions(-Dlinux)
	add_definitions(-DUSINGJSON)
	add_definitions(-DmillSocket)
ELSEIF(WIN32)
	add_definitions(-Dwindows)
ENDIF()

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
project(Ru_APM)

#Opencv----4.1.0
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
 

IF(UNIX)
#wiringPi
	find_library(wiringPi_LIB wiringPi)
ENDIF()

#json------3.7.0
add_subdirectory(SRC/_thirdparty/json)

#libmillNet
find_library(mill mill)

#CameraControl
add_subdirectory(SRC/CameraInclude)

#Tools
add_subdirectory(SRC/ToolsInclude)

#Config-Setter
add_subdirectory(SRC/ConfigSetter)

#APMController
add_subdirectory(SRC/APMController)

#SingleAPMAPI
add_subdirectory(SRC/RPiSingleAPM/src)
add_subdirectory(SRC/RPiSingleAPM/_thirdparty)

#main-include
add_executable(RuAPM_Cli SRC/TestModule.cpp)

#target_link
target_link_libraries( RuAPM_Cli Tools)
target_link_libraries( RuAPM_Cli CameraControl)
target_link_libraries( RuAPM_Cli ConfigSetter)
target_link_libraries( RuAPM_Cli APMController)
target_link_libraries( RuAPM_Cli RPiSingleAPI)
target_link_libraries( RuAPM_Cli LIB)
#target_link_thirdparty
target_link_libraries( RuAPM_Cli ${OpenCV_LIBS})
target_link_libraries( RuAPM_Cli Json)
target_link_libraries( RuAPM_Cli ${mill})

IF(UNIX)
	target_link_libraries( RuAPM_Cli ${wiringPi_LIB})
ENDIF()