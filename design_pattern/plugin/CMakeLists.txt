cmake_minimum_required(VERSION 2.8)
project(plugin)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

message(${PROJECT_SOURCE_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
# lib output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/plugin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/plugin)

include_directories("./")

set(MAIN main.cpp plugin_manager.cpp renderer_factory.cpp)

add_executable(manager_main ${MAIN})
target_link_libraries(manager_main dl)

set(OPENGL plugin.cpp pluginapi.cpp)
add_library(opengl SHARED ${OPENGL})