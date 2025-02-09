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
# preprocess define
ADD_DEFINITIONS(
        -DSS_COMMON_DATA_EXPORTS
        -DSQLITE_HAS_CODEC
)
#--------------------------------     need you revise      ----------------------------------   BEGIN

SET(
        PLUGINS_NAME                 CommonDataPlugin
)
#--------------------------------     need you revise      ----------------------------------   END

IF      (WIN32)
    SET(CUR_PLATFORM             Win)
ELSEIF  (LINUX)
    SET(CUR_PLATFORM             Linux)
ENDIF   ()

PROJECT(
        ${PLUGINS_NAME}              LANGUAGES                  CXX
)

FIND_PACKAGE(Qt6
        REQUIRED                     COMPONENTS
        Core                         Gui
)

# 头文件路径添加       add head file path
#--------------------------------   third party include    ----------------------------------   BEGIN
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/lib-common/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/utils)
#--------------------------------   third party include    ----------------------------------   END

# 库文件路径添加       add lib file path
#--------------------------------      third party lib     ----------------------------------   BEGIN
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/lib-common/lib/${CUR_PLATFORM})
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/out)
#--------------------------------      third party lib     ----------------------------------   END
FILE(
        GLOB_RECURSE                 SRC_CPP                    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# utils src code
SET(
        SRC_CPP                      ${SRC_CPP}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/log/Log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/get-time/GetCurTime.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/db-lite/LiteConn.cpp
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
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    TARGET_LINK_LIBRARIES(
            ${PLUGINS_NAME}              PUBLIC
            Qt6::Core                    Qt6::Gui
            #--------------------------------    your lib file name    ----------------------------------   BEGIN
            yaml-cppd                    libcrypto                     libssl
            libsqlite3                   sqlcipher
            #--------------------------------    your lib file name    ----------------------------------   END
    )
ELSEIF (CMAKE_BUILD_TYPE STREQUAL "Release")
    TARGET_LINK_LIBRARIES(
            ${PLUGINS_NAME}              PUBLIC
            Qt6::Core                    Qt6::Gui
            #--------------------------------    your lib file name    ----------------------------------   BEGIN
            yaml-cpp                     libcrypto                     libssl
            libsqlite3                   sqlcipher
            #--------------------------------    your lib file name    ----------------------------------   END
    )
ENDIF ()
