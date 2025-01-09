cmake_minimum_required(VERSION 3.5)

project(KeybordMouse)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_DEFAULT_MAJOR_VERSION 6)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_subdirectory(src)
add_subdirectory(thirdPart/QHotkey)
add_subdirectory(thirdPart/ElaWidgetTools)
