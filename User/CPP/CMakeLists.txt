cmake_minimum_required(VERSION 3.0.0)
project(APSCPPExample VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(OpenCV_DIR "/opt/intel/openvino/opencv/cmake")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lz -lpthread -Wl,-rpath -Wl,/opt/intel/openvino/inference_engine/lib/armv7l")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
find_package(Lua 5.3 REQUIRED)
include_directories(${LUA_INCLUDE_DIR})

add_library(APSCPPExample SHARED APSCPPExample.cpp)
target_link_libraries(APSCPPExample ${OpenCV_LIBS})
target_link_libraries(APSCPPExample ${LUA_LIBRARIES})