### set(GTEST_DIR "../../gtest/googletest" CACHE PATH "The path to GTest framework.")
###
### if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
###     # force this option to ON so that Google Test will use /MD instead of /MT
###     # /MD is now the default for Visual Studio, so it should be our default, too
###     option(gtest_force_shared_crt
###            "Use shared (DLL) run-time lib even when Google Test is built as static lib."
###            ON)
###
###     # GoogleTest uses std::tr1, which is deprecated in VS2017.
###     # The following is an escape-hatch macro to silence the deprecation warnings.
###     add_definitions(-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
###     add_definitions(-DGTEST_LANG_CXX11=1)
### elseif (APPLE)
###     add_definitions(-DGTEST_USE_OWN_TR1_TUPLE=1)
### endif()
###
### add_subdirectory(${GTEST_DIR} ${CMAKE_BINARY_DIR}/gtest)
### # set_property(TARGET gtest APPEND_STRING PROPERTY COMPILE_FLAGS " -w")
###
### include_directories(SYSTEM ${GTEST_DIR}/googletest/include)
###
### #
### # add_gmock_test(<target> <sources>...)
### #
### #  Adds a Google Mock based test executable, <target>, built from <sources> and
### #  adds the test so that CTest will run it. Both the executable and the test
### #  will be named <target>.
### #
### function(add_gtest_test target)
###     add_executable(${target} ${ARGN})
###     target_link_libraries(${target} gtest_main)
###
###     add_test(${target} ${target})
###
###     add_custom_command(TARGET ${target}
###                        POST_BUILD
###                        COMMAND ${target} --gtest_color=yes
###                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
###                        COMMENT "Running ${target}" VERBATIM)
###
### endfunction()


# Download and unpack googletest at configure time
configure_file(cmake/modules/CMakeLists.txt.gtest-download googletest-download/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download" )
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download" )

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This adds
# the following targets: gtest, gtest_main, gmock
# and gmock_main
add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src"
                 "${CMAKE_BINARY_DIR}/googletest-build")

# The gtest/gmock targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if(CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include"
                        "${gmock_SOURCE_DIR}/include")
endif()

function(add_gtest_test target)
    add_executable(${target} ${ARGN})
    target_link_libraries(${target} gtest_main)

    add_test(${target} ${target})

    add_custom_command(TARGET ${target}
                       POST_BUILD
                       COMMAND ${target} --gtest_color=yes
                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                       COMMENT "Running ${target}" VERBATIM)

endfunction()