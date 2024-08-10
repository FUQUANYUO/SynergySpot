cmake_minimum_required(VERSION 3.20)
project(Server)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/out)

find_package(Threads REQUIRED)

include_directories(thread-pool)
include_directories(tcp-socket)
include_directories(tcp-server)
include_directories(do-business)
include_directories(../src/protofile)
include_directories(../src/lib-common/include)
include_directories(../src/utils)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIR})

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

target_link_directories(Server PRIVATE
        ../src/lib-common/lib/Linux
)

target_link_libraries(Server PRIVATE
        Threads::Threads
        curl
        protobuf
        yaml-cpp
        mysqlclient
        ${PROTOBUF_LIBRARY}
        ${CURL_LIBRARIES}
)