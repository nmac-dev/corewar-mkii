#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   CMAKE ---> ./SOURCES/TEST
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~TESTERS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
include_directories( include )

add_executable( tester-parser     src/tester-parser.cpp       )
add_executable( tester-scheduler  src/OS/tester-scheduler.cpp )
add_executable( tester-memory     src/OS/tester-memory.cpp    )
add_executable( tester-cpu        src/OS/tester-cpu.cpp       )

target_link_libraries( tester-parser     source.core )
target_link_libraries( tester-scheduler  source.os   )
target_link_libraries( tester-memory     source.os   )
target_link_libraries( tester-cpu        source.os   )

#~~TEST~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_test( test.parser     tester-parser    )
add_test( test.scheduler  tester-scheduler )
add_test( test.memory     tester-memory    )
add_test( test.cpu        tester-cpu       )

#~~RESOURCES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
func_add_target_dir( tester-parser
    tester-warriors
        ${CMAKE_SOURCE_DIR}/sources/test
        ${CMAKE_BINARY_DIR}/sources/test
    )
