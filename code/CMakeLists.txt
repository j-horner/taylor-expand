cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

project("Fields")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

enable_testing()
include(gtest)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra -Werror")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(warnings "/W4 /WX /EHsc /std:c++17")
endif()

if (NOT CONFIGURED_ONCE)
    set(CMAKE_CXX_FLAGS "${warnings}" CACHE STRING "Flags used by the compiler during all build types." FORCE)
    set(CMAKE_C_FLAGS   "${warnings}" CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}../../bin)

add_subdirectory(src)

set(CONFIGURED_ONCE TRUE CACHE INTERNAL "A flag showing that CMake has configured at least once.")