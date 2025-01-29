CMAKE_MINIMUM_REQUIRED(VERSION 3.20)
SET(
        CMAKE_ARCHIVE_OUTPUT_DIRECTORY                          ${CMAKE_SOURCE_DIR}/src/out
)
SET(
        CMAKE_LIBRARY_OUTPUT_DIRECTORY                          ${CMAKE_SOURCE_DIR}/src/out
)
SET(
        CMAKE_RUNTIME_OUTPUT_DIRECTORY                          ${CMAKE_SOURCE_DIR}/src/out
)
SET(
        PLUGINS_NAME                 PluginManager
)
set(
        CMAKE_AUTOMOC                ON
)
set(
        CMAKE_AUTOUIC                ON
)
set(
        CMAKE_AUTORCC                ON
)
ADD_DEFINITIONS(
        -DSS_PLUGIN_MANAGER_EXPORTS
)

IF      (WIN32)
        SET(CUR_PLATFORM             Win)
ELSEIF  (LINUX)
        SET(CUR_PLATFORM             Linux)
ENDIF   ()

PROJECT(
        ${PLUGINS_NAME}
)

FIND_PACKAGE(Qt6
        REQUIRED                     COMPONENTS
        Core                         Gui                        Widgets
)

# 头文件路径添加       add head file path
#--------------------------------   third party include    ----------------------------------   BEGIN
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/lib-common/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/utils)
#--------------------------------   third party include    ----------------------------------   END

# 库文件路径添加       add lib file path
#--------------------------------      third party lib     ----------------------------------   BEGIN
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/lib-common/lib/${CUR_PLATFORM})
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/out)
#--------------------------------      third party lib     ----------------------------------   END

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
FILE(
        GLOB_RECURSE                 SRC_CPP                    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# utils src code
SET(
        SRC_CPP                      ${SRC_CPP}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/log/Log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/get-time/GetCurTime.cpp
)

ADD_LIBRARY(
        ${PLUGINS_NAME}              SHARED
        ${SRC_CPP}
)

SET_TARGET_PROPERTIES(
        ${PLUGINS_NAME}              PROPERTIES
        OUTPUT_NAME                  ${PLUGINS_NAME}
        OUTPUT_NAME_DEBUG            ${PLUGINS_NAME}d
        OUTPUT_NAME_RELEASE          ${PLUGINS_NAME}
)

TARGET_LINK_LIBRARIES(
        ${PLUGINS_NAME}              PUBLIC
        Qt6::Core                    Qt6::Gui                   Qt6::Widgets
#--------------------------------    your lib file name    ----------------------------------   BEGIN
#--------------------------------    your lib file name    ----------------------------------   END
)