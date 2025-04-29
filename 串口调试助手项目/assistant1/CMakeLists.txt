cmake_minimum_required(VERSION 3.16)

#项目名称
project(assistant1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON) #用户界面编译器，将根据.ui文件生成相应的.h文件，例如根据mainwindow.ui生成ui_mainwindow.h。
set(CMAKE_AUTOMOC ON)#元对象编译器，用于处理QT拓展的C++语法。
set(CMAKE_AUTORCC ON)#rcc工具用于在构建过程中将资源嵌入到Qt应用程序中
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets SerialPort)


# set(HEADER_DIR ${CMAKE_SOURCE_DIR}/include)
# set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
# set(FORM_DIR ${CMAKE_SOURCE_DIR}/form)
# 查找某个路径下的所有源文件，此处将src目录下的所有文件名赋给SRC变量
aux_source_directory(src SRC)
# file(GLOB/GLOB_RECURSE 变量名 要搜索的文件路径和文件类型)
#GLOB: 将指定目录下搜索到的满足条件的所有文件名生成一个列表，并将其存储到变量中。
#GLOB_RECURSE：递归搜索指定目录，将搜索到的满足条件的文件名生成一个列表，并将其存储到变量中。
#CMAKE_CURRENT_SOURCE_DIR 宏表示当前访问的 CMakeLists.txt 文件所在的路径。
FILE(GLOB INC "include/*.h")
# 添加qrc文件
file(GLOB QRC "resources/*.qrc")

#CMAKE_AUTOUIC_SEARCH_PATHS告诉 CMake 哪些目录下的 .ui 文件应该被 AutoUic 处理。设置了这个变量后，CMake 将会在这些指定的目录下搜索 .ui 文件，并将它们转换为相应的头文件.h。
list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS "form")

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
    ${INC}
    ${QRC}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(assistant1
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET assistant1 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(assistant1 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        # 定义工程会生成一个可执行程序 add_executable(可执行程序名 源文件名称)
        add_executable(assistant1
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(assistant1 PRIVATE include)
target_link_libraries(assistant1 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::SerialPort)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.assistant1)
endif()
set_target_properties(assistant1 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS assistant1
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(assistant1)
endif()
