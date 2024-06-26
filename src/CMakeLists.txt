cmake_minimum_required(VERSION 3.27)
project(SynergySpot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "D:/QT/6.5.3/msvc2019_64")

# 设置输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/out)

# Qt module
find_package(Qt6 6.5.3 EXACT COMPONENTS
        Core
        Gui
        Widgets
        Network
        REQUIRED)

file(GLOB_RECURSE CoreCpp ${CMAKE_SOURCE_DIR}/core/*.cpp)
file(GLOB_RECURSE PageCpp ${CMAKE_SOURCE_DIR}/page/*.cpp)
file(GLOB_RECURSE ProtoCC ${CMAKE_SOURCE_DIR}/protofile/*.cc)
file(GLOB_RECURSE UtilsCpp ${CMAKE_SOURCE_DIR}/utils/*.cpp)
file(GLOB_RECURSE PageUi ${CMAKE_SOURCE_DIR}/page/*.ui)

# core
include_directories(core)
# lib-common
include_directories(lib-common/include)
# page
include_directories(page)
# protofile
include_directories(protofile)
# utils
include_directories(utils)

add_executable(SynergySpot main.cpp
        ${CoreCpp}
        ${PageCpp}
        ${ProtoCC}
        ${UtilsCpp}
        ${PageUi}
)

# 使用 yaml 静态库
target_compile_definitions(SynergySpot PUBLIC
        YAML_CPP_STATIC_DEFINE
        PROTOBUF_USE_DLLS
        #LOC_TEST
)

target_link_directories(SynergySpot PRIVATE
        lib-common/lib
)

target_link_libraries(SynergySpot PRIVATE
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

# 移动 protobuf.dll 到 out 文件夹
file(GLOB PROTOBUF_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/lib-common/lib/libproto*.dll")
set(DLL_DESTINATION_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out/Debug")
foreach(DLL_FILE ${PROTOBUF_DLLS})
    get_filename_component(DLL_NAME "${DLL_FILE}" NAME)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${DLL_FILE}"
            "${DLL_DESTINATION_DIR}/${DLL_NAME}"
            COMMENT "Copying ${DLL_NAME} to ${DLL_DESTINATION_DIR}"
    )
endforeach()

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
