cmake_minimum_required(VERSION 3.1...3.25)

project(KeybordMouse)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_subdirectory(src)
add_subdirectory(thirdPart/QHotkey)
add_subdirectory(thirdPart/ElaWidgetTools)
