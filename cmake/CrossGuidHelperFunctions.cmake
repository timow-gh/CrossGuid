function(calcPATHWithCrossGuidTarget result)
    set(CrossGuidDir "$<SHELL_PATH:$<TARGET_FILE_DIR:CrossGuid::CrossGuid>>")
    set(${result} "PATH=${CrossGuidDir}$<SEMICOLON>$ENV{PATH}" PARENT_SCOPE)
endfunction()
