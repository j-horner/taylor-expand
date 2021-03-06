
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
    include_directories(SYSTEM "${gtest_SOURCE_DIR}/include"
						       "${gmock_SOURCE_DIR}/include")
endif()

function(add_gtest_test target)
    add_executable(${target} ${ARGN})

    target_link_libraries(${target} PUBLIC gtest_main)
    
	target_compile_options(${target} PRIVATE
                           $<$<CXX_COMPILER_ID:Clang>:
                                >
                           $<$<CXX_COMPILER_ID:MSVC>:
                                /W3 /WX /EHsc /bigobj>)



    add_test(${target} ${target})

    add_custom_command(TARGET ${target}
                       POST_BUILD
                       COMMAND ${target} --gtest_color=yes
                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                       COMMENT "Running ${target}" VERBATIM)

endfunction()