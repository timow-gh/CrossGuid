add_executable(printGUID printGUID.cpp)
target_link_libraries(printGUID PRIVATE CrossGuid::CrossGuid)

include(HelperFunctions)
if (MSVC)
    setMSVCOutputDirectories(printGUID)
endif ()
