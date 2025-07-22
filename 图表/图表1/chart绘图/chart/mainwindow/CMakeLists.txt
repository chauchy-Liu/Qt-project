
# project(mainwindow)
# set(HEADER_DIR ${CMAKE_SOURCE_DIR}/include)
# set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
# set(FORM_DIR ${CMAKE_SOURCE_DIR}/form)
# 查找某个路径下的所有源文件，此处将src目录下的所有文件名赋给SRC变量
aux_source_directory(src SRC)
# file(GLOB/GLOB_RECURSE 变量名 要搜索的文件路径和文件类型)
#GLOB: 将指定目录下搜索到的满足条件的所有文件名生成一个列表，并将其存储到变量中。
#GLOB_RECURSE：递归搜索指定目录，将搜索到的满足条件的文件名生成一个列表，并将其存储到变量中。
#CMAKE_CURRENT_SOURCE_DIR 宏表示当前访问的 CMakeLists.txt 文件所在的路径。
message("打印CMAKE_CURRENT_SOURCE_DIR路径${CMAKE_CURRENT_SOURCE_DIR}")
FILE(GLOB INC "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
# 添加qrc文件
file(GLOB QRC "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.qrc")
message("src目录下CmakeList中QRC文件路径：${QRC}")
#CMAKE_AUTOUIC_SEARCH_PATHS告诉 CMake 哪些目录下的 .ui 文件应该被 AutoUic 处理。设置了这个变量后，CMake 将会在这些指定的目录下搜索 .ui 文件，并将它们转换为相应的头文件.h。
message("打印AUTOUIC路径${CMAKE_AUTOUIC_SEARCH_PATHS}")
list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS "form")
message("打印AUTOUIC路径${CMAKE_AUTOUIC_SEARCH_PATHS}")

# qt6_wrap_ui(MOC_ ${FORM_DIR}/mainwindow.ui)
# qt6_wrap_ui(UI_HEADERS ${FORM_DIR}/mainwindow.ui)

# include_directories(
#     ${CMAKE_SOURCE_DIR}/include
#     ${UI_HEADERS}
# )


# 生成链接库
add_library (MainWindow STATIC ${SRC} ${INC} ${QRC})
target_include_directories(MainWindow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/) # 必须是目录
target_link_libraries(MainWindow PUBLIC Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Core)
