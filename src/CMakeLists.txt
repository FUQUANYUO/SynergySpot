cmake_minimum_required(VERSION 3.20)
project(SynergySpot VERSION 0.01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 子模块 添加的子项目放到这个变量下面即可
set(SUB_PROS
        effect-component
        arch-page
        land-page
)

# 设置输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/out)

# Qt module
find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Network
)

#file(GLOB_RECURSE   CoreCpp         ${CMAKE_SOURCE_DIR}/core/*.cpp      )
#file(GLOB_RECURSE   ProtoCC         ${CMAKE_SOURCE_DIR}/protofile/*.cc  )
#file(GLOB_RECURSE   UtilsCpp        ${CMAKE_SOURCE_DIR}/utils/*.cpp     )

# 遍历子项目
foreach(SUB_PATH ${SUB_PROS})
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/page-view/${SUB_PATH}/CMakeLists.txt)
                add_subdirectory(page-view/${SUB_PATH})
        endif ()
endforeach()

# core
include_directories(core)
# lib-common
include_directories(lib-common/include)
# page
include_directories(page-view)
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
                out
        )
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
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

                        ArchPagePluginsd.lib
                        LandPagePluginsd.lib
                )
        elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
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

                        elawidgettools.lib
                        ArchPagePlugins.lib
                        LandPagePlugins.lib
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