cmake_minimum_required(VERSION 3.3)
project(Server)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/out)

find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(CURL REQUIRED)

include_directories(thread-pool)
include_directories(tcp-socket)
include_directories(tcp-server)
include_directories(do-business)
include_directories(../src/protofile)
include_directories(../src/lib-common/include)
include_directories(../src/utils)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIR})

# 指定 MySQL include
include_directories(/usr/include/mysql)

link_directories(/usr/lib64/mysql)
link_directories(/usr/lib64/yaml)

# protobuf
file(GLOB_RECURSE ProCC ../src/protofile/*.cc)
# do business
file(GLOB_RECURSE DoCpp do-business/*.cpp)
# db pool
file(GLOB_RECURSE DBCpp ../src/utils/db-pool/*.cpp)

add_executable(Server
        Server.cpp
        tcp-server/TcpServer.cpp
        tcp-server/EpollEngine.cpp
        tcp-socket/TcpSocket.cpp
        thread-pool/ThreadPool.cpp
        thread-pool/TaskQueue.cpp
        ${DBCpp}
        ${DoCpp}
        ${ProCC}
)

target_link_libraries(Server PRIVATE
        Threads::Threads
        yaml-cpp.so
        mysqlclient
        ${PROTOBUF_LIBRARY}
        ${CURL_LIBRARIES}
)