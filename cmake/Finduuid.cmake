find_path(uuid_INCLUDE_DIR uuid)
find_library(uuid_LIBRARY NAMES uuid uuidd)
mark_as_advanced(uuid_INCLUDE_DIR uuid_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(uuid REQUIRED_VARS uuid_LIBRARY uuid_INCLUDE_DIR)

if (uuid_FOUND AND NOT TARGET uuid::uuid)
    add_library(uuid::uuid UNKNOWN IMPORTED)
    set_target_properties(uuid::uuid PROPERTIES
                          IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
                          IMPORTED_LOCATION "${uuid_LIBRARY}"
                          INTERFACE_INCLUDE_DIRECTORIES "${uuid_INCLUDE_DIR}"
                          )
endif ()
