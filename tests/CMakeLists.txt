cmake_minimum_required(VERSION 3.16)
project(Tests
        LANGUAGES CXX)

option(INSTALL_GTEST OFF)
#include(FetchContent)
#FetchContent_Declare(
#        googletest
#        GIT_REPOSITORY https://github.com/google/googletest.git
#        GIT_TAG release-1.10.0
#)
#FetchContent_MakeAvailable(googletest)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download)
if (result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif ()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download)
if (result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif ()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif ()

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    set(gtest_mainDir "$<SHELL_PATH:$<TARGET_FILE_DIR:gtest_main>>")
    set(gtest_mainExecPath "PATH=${gtest_mainDir}$<SEMICOLON>$ENV{PATH}")

    set(CrossGuidDir "$<SHELL_PATH:$<TARGET_FILE_DIR:CrossGuid::CrossGuid>>")
    set(CrossGuidExecPath "PATH=${CrossGuidDir}$<SEMICOLON>${CrossGuidDir}$<SEMICOLON>$ENV{PATH}")
endif ()

add_executable(testBasicCrossGuid testBasicCrossGuid.cpp)
target_link_libraries(testBasicCrossGuid PRIVATE gtest_main CrossGuid::CrossGuid)
add_test(NAME Test_testBasicCrossGuid COMMAND testBasicCrossGuid)
if (WIN32)
    set_tests_properties(Test_testBasicCrossGuid PROPERTIES
            ENVIRONMENT "${CrossGuidExecPath}"
            )
endif ()
