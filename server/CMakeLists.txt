cmake_minimum_required(VERSION 3.3)
project(Server)

set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/out)

include(FindProtobuf)
find_package(Threads REQUIRED)
find_package(protobuf REQUIRED)

include_directories(ThreadPool)
include_directories(TcpSocket)
include_directories(TcpServer)
include_directories(../src/protofile)
include_directories(../src/lib-common/include)
include_directories(${Protobuf_INCLUDE_DIRS})

# protobuf
file(GLOB_RECURSE ProCC ../src/protofile/*.cc)

link_directories(../src/lib-common/lib)

add_executable(Server
        Server.cpp
        TcpServer/TcpServer.cpp
        TcpSocket/TcpSocket.cpp
        ThreadPool/ThreadPool.cpp
        ThreadPool/TaskQueue.cpp
        ${ProCC}
)

target_link_libraries(Server PRIVATE
        Threads::Threads
        libyaml-cpp.so
        ${PROTOBUF_LIBRARY}
)