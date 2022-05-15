#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   CMAKE ---> __TEST
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~HEADERS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
include_directories ( ${PROJECT_SOURCE_DIR}/${__SRC_HEADERS}   )
include_directories ( ${PROJECT_SOURCE_DIR}/${__TEST_HEADERS}  )

link_libraries ( ${__SRC_COREWAR_ALL_CPP}  )

#~~SOURCE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set ( __TEST_PARSER       parser.tester     )
set ( __TEST_SCHEDULER    scheduler.tester  )
set ( __TEST_MEMORY       memory.tester     )
set ( __TEST_CPU          cpu.tester        )

add_executable ( ${__TEST_PARSER}       ${__SRC}/${__TEST_PARSER}.cpp        )
add_executable ( ${__TEST_SCHEDULER}    ${__SRC_OS}/${__TEST_SCHEDULER}.cpp  )
add_executable ( ${__TEST_MEMORY}       ${__SRC_OS}/${__TEST_MEMORY}.cpp     )
add_executable ( ${__TEST_CPU}          ${__SRC_OS}/${__TEST_CPU}.cpp        )

#~~TEST~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_test ( ${__TEST_PARSER}       ${__TEST_PARSER}     )
add_test ( ${__TEST_SCHEDULER}    ${__TEST_SCHEDULER}  )
add_test ( ${__TEST_MEMORY}       ${__TEST_MEMORY}     )
add_test ( ${__TEST_CPU}          ${__TEST_CPU}        )

#~~RESOURCES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set ( __TEST_WARRIORS    warriors.tester  )

add_custom_target ( ${__TEST_WARRIORS}    ALL    #~~ 'warriors.tester/'
    COMMAND ${CMAKE_COMMAND}    -E copy_directory
    ${PROJECT_SOURCE_DIR}/${__TEST}/${__TEST_WARRIORS}
    ${CMAKE_BINARY_DIR}/${__TEST}/${__TEST_WARRIORS}
    COMMENT "Copying \"${__TEST}/${__TEST_WARRIORS}/\""
    )

#~~DEPENDENCIES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_dependencies ( ${__TEST_PARSER}    ${__TEST_WARRIORS}  )    #~~ parser.tester.cpp
