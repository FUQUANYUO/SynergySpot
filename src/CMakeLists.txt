cmake_minimum_required(VERSION 3.27)
project(SynergySpot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "D:/QT/6.4.3/msvc2019_64")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(SynergySpot main.cpp
        page/base-arch/arch-page/ArchPage.cpp
        page/base-arch/arch-page/ArchPage.h
        page/base-arch/arch-page/ArchPage.ui
        page/base-arch/contact-page/ContactPage.cpp
        page/base-arch/contact-page/ContactPage.h
        page/base-arch/contact-page/ContactPage.ui
        page/base-arch/file-page/FilePage.cpp
        page/base-arch/file-page/FilePage.h
        page/base-arch/file-page/FilePage.ui
        page/base-arch/more-option-page/MoreOptionPage.cpp
        page/base-arch/more-option-page/MoreOptionPage.h
        page/base-arch/more-option-page/MoreOptionPage.ui
        page/base-arch/msg-page/MsgPage.cpp
        page/base-arch/msg-page/MsgPage.h
        page/base-arch/msg-page/MsgPage.ui
        page/base-arch/msg-page/MsgPage.cpp
        page/base-arch/msg-page/MsgPage.h
        page/base-arch/msg-page/MsgPage.ui
        page/base-arch/avatar-page/AvatarPage.cpp
        page/base-arch/avatar-page/AvatarPage.h
        page/base-arch/avatar-page/AvatarPage.ui
        page/base-arch/avatar-page/AvatarPage.h
        page/land-page/LandPage.cpp
        page/land-page/LandPage.h
        page/land-page/LandPage.ui
)
target_link_libraries(SynergySpot
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

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
