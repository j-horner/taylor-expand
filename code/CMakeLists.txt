cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

set (CMAKE_CXX_STANDARD 17)

project("Fields")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

enable_testing()
include(gtest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}../../bin)

add_subdirectory(src)

message("${PROJECT_SOURCE_DIR}")
