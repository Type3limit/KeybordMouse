cmake_minimum_required(VERSION 3.1...3.25)
project(KeybordMouse)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(BUILD_TYPE "Release")
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    set(BUILD_TYPE "Debug")
endif()

if(WIN32)
    set(PLATFORM "windows")
elseif(APPLE)
    set(PLATFORM "macos")
elseif(UNIX)
    set(PLATFORM "linux")
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../bin/${PLATFORM}/${BUILD_TYPE})


find_package(Qt${QT_DEFAULT_MAJOR_VERSION} COMPONENTS
        Core
        Gui
        Svg
        Widgets
        Network
        REQUIRED)


file(GLOB_RECURSE RESOURCES "*.cpp" "*.h" "*.hpp" "*.ui" "*.qrc")

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/../thirdPart/QHotkey/QHotkey
        ${CMAKE_CURRENT_LIST_DIR}/../thirdPart/ElaWidgetTools/src/include
)

set(LinkLib
        Qt${QT_DEFAULT_MAJOR_VERSION}::Core
        Qt${QT_DEFAULT_MAJOR_VERSION}::Gui
        Qt${QT_DEFAULT_MAJOR_VERSION}::Svg
        Qt${QT_DEFAULT_MAJOR_VERSION}::Widgets
        Qt${QT_DEFAULT_MAJOR_VERSION}::Network)



list(APPEND LinkLib qhotkey ElaWidgetTools)

if(WIN32)
    add_executable(${CMAKE_PROJECT_NAME} WIN32 ${RESOURCES})
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE user32 DbgHelp)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE NOMINMAX)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE appicon.rc)
else()
    add_executable(${CMAKE_PROJECT_NAME}  ${RESOURCES} appicon.rc)
endif()
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${LinkLib})


if (APPLE)
    set(MACOSX_BUNDLE true)
    set(MACOSX_BUNDLE_BUNDLE_NAME ${CMAKE_PROJECT_NAME})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.ismisv.${CMAKE_PROJECT_NAME}")
    set(MACOSX_BUNDLE_ICON_FILE "app.ico")
    set(MACOSX_BUNDLE_INFO_STRING ${CMAKE_PROJECT_NAME})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "1.0.0.1")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")

endif()

set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/app.ico" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
set_target_properties(
        ${CMAKE_PROJECT_NAME}
        PROPERTIES
        RESOURCE "./app.ico"
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
)

add_custom_command(
        TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ElaWidgetTools> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)