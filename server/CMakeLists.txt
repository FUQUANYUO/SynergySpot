cmake_minimum_required(VERSION 3.20)
project(Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/out/${CMAKE_BUILD_TYPE})

add_subdirectory(realtime-business-process)

find_package(Threads REQUIRED)
find_package(ZLIB    REQUIRED)

include_directories(normal-business-process)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/protofile)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/lib-common/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/lib-common/include/grpc-inc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/utils)

# protobuf
file(GLOB_RECURSE ProCC ${CMAKE_CURRENT_SOURCE_DIR}/../src/protofile/*.cc)
# do business
file(GLOB_RECURSE NormalCpp ${CMAKE_CURRENT_SOURCE_DIR}/normal-business-process/*.cpp)
# db pool
file(GLOB_RECURSE DBCpp ${CMAKE_CURRENT_SOURCE_DIR}/../src/utils/db-pool/*.cpp)
# log
file(GLOB_RECURSE LOGCpp ${CMAKE_CURRENT_SOURCE_DIR}/../src/utils/log/*.cpp)
# get time
file(GLOB_RECURSE TimeCpp ${CMAKE_CURRENT_SOURCE_DIR}/../src/utils/get-time/*.cpp)
# encrypt
file(GLOB_RECURSE EncryptCpp ${CMAKE_CURRENT_SOURCE_DIR}/../src/utils/encrypt/*.cpp)

add_executable(Server
        ${NormalCpp}
        ${DBCpp}
        ${DoCpp}
        ${ProCC}
        ${LOGCpp}
        ${TimeCpp}
        ${EncryptCpp}
)

target_link_directories(Server PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/lib-common/lib/Linux
)

target_link_libraries(Server PRIVATE
        Threads::Threads
        ZLIB::ZLIB
        curl
        ssl
        crypto
        protobuf
        yaml-cpp
        mysqlclient
)