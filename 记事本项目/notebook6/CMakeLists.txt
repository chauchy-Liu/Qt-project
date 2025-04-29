cmake_minimum_required(VERSION 3.16)

#modify:
project(notebook6 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Core5Compat)


# set(HEADER_DIR ${CMAKE_SOURCE_DIR}/include)
# set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
# set(FORM_DIR ${CMAKE_SOURCE_DIR}/form)
#添加源文件
aux_source_directory(src SRC)
aux_source_directory(src/MyButton MYBUTTON)
aux_source_directory(src/MyTextEdit MYTEXTEDIT)
# 添加头文件
FILE(GLOB INC "include/*.h")
# 添加ui文件路径
list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS "form")
# 添加qrc文件
file(GLOB QRC "resources/*.qrc")

# qt6_wrap_ui(MOC_ ${FORM_DIR}/mainwindow.ui)
# qt6_wrap_ui(UI_HEADERS ${FORM_DIR}/mainwindow.ui)

# include_directories(
#     ${CMAKE_SOURCE_DIR}/include
#     ${UI_HEADERS}
# )

set(PROJECT_SOURCES
    # ${SOURCE_DIR}/main.cpp
    # ${SOURCE_DIR}/mainwindow.cpp
    # ${UI_HEADERS}
    # ${HEADER_DIR}
    ${SRC}
    ${MYBUTTON}
    ${MYTEXTEDIT}
    ${INC}
    ${QRC}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(notebook6
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET project1 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(notebook6 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(notebook6
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(notebook6 PRIVATE include)
target_link_libraries(notebook6 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Core5Compat)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.notebook6)
endif()
set_target_properties(notebook6 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS notebook6
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(notebook6)
endif()
