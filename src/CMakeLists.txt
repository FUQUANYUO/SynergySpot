cmake_minimum_required(VERSION 3.20)
project(SynergySpot VERSION 0.01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/out)

# 前端Ui子模块 添加的子项目放到这个变量下面即可
set(SUB_PROS
        plugin-manager
        effect-component
        arch-page
        land-page
        message-page
        contact-page
        file-manager-page
        settings-page
        about-page
        user-page
)

# Qt module
find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Network

        OpenGLWidgets
)

#file(GLOB_RECURSE   CoreCpp         ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp      )
file(GLOB_RECURSE   ProtoCC         ${CMAKE_CURRENT_SOURCE_DIR}/protofile/*.cc     )
file(GLOB_RECURSE   UtilsCpp        ${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp        )

set(CoreCpp
        ${CMAKE_CURRENT_SOURCE_DIR}/core/base/client-request-handler/ClientRequestHandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/core/safety/email-check/EmailVerify.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/core/safety/encrypt-passwd/EncryptPasswd.cpp
)

# 遍历前端Ui子项目
foreach(SUB_PATH ${SUB_PROS})
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/page-view/${SUB_PATH}/CMakeLists.txt)
                add_subdirectory(page-view/${SUB_PATH})
        endif ()
endforeach()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
        # 添加实时子项目
        add_subdirectory(core/realtime)
endif ()
# 公共数据插件
add_subdirectory(core/common-data)

# core
include_directories(core)
# lib-common
include_directories(lib-common/include)
include_directories(lib-common/include/grpc-inc)
# page
include_directories(page-view)
# protofile
include_directories(protofile)
# utils
include_directories(utils)

qt_add_executable(${PROJECT_NAME} main.cpp MainLogic.cpp lib-common/include/ela-widget-tools/ElaWidgetTools.qrc
        ${CoreCpp}
        ${ProtoCC}
        ${UtilsCpp}
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
        PROTOBUF_USE_DLLS
        SQLITE_HAS_CODEC
        SS_CLIENT
        #LOC_TEST
)

# select win or linux
if(SS_CLIENT_WIN)
        target_link_directories(${PROJECT_NAME} PRIVATE
                lib-common/lib/Win
                lib-common/lib/Win/grpc
                out
        )
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
                target_link_libraries(${PROJECT_NAME} PRIVATE
                        Qt::Core
                        Qt::Gui
                        Qt::Widgets
                        Qt::Network
                        Qt::OpenGLWidgets
                        libprotobufd
                        yaml-cppd
                        libmysql
                        libcrypto
                        libsqlite3
                        libssl
                        sqlcipher
                        CommonDataPlugind

                        ElaWidgetToolsd
                        PluginManagerd
                        ArchPagePlugind
                        ContactPagePlugind
                        MessagePagePlugind
                        LandPagePlugind
                        UserPagePlugind
                )
        elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
                target_link_libraries(${PROJECT_NAME} PRIVATE
                        Qt::Core
                        Qt::Gui
                        Qt::Widgets
                        Qt::Network
                        Qt::OpenGLWidgets
                        libprotobuf
                        yaml-cpp
                        libmysql

                        CommonDataPlugin

                        PluginManager
                        ArchPagePlugin
                        LandPagePlugin
                )
        endif ()

        # 移动 dll 到 out 文件夹
        file(GLOB DLLS "${CMAKE_CURRENT_SOURCE_DIR}/lib-common/lib/Win/*.dll")
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
                set(DLL_DESTINATION_DIR "${CMAKE_SOURCE_DIR}/src/out/Debug")
        elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
                set(DLL_DESTINATION_DIR "${CMAKE_SOURCE_DIR}/src/out/Release")
        endif()
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
                out
        )

        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
                target_link_libraries(${PROJECT_NAME} PRIVATE
                        Qt::Core
                        Qt::Gui
                        Qt::Widgets
                        Qt::Network
                        Qt::OpenGLWidgets
                        libprotobuf
                        sqlite3
                        libmysql
                        yaml-cppd
                        libcrypto
                        libssl

                        PluginManagerd
                        ArchPagePlugind
                        LandPagePlugind
                )
        elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
                target_link_libraries(${PROJECT_NAME} PRIVATE
                        Qt::Core
                        Qt::Gui
                        Qt::Widgets
                        Qt::Network
                        Qt::OpenGLWidgets
                        libprotobuf
                        sqlite3
                        libmysql
                        yaml-cpp
                        libcrypto
                        libssl

                        PluginManager
                        ArchPagePlugin
                        LandPagePlugin
                )
        endif ()

        # 移动 dll 到 out 文件夹
        file(GLOB DLLS "${CMAKE_CURRENT_SOURCE_DIR}/lib-common/lib/Linux/*.so")
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
                set(DLL_DESTINATION_DIR "${CMAKE_SOURCE_DIR}/src/out/Debug")
        elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
                set(DLL_DESTINATION_DIR "${CMAKE_SOURCE_DIR}/src/out/Release")
        endif()
        foreach(DLL_FILE ${DLLS})
                get_filename_component(DLL_NAME "${DLL_FILE}" NAME)
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy
                        "${DLL_FILE}"
                        "${DLL_DESTINATION_DIR}/${DLL_NAME}"
                        COMMENT "Copying ${DLL_NAME} to ${DLL_DESTINATION_DIR}"
                )
        endforeach()
endif()