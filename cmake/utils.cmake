#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   CMake Utility Scripts
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Sets a directory as a custom target, then adds it as a dependency for the specified target
# @param TGT: target which depends on the directory
# @param DIR: directory to add as a custom target
# @param SRC: source path of directory
# @param DST: destination path of directory
function(func_add_target_dir TGT DIR SRC DST)

    set( TGT_DIR  DIR.${DIR} )

    add_custom_target( ${TGT_DIR}  ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${SRC}/${DIR}
            ${DST}/${DIR}
        COMMENT "Added: \"${TGT_DIR}\""
    )
    add_dependencies( ${TGT}  ${TGT_DIR} )

endfunction()

# Sets a file as a custom target, then adds it as a dependency for the specified target
# @param TGT:  target which depends on the directory
# @param FILE: file to add as a custom target
# @param SRC:  source path of directory
# @param DST:  destination path of directory
function(func_add_target_file TGT FILE SRC DST)

    set( TGT_FILE  FILE.${FILE} )

    add_custom_target( ${TGT_FILE}  ALL
        COMMAND ${CMAKE_COMMAND} -E copy
            ${SRC}/${FILE}
            ${DST}/${FILE}
        COMMENT "Added: \"${TGT_FILE}\""
    )
    add_dependencies( ${TGT}  ${TGT_FILE} )

endfunction()
