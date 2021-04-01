include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.10.0
)
FetchContent_MakeAvailable(googletest)

include(HelperFunctions)
if (MSVC)
    set(PathWithCrossGuidTarget)
    calcPATHWithCrossGuidTarget(PathWithCrossGuidTarget)
endif ()

function(add_crossguid_test targetName)
    add_executable(${targetName} ${ARGN})
    target_link_libraries(${targetName}
            PRIVATE
            gtest_main
            CrossGuid::CrossGuid)
    add_test(NAME ${targetName} COMMAND ${targetName})
    if (MSVC)
        set_tests_properties(${targetName}
                PROPERTIES ENVIRONMENT "${PathWithCrossGuidTarget}")
    endif ()
endfunction()

add_crossguid_test(testBasicCrossGuid testBasicCrossGuid.cpp)
