 #base--
cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(OpenCV_DIR "/opt/intel/openvino/opencv/cmake")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lz -lpthread -Wl,-rpath -Wl,/opt/intel/openvino/inference_engine/lib/armv7l")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a")
add_definitions(-DModuleTest)
# add_definitions(-DDEBUG)
add_definitions(-DRPiDEBUG)
project(Ru_APM)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
find_package(Lua 5.3 REQUIRED)
include_directories(${LUA_INCLUDE_DIR})
add_subdirectory(SRC/_VisionBase)
add_subdirectory(SRC/RPiSingleAPM/src)
add_subdirectory(SRC/MessageController)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/User/CPP/)

add_executable(RuAPM_Cli SRC/TestModule.cpp)
target_link_libraries( RuAPM_Cli VisionBase)
target_link_libraries( RuAPM_Cli MessageController)
target_link_libraries( RuAPM_Cli RPiSingleAPI)
target_link_libraries( RuAPM_Cli ${OpenCV_LIBS})
target_link_libraries( RuAPM_Cli ${LUA_LIBRARIES})
target_link_libraries(RuAPM_Cli ${CMAKE_CURRENT_SOURCE_DIR}/User/CPP/build/libAPSCPPExample.so)