#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   CMAKE ---> PROJECT/__SRC
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~HEADERS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
include_directories ( ${PROJECT_SOURCE_DIR}/${__HEADERS}  )

#~~SOURCE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#~~ OS
add_library ( ${__OS_SOURCE}
    ${__OS}/assembly.cpp
    ${__OS}/pcb.cpp
    ${__OS}/scheduler.cpp
    ${__OS}/ctrl_unit.cpp
    ${__OS}/memory.cpp
    ${__OS}/report.cpp
    ${__OS}/cpu.cpp
    )

#~~ Corewar
add_library ( ${__COREWAR_SOURCE}
    parser.cpp
    corewar.cpp
    )

target_link_libraries( ${__COREWAR_SOURCE}    ${__OS_SOURCE}  )

#~~EXECUTABLE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
target_link_libraries( ${PROJECT_NAME}    ${__COREWAR_SOURCE}  )
