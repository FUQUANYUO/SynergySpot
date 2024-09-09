cmake_minimum_required(VERSION 3.20)
project(SynergySpot VERSION 0.01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 子模块 添加的子项目放到这个变量下面即可
set(SUB_PROS
        land-page
        arch-page
        message-container-page
        contact-page
)

# 设置输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/out)

# Qt module
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Network
)

file(GLOB_RECURSE   CoreCpp         ${CMAKE_SOURCE_DIR}/core/*.cpp      )
file(GLOB_RECURSE   ProtoCC         ${CMAKE_SOURCE_DIR}/protofile/*.cc  )
file(GLOB_RECURSE   UtilsCpp        ${CMAKE_SOURCE_DIR}/utils/*.cpp     )

# 即将优化掉
#file(GLOB_RECURSE PageCpp ${CMAKE_SOURCE_DIR}/page/*.cpp)
#file(GLOB_RECURSE PageUi ${CMAKE_SOURCE_DIR}/page/*.ui)

# core
include_directories(core)
# lib-common
include_directories(lib-common/include)
# page
#include_directories(page)
# protofile
include_directories(protofile)
# utils
include_directories(utils)

qt_add_executable(${PROJECT_NAME} main.cpp
        ${CoreCpp}
        ${PageCpp}
        ${ProtoCC}
        ${UtilsCpp}
        ${PageUi}
)

# 使用 yaml 静态库
target_compile_definitions(${PROJECT_NAME} PUBLIC
        YAML_CPP_STATIC_DEFINE
        PROTOBUF_USE_DLLS
        #LOC_TEST
)

# select win or linux
if(WIN32)
        target_link_directories(${PROJECT_NAME} PRIVATE
                lib-common/lib/Win
        )

        target_link_libraries(${PROJECT_NAME} PRIVATE
                Qt::Core
                Qt::Gui
                Qt::Widgets
                Qt::Network
                libprotobufd
                libprotobuf-lited
                libprotocd
                sqlite3
                libmysql
                yaml-cppd
                libcrypto
                libssl
        )

        # 移动 dll 到 out 文件夹
        file(GLOB DLLS "${CMAKE_CURRENT_SOURCE_DIR}/lib-common/lib/Win/lib*.dll")
        set(DLL_DESTINATION_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out/Debug")
        foreach(DLL_FILE ${DLLS})
        get_filename_component(DLL_NAME "${DLL_FILE}" NAME)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${DLL_FILE}"
                "${DLL_DESTINATION_DIR}/${DLL_NAME}"
                COMMENT "Copying ${DLL_NAME} to ${DLL_DESTINATION_DIR}"
        )
        endforeach()
else()
        target_link_directories(${PROJECT_NAME} PRIVATE
                lib-common/lib/Linux
        )

        target_link_libraries(${PROJECT_NAME} PRIVATE
                Qt::Core
                Qt::Gui
                Qt::Widgets
                Qt::Network
                protobuf
                sqlite3
                mysqlclient
                yaml-cpp
                crypto
                ssl
        )
endif()