set(MODULE_CV false  CACHE BOOL "enable module: computer vision toolkit")
set(MODULE_OPENSSL false  CACHE BOOL "enable module: openssl")
set(MODULE_LUA false  CACHE BOOL "enable module: lua plugin")

cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)

project(Ru_APM)

find_package(Threads REQUIRED)
add_subdirectory(SRC/RPiSingleAPM/src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/SRC/_Excutable/CameraDrive/ v4l2)

add_executable(RuAPM_Cli SRC/Launch.cpp ${v4l2})
target_link_libraries(RuAPM_Cli PRIVATE RPiSingleAPI)
target_link_libraries(RuAPM_Cli PRIVATE Threads::Threads)