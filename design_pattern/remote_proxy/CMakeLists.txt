cmake_minimum_required(VERSION 2.8)
project(remote_proxy)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O0")

message(${PROJECT_SOURCE_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
# lib output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)

include_directories("./" "core" "core/impl")

set(SERVICE service_main.cc service.cc
    core/service.cc
    core/rpc_message.cc
    core/connection.cc
    core/runtime.cc
    core/utils.cc
    core/impl/socket_connection_impl.cc
    core/impl/socket_conn_factory_impl.cc)

set(PROXY proxy_main.cc proxy.cc
    core/proxy.cc
    core/rpc_message.cc
    core/connection.cc
    core/runtime.cc
    core/utils.cc
    core/impl/socket_connection_impl.cc
    core/impl/socket_conn_factory_impl.cc)

add_executable(service_main ${SERVICE})
target_link_libraries(service_main pthread)

add_executable(proxy_main ${PROXY})
target_link_libraries(proxy_main pthread)
