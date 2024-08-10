cmake_minimum_required(VERSION 3.20)
project(SynergySpot VERSION 0.01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 子模块 添加的子项目放到这个变量下面即可
set(SUB_PROS
        land-page
)

# 设置输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/out)

# Qt module
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Network
        Quick
        Qml
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Network
        Quick
        Qml
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

set(QRC_FILES_LIST "")
set(APP_QRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/page-qml/App.qrc)

qt_add_big_resources(QRC_RESOURCES ${APP_QRC_PATH})

set_property(SOURCE ${APP_QRC_PATH} PROPERTY SKIP_AUTORCC ON)

# 清空App.qrc文件，准备覆写
file(WRITE ${APP_QRC_PATH} "<RCC>\n       <qresource prefix=\"/\">\n           <file>App.qml</file>\n")

# 遍历子项目
foreach(SUB_PATH ${SUB_PROS})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/page-qml/${SUB_PATH}/CMakeLists.txt)
        add_subdirectory(page-qml/${SUB_PATH})

        # 遍历子项目中的QML文件
        file(GLOB_RECURSE QML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/page-qml/${SUB_PATH}/*.qml)
        file(GLOB_RECURSE RC_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/page-qml/${SUB_PATH}/rc-page/*.*)

        foreach(RC_FILE ${RC_FILES})
            file(REAL_PATH ${RC_FILE} REAL_RC_FILE)
            file(RELATIVE_PATH RC_RELATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/page-qml/${SUB_PATH} ${REAL_RC_FILE})

            # 将QML文件路径添加到App.qrc文件中
            file(APPEND ${APP_QRC_PATH} "           <file>${SUB_PATH}/${RC_RELATIVE_PATH}</file>\n")
        endforeach()

        # 注意资源文件与QML文件的资源顺序
        file(APPEND ${APP_QRC_PATH} "\n")
        foreach(QML_FILE ${QML_FILES})
            file(REAL_PATH ${QML_FILE} REAL_QML_FILE)
            file(RELATIVE_PATH QML_RELATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/page-qml/${SUB_PATH} ${REAL_QML_FILE})

            # 将QML文件路径添加到App.qrc文件中
            file(APPEND ${APP_QRC_PATH} "           <file>${SUB_PATH}/${QML_RELATIVE_PATH}</file>\n")
        endforeach()

        # 结束子项目的<file>标签
        file(APPEND ${APP_QRC_PATH} "       </qresource>\n")
    else()
        message(WARNING "Subproject path page-qml/${SUB_PATH} does not exist!")
    endif()
endforeach()

# 结束<qresource>和<RCC>标签
file(APPEND ${APP_QRC_PATH} "</RCC>\n")

qt_add_executable(${PROJECT_NAME} main.cpp MANUAL_FINALIZATION
        ${CoreCpp}
        ${PageCpp}
        ${ProtoCC}
        ${UtilsCpp}
        ${PageUi}
        ${QRC_RESOURCES}
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
                Qt::Quick
                Qt::Qml
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
                Qt::Quick
                Qt::Qml
                protobuf
                sqlite3
                mysqlclient
                yaml-cpp
                crypto
                ssl
        )
endif()

#set_target_properties(${PROJECT_NAME} PROPERTIES
#        ${BUNDLE_ID_OPTION}
#        WIN32_EXECUTABLE TRUE
#)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(${PROJECT_NAME})
    qt_finalize_executable(${PROJECT_NAME})
endif()