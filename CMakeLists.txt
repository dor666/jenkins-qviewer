cmake_minimum_required(VERSION 2.8.11)

project(jenkins-qviewer)

set(PROJECT_LIB_NAME ${CMAKE_PROJECT_NAME}-lib)

add_subdirectory(src)
add_subdirectory(tests)
