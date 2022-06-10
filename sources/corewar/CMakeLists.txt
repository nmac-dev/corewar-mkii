#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   CMAKE ---> ./SOURCE/COREWAR
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~SOURCE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_library ( ${SOURCE_OS}          #~~ OS
    ${__SRC}/${__OS}/assembly.cpp
    ${__SRC}/${__OS}/pcb.cpp
    ${__SRC}/${__OS}/scheduler.cpp
    ${__SRC}/${__OS}/ctrl_unit.cpp
    ${__SRC}/${__OS}/memory.cpp
    ${__SRC}/${__OS}/report.cpp
    ${__SRC}/${__OS}/cpu.cpp
    )
target_include_directories( ${SOURCE_OS}   PUBLIC  ${__HEADERS}  )

add_library ( ${SOURCE_COREWAR}     #~~ Corewar
    ${__SRC}/parser.cpp
    ${__SRC}/corewar.cpp
    )
target_link_libraries( ${SOURCE_COREWAR}    ${SOURCE_OS}  )

#~~EXECUTABLE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
target_link_libraries( ${PROJECT_NAME}    ${SOURCE_COREWAR}  )
