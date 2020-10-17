 #base--
cmake_minimum_required(VERSION 3.13)
set(OpenCV_DIR "/opt/intel/openvino/opencv/cmake")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,/opt/intel/openvino/inference_engine/lib/armv7l")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -march=armv7-a")
add_definitions(-Dlinux)
add_definitions(-DDEBUG)
project(Ru_APM)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_subdirectory(SRC/_VisionBase)

add_executable(RuAPM_Cli SRC/TestModule.cpp)
target_link_libraries( RuAPM_Cli VisionBase)
target_link_libraries( RuAPM_Cli ${OpenCV_LIBS})