option(INSTALL_GTEST OFF)
mark_as_advanced(INSTALL_GTEST)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.10.0
)
FetchContent_MakeAvailable(googletest)

function(add_crossguid_test targetName)
    add_executable(${targetName} ${ARGN})
    target_link_libraries(${targetName}
            PRIVATE
            gtest_main
            CrossGuid::CrossGuid)
    add_test(NAME ${targetName} COMMAND ${targetName})
endfunction()

add_crossguid_test(testBasicCrossGuid testBasicCrossGuid.cpp)
