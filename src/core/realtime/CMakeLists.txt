cmake_minimum_required(VERSION 3.20)
project(SynergySpot-GRPC-Client VERSION 0.01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../out)

# grpc cmake
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../lib-common/lib/Win/grpc/lib")

set(gRPC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../lib-common/lib/Win/grpc/lib/cmake/grpc")
set(absl_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../lib-common/lib/Win/grpc/lib/cmake/absl")
set(Protobuf_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../lib-common/lib/Win/grpc/cmake")


# 查找依赖
find_package(Protobuf CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)


# Qt module
find_package(Qt6 REQUIRED COMPONENTS
        Core
        Network
        Widgets
        OpenGLWidgets
)

file(GLOB_RECURSE   ProtoCC         ${CMAKE_CURRENT_SOURCE_DIR}/../../protofile-grpc/*.cc     )
file(GLOB_RECURSE   RealtimeCpp     ${CMAKE_CURRENT_SOURCE_DIR}/business/*.cpp)
file(GLOB_RECURSE   TRTCCpp         ${CMAKE_CURRENT_SOURCE_DIR}/trtc-realtime-comm/*.cpp)

set(UtilsCpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/get-time/GetCurTime.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/log/Log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/uuid/GenUUID.cpp
)

QT_ADD_RESOURCES(
        QRC_FILES                    Realtime.qrc
)

set(RealtimeCpp ${RealtimeCpp} main.cpp RealtimeCommHandler.cpp)

# common data
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common-data)

# grpc
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib-common/include/grpc-inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib-common/include/trtc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib-common/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../utils)
# protofile
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../protofile-grpc)

qt_add_executable(${PROJECT_NAME}
        ${RealtimeCpp}
        ${ProtoCC}
        ${UtilsCpp}
        ${TRTCCpp}
        ${QRC_FILES}
)

# 使用 yaml 静态库
target_compile_definitions(${PROJECT_NAME} PUBLIC
        YAML_CPP_STATIC_DEFINE
        PROTOBUF_USE_DLLS
        #LOC_TEST
)

# select win or linux
if(SS_CLIENT_WIN)
    target_link_directories(${PROJECT_NAME} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../../lib-common/lib/Win
            ${CMAKE_CURRENT_SOURCE_DIR}/../../out
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(${PROJECT_NAME} PRIVATE
                Qt::Core
                Qt::Network
                Qt::Widgets
                Qt::OpenGLWidgets
                yaml-cppd
                gRPC::grpc++
                gRPC::grpc
                libprotobuf
                liteav.lib

                ElaWidgetToolsd
                CommonDataPlugind
                PluginManagerd
        )
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        target_link_libraries(${PROJECT_NAME} PRIVATE
                Qt::Core
                Qt::Network
                Qt::Widgets
                Qt::OpenGLWidgets
                yaml-cpp
                gRPC::grpc++
                gRPC::grpc
                libprotobuf
                liteav.lib

                ElaWidgetTools
                CommonDataPlugin
                PluginManager
        )
    endif ()
else()
    target_link_directories(${PROJECT_NAME} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../../lib-common/lib/Linux
            ${CMAKE_CURRENT_SOURCE_DIR}/../../out
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(${PROJECT_NAME} PRIVATE
                Qt::Core
                yaml-cppd
        )
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_link_libraries(${PROJECT_NAME} PRIVATE
                Qt::Core
                libmysql
                yaml-cpp
        )
    endif ()
endif()