cmake_minimum_required(VERSION 3.20)
project(SynergySpot-GRPC-Client VERSION 0.01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../out)

# Qt module
find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Network
)

file(GLOB_RECURSE   ProtoCC         ${CMAKE_CURRENT_SOURCE_DIR}/../../protofile-grpc/*.cc     )
file(GLOB_RECURSE   RealtimeCpp     ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# grpc
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib-common/include/grpc-inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib-common/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../utils)
# protofile
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../protofile-grpc)

qt_add_executable(${PROJECT_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/get-time/GetCurTime.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/log/Log.cpp
        ${RealtimeCpp}
        ${ProtoCC}
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
            ${CMAKE_CURRENT_SOURCE_DIR}/../../lib-common/lib/Win/grpc
            ${CMAKE_CURRENT_SOURCE_DIR}/../../out
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(${PROJECT_NAME} PRIVATE
                Qt::Core
                absl_all
                grpc_all
                yaml-cppd
        )
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_link_libraries(${PROJECT_NAME} PRIVATE
                Qt::Core
                libprotobuf
                absl_all
                grpc_all
                yaml-cpp
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
                libprotobufd
                yaml-cppd
        )
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_link_libraries(${PROJECT_NAME} PRIVATE
                Qt::Core
                libprotobuf
                libmysql
                yaml-cpp
        )
    endif ()
endif()