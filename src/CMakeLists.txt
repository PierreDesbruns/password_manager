# Copyright (C) 2024 Pierre Desbruns
# SPDX-License-Identifier: LGPL-3.0-only

cmake_minimum_required(VERSION 3.5)

project(password_manager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        ressources.qrc
        main.cpp
        mainwindow.cpp
        mainwindow.h
        addentrywindow.cpp
        addentrywindow.h
        delentrywindow.cpp
        delentrywindow.h
        regentrywindow.cpp
        regentrywindow.h
        loginwindow.cpp
        loginwindow.h
        pwmsecurity.cpp
        pwmsecurity.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(password_manager
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET password_manager APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(password_manager SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(password_manager
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Including libsodium
target_include_directories(password_manager PRIVATE "C:/DevTools/libsodium-win64/include")

target_link_libraries(password_manager
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE "C:/DevTools/libsodium-win64/lib/libsodium.a"
)

set_target_properties(password_manager PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS password_manager
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(password_manager)
endif()
