#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   CMAKE ---> ./SOURCES/TEST
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~HEADERS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
include_directories ( ${__HEADERS}  )
link_libraries ( source.core  )

#~~SRC~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set ( TEST_PARSER       parser.${TESTER}     )
set ( TEST_SCHEDULER    scheduler.${TESTER}  )
set ( TEST_MEMORY       memory.${TESTER}     )
set ( TEST_CPU          cpu.${TESTER}        )

add_executable ( ${TEST_PARSER}       ${__SRC}/${TEST_PARSER}.cpp        )
add_executable ( ${TEST_SCHEDULER}    src/OS/${TEST_SCHEDULER}.cpp  )
add_executable ( ${TEST_MEMORY}       src/OS/${TEST_MEMORY}.cpp     )
add_executable ( ${TEST_CPU}          src/OS/${TEST_CPU}.cpp        )

#~~TEST~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_test ( ${TEST_PARSER}       ${TEST_PARSER}     )
add_test ( ${TEST_SCHEDULER}    ${TEST_SCHEDULER}  )
add_test ( ${TEST_MEMORY}       ${TEST_MEMORY}     )
add_test ( ${TEST_CPU}          ${TEST_CPU}        )

#~~RESOURCES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set ( TEST_WARRIORS    warriors.${TESTER}  )

add_custom_target ( ${TEST_WARRIORS}    ALL    #~~ 'warriors.tester'
    COMMAND ${CMAKE_COMMAND}    -E copy_directory
    ${PROJECT_SOURCE_DIR}/${__SOURCES}/${__TEST}/${TEST_WARRIORS}
    ${CMAKE_BINARY_DIR}/${__SOURCES}/${__TEST}/${TEST_WARRIORS}
    COMMENT "Added \"${__SOURCES}/${__TEST}/${TEST_WARRIORS}/\""
    )

#~~DEPENDENCIES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_dependencies ( ${TEST_PARSER}    ${TEST_WARRIORS}  )    #~~ parser.tester.cpp
