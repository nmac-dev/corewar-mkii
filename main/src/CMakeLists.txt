#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   CMAKE ---> __SRC
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~HEADERS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
include_directories ( ${PROJECT_SOURCE_DIR}/${__HEADERS}  )

#~~SOURCE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_library ( ${__COREWAR_SOURCE}
    parser.cpp
    corewar.cpp
    ${__OS}/assembly.cpp
    ${__OS}/pcb.cpp
    ${__OS}/scheduler.cpp
    ${__OS}/ctrl_unit.cpp
    ${__OS}/memory.cpp
    ${__OS}/report.cpp
    ${__OS}/cpu.cpp
    )

link_libraries ( ${__COREWAR_SOURCE}  )

#~~EXECUTABLE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set ( EXECUTABLE_OUTPUT_PATH    ${CMAKE_BINARY_DIR}  )

add_executable ( ${PROJECT_NAME}    cli_debug.cpp  )
