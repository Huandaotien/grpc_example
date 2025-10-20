cmake_minimum_required(VERSION 3.15)
project(grpc_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

message(STATUS "Found Protobuf ${Protobuf_VERSION}")
message(STATUS "Found gRPC ${gRPC_VERSION}")

# ----------------------------
# Generated .pb and .grpc.pb files
# ----------------------------
set(GRPC_PROTO_SRCS
    src/map_service.pb.cc
    src/map_service.grpc.pb.cc
)

set(GRPC_PROTO_HDRS
    src/map_service.pb.h
    src/map_service.grpc.pb.h
)

add_library(proto_lib STATIC ${GRPC_PROTO_SRCS})
target_include_directories(proto_lib PUBLIC src)
target_link_libraries(proto_lib
    PUBLIC
    gRPC::grpc++
    protobuf::libprotobuf
)

# ----------------------------
# Source code
# ----------------------------
set(SRC
    src/main.cpp
    src/SlamSystem.cpp
    src/GrpcServerManager.cpp
    src/MapServiceImpl.cpp
)

add_executable(slam_server ${SRC})
target_include_directories(slam_server PRIVATE src)
target_link_libraries(slam_server PRIVATE proto_lib)

# ----------------------------
# Optional client test
# ----------------------------
add_executable(slam_client client/client.cpp)
target_include_directories(slam_client PRIVATE src)
target_link_libraries(slam_client PRIVATE proto_lib)

install(TARGETS slam_server slam_client RUNTIME DESTINATION bin)
