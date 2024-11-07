cmake_minimum_required(VERSION 3.5)

project(gotify-tray++ VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(KF6Notifications QUIET)
if(KF6Notifications_FOUND)
    add_definitions(-DUSE_KDE)
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network WebSockets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network WebSockets Sql)

if (WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/gotify-tray++.rc")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(PROJECT_SOURCES
        src/main.cpp
)

qt_add_executable(gotify-tray++
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${APP_ICON_RESOURCE_WINDOWS}

    resources.qrc
    README.md
    src/tray.h src/tray.cpp
    src/mainapplication.h src/mainapplication.cpp
    src/settings.h src/settings.cpp
    src/gotifyapi.h src/gotifyapi.cpp
    src/requesthandler.h src/requesthandler.cpp
    src/gotifymodels.h src/gotifymodels.cpp
    src/listener.h src/listener.cpp
    src/processthread.h src/processthread.cpp

    src/utils.h src/utils.cpp
    src/cache.h src/cache.cpp


    src/serverinfodialog.h src/serverinfodialog.cpp src/serverinfodialog.ui
    src/settingsdialog.h src/settingsdialog.cpp src/settingsdialog.ui
    src/appversion.h src/appversion.cpp

)

if(KF6Notifications_FOUND)
target_link_libraries(gotify-tray++ PRIVATE
    KF6::Notifications
)
endif()

target_link_libraries(gotify-tray++ PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::WebSockets Qt${QT_VERSION_MAJOR}::Sql)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.gotify-tray++)
endif()
set_target_properties(gotify-tray++ PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS gotify-tray++
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(gotify-tray++)
endif()
