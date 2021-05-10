
function(calcPATHWithCrossGuidTarget result)
    set(CrossGuidDir "$<SHELL_PATH:$<TARGET_FILE_DIR:CrossGuid::CrossGuid>>")
    set(${result} "PATH=${CrossGuidDir}$<SEMICOLON>$ENV{PATH}" PARENT_SCOPE)
endfunction()

function(setMSVCOutputDirectories targetName)
    set_target_properties(${targetName}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
            ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
            COMPILE_PDB_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
            PDB_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
            LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
endfunction()
