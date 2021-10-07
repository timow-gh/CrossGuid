cmake_minimum_required(VERSION 3.19)

include(FetchContent)
FetchContent_Declare(twCMakeUtils
        GIT_REPOSITORY https://github.com/timow-gh/twCMakeUtils.git
        GIT_TAG main)
FetchContent_GetProperties(twCMakeUtils)
set(lcs_twCMakeUtils "")
string(TOLOWER "twCMakeUtils" lcs_twCMakeUtils)
if (NOT ${lcs_twCMakeUtils}_POPULATED)
    FetchContent_Populate(twCMakeUtils)
    list(APPEND CMAKE_MODULE_PATH ${${lcs_twCMakeUtils}_SOURCE_DIR}/modules)
    include(CMakeHelperFunctions)
endif ()

project("CrossGuid"
        VERSION 0.1.0
        LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

option(${PROJECT_NAME}_INSTALL "" ON)
option(${PROJECT_NAME}_BUILD_TESTS "" OFF)
option(${PROJECT_NAME}_BUILD_EXAMPLES "" OFF)

preamble()

add_library(CrossGuid
        source/Guid.cpp
        include/CrossGuid/Guid.hpp)
add_library(CrossGuid::CrossGuid ALIAS CrossGuid)

if (MSVC)
    target_compile_definitions(CrossGuid PRIVATE GUID_WINDOWS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)
elseif (CMAKE_GENERATOR STREQUAL "Xcode")
    find_library(CFLIB CoreFoundation)
    target_link_libraries(CrossGuid ${CFLIB})
    target_compile_definitions(CrossGuid PRIVATE GUID_CFUUID)
else ()
    find_package(uuid REQUIRED)
    target_link_libraries(CrossGuid PRIVATE uuid::uuid)
    target_compile_definitions(CrossGuid PRIVATE GUID_LIBUUID)
endif ()

include(GenerateExportHeader)
generate_export_header(CrossGuid)

target_include_directories(CrossGuid
        PUBLIC
        "$<BUILD_INTERFACE:${CrossGuid_BINARY_DIR}/>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>")

set_target_properties(CrossGuid
        PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
        DEBUG_POSTFIX d
        LINKER_LANGUAGE CXX
        EXPORT_NAME CrossGuid)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

if (${PROJECT_NAME}_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif ()

if (${PROJECT_NAME}_INSTALL)
    install(TARGETS CrossGuid
            EXPORT CrossGuidExportSet
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(FILES "include/CrossGuid/Guid.hpp" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/CrossGuid")
    string(TOLOWER "crossGuid_export.h" GENERATED_TARGET_EXPORTS_FILE)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${GENERATED_TARGET_EXPORTS_FILE}"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/CrossGuid")
    setupExportSetInstall("${PROJECT_NAME}" "CrossGuidExportSet")
endif ()

