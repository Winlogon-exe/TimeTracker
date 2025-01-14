cmake_minimum_required(VERSION 3.5)

project(TimerTracker VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwidget.cpp
        inc/mainwidget.h
        inc/logicMainWidget.h
        src/logicMainWiget.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TimerTracker
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
    )

else()
    if(ANDROID)
        add_library(TimerTracker SHARED
                ${PROJECT_SOURCES}
        )
    else()
        add_executable(TimerTracker
                ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(TimerTracker PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(TimerTracker PRIVATE ${CMAKE_SOURCE_DIR}/inc)
target_include_directories(TimerTracker PRIVATE ${CMAKE_SOURCE_DIR}/src)

if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.TimerTracker)
endif()
set_target_properties(TimerTracker PROPERTIES
        ${BUNDLE_ID_OPTION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE FALSE
)

include(GNUInstallDirs)
install(TARGETS TimerTracker
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TimerTracker)
endif()
