cmake_minimum_required(VERSION 3.1.3)

project(jenkins-qviewer)

set(PROJECT_LIB_NAME ${CMAKE_PROJECT_NAME}-lib)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DQT_QML_DEBUG)
endif()

add_subdirectory(src)
add_subdirectory(tests)
