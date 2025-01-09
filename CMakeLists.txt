cmake_minimum_required(VERSION 3.16)

project(Deplieur VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt6 REQUIRED COMPONENTS Network)

set(TS_FILES Deplieur_fr_FR.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp

        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Deplieur
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc
        depliage.h depliage.cpp
        mat4x4.h mat4x4.cpp
        vec3d.h vec3d.cpp
        triangle2d.h triangle2d.cpp
        depliagescene.h depliagescene.cpp
        depliagevue3d.h depliagevue3d.cpp
        triangleitem2d.h triangleitem2d.cpp
        triangleitem3d.h triangleitem3d.cpp
        facette.h facette.cpp
        depliagevue2d.h depliagevue2d.cpp
        piecepolygonitem.h piecepolygonitem.cpp
        pieceligneitem.h pieceligneitem.cpp
        piecelangitem.h piecelangitem.cpp
        piecenumitem.h piecenumitem.cpp
        filedownloader.h filedownloader.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Deplieur APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(Deplieur SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Deplieur
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(Deplieur PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Deplieur PRIVATE Qt6::Network)

#target_link_options(Deplieur PUBLIC -sASYNCIFY -Os)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Deplieur)
endif()
set_target_properties(Deplieur PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Deplieur
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Deplieur)
endif()
