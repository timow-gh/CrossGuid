cmake_minimum_required(VERSION 3.16)

project("CrossGuid"
        VERSION 0.1.0
        LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif ()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

option(${PROJECT_NAME}_INSTALL "" ON)
option(${PROJECT_NAME}_BUILD_TESTS "" OFF)
option(${PROJECT_NAME}_BUILD_EXAMPLES "" OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

include(HelperFunctions)
if (NOT CMAKE_GENERATOR STREQUAL "Xcode")
    set(CMAKE_INSTALL_RPATH $ORIGIN)
endif ()

set(TARGET_NAME CrossGuid)
add_library(${TARGET_NAME} SHARED
        source/Guid.cpp
        include/CrossGuid/Guid.hpp)
add_library(CrossGuid::CrossGuid ALIAS ${TARGET_NAME})

if (MSVC)
    target_compile_definitions(${TARGET_NAME} PRIVATE GUID_WINDOWS)
elseif (CMAKE_GENERATOR STREQUAL "Xcode")
    find_library(CFLIB CoreFoundation)
    target_link_libraries(${TARGET_NAME} ${CFLIB})
    target_compile_definitions(${TARGET_NAME} PRIVATE GUID_CFUUID)
else ()
    find_package(uuid REQUIRED)
    target_link_libraries(${TARGET_NAME} PRIVATE uuid::uuid)
    target_compile_definitions(${TARGET_NAME} PRIVATE GUID_LIBUUID)
endif ()

include(GenerateExportHeader)
generate_export_header(${TARGET_NAME})

target_include_directories(${TARGET_NAME}
        PUBLIC
        "$<BUILD_INTERFACE:${CrossGuid_BINARY_DIR}/>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
        )

set_target_properties(${TARGET_NAME}
        PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
        DEBUG_POSTFIX d)

if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)
    setMSVCOutputDirectories(${TARGET_NAME})
endif ()

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

if (${PROJECT_NAME}_BUILD_TESTS)
    include(CTest)
    add_subdirectory(tests)
endif ()

if (${PROJECT_NAME}_INSTALL)
    include(GNUInstallDirs)
    include(InstallRequiredSystemLibraries)
    include(CMakePackageConfigHelpers)

    set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_LIST_DIR}/README.md)
    include(CPack)

    if (NOT CMAKE_GENERATOR STREQUAL "Xcode")
        file(RELATIVE_PATH relDir
                ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
                ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
                )
        set(CMAKE_INSTALL_RPATH $ORIGIN $ORIGIN/${relDir})
    endif ()

    write_basic_package_version_file(
            ${PROJECT_NAME}ConfigVersion.cmake
            VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
            COMPATIBILITY SameMajorVersion
    )

    set(${PROJECT_NAME}_INSTALL_CMAKEDIR
            "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
            CACHE STRING "Path to install ${PROJECT_NAME} Config*.cmake files to.")
    set(${PROJECT_NAME}_MODULE_INSTALL_DIR
            "${CMAKE_INSTALL_LIBDIR}/cmake"
            CACHE STRING "Path to install ${PROJECT_NAME}'s .cmake  module files to.")

    configure_package_config_file(
            ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
            ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
            INSTALL_DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKEDIR}
            PATH_VARS ${PROJECT_NAME}_MODULE_INSTALL_DIR
            NO_SET_AND_CHECK_MACRO
            NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )

    install(TARGETS ${TARGET_NAME}
            EXPORT ${PROJECT_NAME}_Targets
            RUNTIME
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            INCLUDES
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            )

    install(EXPORT ${PROJECT_NAME}_Targets
            NAMESPACE ${PROJECT_NAME}::
            FILE ${PROJECT_NAME}Targets.cmake
            DESTINATION ${${PROJECT_NAME}_INSTALL_CMAKEDIR}
            )

    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            DESTINATION "${${PROJECT_NAME}_INSTALL_CMAKEDIR}"
            )

    install(FILES "include/CrossGuid/Guid.hpp"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/CrossGuid"
            )

    string(TOLOWER "crossGuid_export.h" GENERATED_TARGET_EXPORTS_FILE)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${GENERATED_TARGET_EXPORTS_FILE}"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/CrossGuid"
            )

    if (${PROJECT_NAME}_EXPORT_TARGETS_TO_USER_REGISTRY)
        export(EXPORT ${PROJECT_NAME}_Targets
                NAMESPACE ${PROJECT_NAME}::
                FILE "${PROJECT_NAME}Targets.cmake"
                )
        export(PACKAGE ${PROJECT_NAME})
    endif ()
endif ()

