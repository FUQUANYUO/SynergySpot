cmake_minimum_required(VERSION 3.20)
project(SynergySpot-GRPC-Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../out/${CMAKE_BUILD_TYPE})

set(CMAKE_PREFIX_PATH "/root/grpc_install")

# need you install grpc
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_package(absl REQUIRED)

message(STATUS "Using protobuf ${Protobuf_VERSION}")
message(STATUS "Using gRPC ${gRPC_VERSION}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/lib-common/include/grpc-inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/lib-common/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/protofile-grpc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/utils)

# realtime cpp
file(GLOB_RECURSE RealtimeCpp ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE GRPC_CC ${CMAKE_CURRENT_SOURCE_DIR}/../../src/protofile-grpc/*.cc)

add_executable(SynergySpot-GRPC-Server
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src/utils/log/Log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src/utils/get-time/GetCurTime.cpp
        ${GRPC_CC}
        ${RealtimeCpp}
)

target_link_directories(SynergySpot-GRPC-Server PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src/lib-common/lib/Linux
)

target_link_libraries(SynergySpot-GRPC-Server PRIVATE
        Threads::Threads
        yaml-cpp
        ssl
        protobuf
        gRPC::gpr
        gRPC::grpc++
)