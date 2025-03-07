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
        -DSS_FILE_MANAGER_PAGE_EXPORTS
)
#--------------------------------     need you revise      ----------------------------------   BEGIN

SET(
        PLUGINS_NAME                 FileManagerPagePlugin
)
SET(
        QRC_PREFIX                   file-manager-page
)
#--------------------------------     need you revise      ----------------------------------   END
set(
        CMAKE_AUTOMOC                ON
)
set(
        CMAKE_AUTOUIC                ON
)
set(
        CMAKE_AUTORCC                ON
)

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
        Core                         Gui                        Widgets
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
        # effect
        ${CMAKE_CURRENT_SOURCE_DIR}/../effect-component/SS-mask-widget/SSMaskWidget.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../effect-component/loading-dialog/LoadingDialog.cpp
)

QT_ADD_RESOURCES(
        QRC_FILES                    ${PLUGINS_NAME}.qrc
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
                Qt6::Core                    Qt6::Gui                   Qt6::Widgets
        #--------------------------------    your lib file name    ----------------------------------   BEGIN
                PluginManagerd               ElaWidgetToolsd
        #--------------------------------    your lib file name    ----------------------------------   END
        )
ELSEIF (CMAKE_BUILD_TYPE STREQUAL "Release")
        TARGET_LINK_LIBRARIES(
                ${PLUGINS_NAME}              PUBLIC
                Qt6::Core                    Qt6::Gui                   Qt6::Widgets
        #--------------------------------    your lib file name    ----------------------------------   BEGIN
                PluginManager                ElaWidgetTools
        #--------------------------------    your lib file name    ----------------------------------   END
        )
ENDIF ()
# auto search rc file fill into *.qrc
# 清空*.qrc文件，准备覆写
file(
        GLOB_RECURSE                 RC_FILES
        ${CMAKE_SOURCE_DIR}/src/page-view/${QRC_PREFIX}/rc-page/*.*
)
FILE(
        WRITE                        ${PROJECT_NAME}.qrc
        ""
)
FILE(
        WRITE                        ${PROJECT_NAME}.qrc
        "<RCC>\n"
        "     <qresource prefix=\"/${QRC_PREFIX}\/\">\n"
)
foreach(
        RC_FILE                      ${RC_FILES}
)
        file(
                REAL_PATH            ${RC_FILE}                 REAL_RC_FILE
        )
        file(
                RELATIVE_PATH        RC_RELATIVE_PATH
                ${CMAKE_SOURCE_DIR}/src/page-view/${QRC_PREFIX}     ${REAL_RC_FILE}
        )
        file(
                APPEND               ${PROJECT_NAME}.qrc
                "           <file>${RC_RELATIVE_PATH}</file>\n"
        )
endforeach(

)
file(
        APPEND                       ${PROJECT_NAME}.qrc
        "     </qresource>\n"
        "</RCC>\n"
)

TARGET_SOURCES(
        ${PLUGINS_NAME}              PRIVATE
        ${QRC_FILES}
)