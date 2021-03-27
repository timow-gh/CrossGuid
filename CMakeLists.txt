cmake_minimum_required(VERSION 3.14)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Do not build in-source.
    Please remove CMakeCache.txt and the CMakeFiles/ directory. Then build out-of-source.")
endif ()

project(
        "CrossGuid"
        VERSION 0.1.0
        LANGUAGES CXX
)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif ()

include(CTest)
include(CMakeDependentOption)
include(GNUInstallDirs)
include(InstallRequiredSystemLibraries)
include(CMakePackageConfigHelpers)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(LTO)
include(Warnings)
include(CopyDllsForDebug)
include(Coverage)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(${PROJECT_NAME}_USE_ADDITIONAL_SOURCEFILE "Use the additional source file" ON)
option(${PROJECT_NAME}_INSTALL "" ON)
option(${PROJECT_NAME}_EXPORT_TARGETS_TO_USER_REGISTRY "" OFF)


cmake_dependent_option(${PROJECT_NAME}_BUILD_TESTS
                       "Enable ${PROJECT_NAME} project tests targets" ON
                       "BUILD_TESTING" OFF
                       )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

find_lto(CXX)

set(TARGET_NAME ${PROJECT_NAME})

set(TARGET_HEADER
    include/CrossGuid/Guid.hpp
    )

set(TARGET_SOURCES
    source/Guid.cpp
    )

if (NOT_APPLE)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
endif ()

add_library(${TARGET_NAME}
            ${TARGET_SOURCES}
            ${TARGET_HEADER}
            )

if(WIN32)
    target_compile_definitions(${TARGET_NAME} PRIVATE GUID_WINDOWS)
elseif(APPLE)
    find_library(CFLIB CoreFoundation)
    target_link_libraries(${TARGET_NAME} ${CFLIB})
    target_compile_definitions(${TARGET_NAME} PRIVATE GUID_CFUUID)
else()
    find_package(uuid REQUIRED)
    target_link_libraries(${TARGET_NAME} PRIVATE uuid::uuid)
    target_compile_definitions(${TARGET_NAME} PRIVATE GUID_LIBUUID)
endif()

include(GenerateExportHeader)
generate_export_header(${TARGET_NAME})

target_include_directories(${TARGET_NAME}
                           PUBLIC
                           "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/>"
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                           "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
                           )

set_target_properties(${TARGET_NAME}
                      PROPERTIES
                      SOVERSION ${PROJECT_VERSION_MAJOR}
                      VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
                      DEBUG_POSTFIX d
                      )

add_library(CrossGuid::CrossGuid ALIAS ${TARGET_NAME})

target_set_warnings(${TARGET_NAME} ENABLE ALL AS_ERROR ALL DISABLE Annoying)
target_enable_lto(${TARGET_NAME} optimized)

if (${PROJECT_NAME}_BUILD_TESTS)
    list(APPEND CUSTOM_COVERAGE_EXCLUDE "/tests/")
    add_subdirectory(tests)
    configure_file(
            ${CMAKE_CURRENT_LIST_DIR}/cmake/CTestCustom.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake
            @ONLY
    )
endif ()

if (${PROJECT_NAME}_INSTALL)
    set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_LIST_DIR}/README.md)
    include(CPack)

    set(${PROJECT_NAME}_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "Path to install ${PROJECT_NAME} Config*.cmake files to.")
    set(${PROJECT_NAME}_MODULE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake" CACHE STRING "Path to install ${PROJECT_NAME}'s .cmake  module files to.")

    write_basic_package_version_file(
            ${PROJECT_NAME}ConfigVersion.cmake
            VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
            COMPATIBILITY SameMajorVersion
    )

    configure_package_config_file(
            ${PROJECT_SOURCE_DIR}/cmake/${TARGET_NAME}Config.cmake.in
            ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
            INSTALL_DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKEDIR}
            PATH_VARS ${PROJECT_NAME}_MODULE_INSTALL_DIR
            NO_SET_AND_CHECK_MACRO
            NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )

    install(
            TARGETS
            ${TARGET_NAME}
            EXPORT ${PROJECT_NAME}_Targets
            INCLUDES
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(
            EXPORT ${PROJECT_NAME}_Targets
            NAMESPACE ${PROJECT_NAME}::
            FILE ${PROJECT_NAME}Targets.cmake
            DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKEDIR}
    )

    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            DESTINATION
            "${${PROJECT_NAME}_INSTALL_CMAKEDIR}"
            )

    install(FILES
            "${TARGET_HEADER}"
            DESTINATION
            "${CMAKE_INSTALL_INCLUDEDIR}/crossguid")

    string(TOLOWER "${TARGET_NAME}_export.h" GENERATED_TARGET_EXPORTS_FILE)
    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${GENERATED_TARGET_EXPORTS_FILE}"
            DESTINATION
            "${CMAKE_INSTALL_INCLUDEDIR}/crossguid")

    if (${PROJECT_NAME}_EXPORT_TARGETS_TO_USER_REGISTRY)
        export(EXPORT ${PROJECT_NAME}_Targets
               NAMESPACE ${PROJECT_NAME}::
               FILE "${PROJECT_NAME}Targets.cmake"
               )
        export(PACKAGE ${PROJECT_NAME})
    endif ()
endif ()

