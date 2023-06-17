project(proto_test)

cmake_minimum_required(VERSION 3.23)


set(CMAKE_CXX_STANDARD 11)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS person.proto)
# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS EXPORT_MACRO DLL_EXPORT person.proto)
# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS DESCRIPTORS PROTO_DESCS person.proto)
file(GLOB NART_PROTOS *.proto)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${NART_PROTOS})
message(${PROTO_SRCS})
protobuf_generate_python(PROTO_PY person.proto)
add_executable(test main.cc ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(test ${Protobuf_LIBRARIES})
