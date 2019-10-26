 #base--
cmake_minimum_required(VERSION 3.15)
IF(UNIX)
	set(CMAKE_C_COMPILER "/usr/bin/gcc")
	set(CMAKE_CXX_COMPILER "/usr/bin/g++")
	set(OpenCV_DIR "/opt/intel/openvino/opencv/cmake")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,/opt/intel/openvino/inference_engine/lib/armv7l")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -march=armv7-a")
	add_definitions(-Dlinux)
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
#AirControl
	add_subdirectory(SRC/AirControl/RECConnect)
#Sensors
	add_subdirectory(SRC/AirControl/SensorsConnet)
#APMModule
	add_subdirectory(SRC/APMModule)
#APM_Client
	add_subdirectory(SRC/APM_Client)
ENDIF()

#json------3.7.0
add_subdirectory(SRC/_thirdparty/json)

#CameraControl
add_subdirectory(SRC/CameraInclude)

#Tools
add_subdirectory(SRC/ToolsInclude)

#Config-Setter
add_subdirectory(SRC/ConfigSetter)

#main-include
add_executable(RuAPM_Cli SRC/TestModule.cpp)

#target_link
target_link_libraries( RuAPM_Cli Tools)
target_link_libraries( RuAPM_Cli CameraControl)
target_link_libraries( RuAPM_Cli ConfigSetter)
#target_link_thirdparty
target_link_libraries( RuAPM_Cli ${OpenCV_LIBS})
target_link_libraries( RuAPM_Cli nlohmann_json::nlohmann_json)

IF(UNIX)
	target_link_libraries( RuAPM_Cli APM_Client)
	target_link_libraries( RuAPM_Cli APMModule)
	target_link_libraries( RuAPM_Cli AirControl)
	target_link_libraries( RuAPM_Cli Sensors)
	target_link_libraries( RuAPM_Cli ${wiringPi_LIB})
ENDIF()