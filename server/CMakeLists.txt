cmake_minimum_required(VERSION 3.3)
project(Server)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/out)

include(FindProtobuf)
find_package(Threads REQUIRED)
find_package(protobuf REQUIRED)

include_directories(thread-pool)
include_directories(tcp-socket)
include_directories(tcp-server)
include_directories(do-business)
include_directories(../src/protofile)
include_directories(../src/lib-common/include)
include_directories(${Protobuf_INCLUDE_DIRS})

# protobuf
file(GLOB_RECURSE ProCC ../src/protofile/*.cc)
# do business
file(GLOB_RECURSE DoCpp do-business/*.cpp)
# db pool
file(GLOB_RECURSE DBCpp ../src/utils/db-pool/*.cpp)

link_directories(../src/lib-common/lib)

add_executable(Server
        Server.cpp
        tcp-server/TcpServer.cpp
        tcp-socket/TcpSocket.cpp
        thread-pool/ThreadPool.cpp
        thread-pool/TaskQueue.cpp
        ${DBCpp}
        ${DoCpp}
        ${ProCC}
)

target_link_libraries(Server PRIVATE
        Threads::Threads
        libyaml-cpp.so
        mysqlclient
        ${PROTOBUF_LIBRARY}
)