#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   CMAKE ---> PROJECT
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cmake_minimum_required (VERSION 3.20)

#~~PROJECT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
project (corewar-cli)

include (CTest) #~~ BUILD_TESTING = TRUE

#~~COMPILER~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set ( CMAKE_CXX_STANDARD       17  )
set ( CXX_STANDARD_REQUIRED    ON  )

#~~CONSTANTS-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set ( __HEADERS        include           )
set ( __SRC            src               )
set ( __OS             OS                )
set ( __SRC_OS         ${__SRC}/${__OS}  )

set ( __COREWAR_SOURCE    __corewar_source  )

#~~TEST-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
if (BUILD_TESTING)

    enable_testing ()
    message ("\n\t . . . Testing enabled . . . \n")

    set ( __TEST            test                  )
    set ( __TEST_HEADERS    ${__TEST}/test_suite  )

    add_subdirectory ( ${__TEST}  )

endif ()
#~~SOURCE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_subdirectory ( ${__SRC}  )

#~~RESOURCES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set ( __WARRIORS       warriors     )
set ( __COREWAR_INI    corewar.ini  )

add_custom_target ( ${__WARRIORS}    ALL    #~~ 'warriors/'
    COMMAND ${CMAKE_COMMAND}    -E copy_directory
    ${PROJECT_SOURCE_DIR}/${__WARRIORS}
    ${CMAKE_BINARY_DIR}/${__WARRIORS}
    COMMENT "Copying \"${__WARRIORS}/\""
    )

add_custom_target ( ${__COREWAR_INI}    ALL    #~~ 'corewar.ini'
    COMMAND ${CMAKE_COMMAND}    -E copy
    ${PROJECT_SOURCE_DIR}/${__COREWAR_INI}
    ${CMAKE_BINARY_DIR}/${__COREWAR_INI}
    COMMENT "Copying \"${__COREWAR_INI}\""
    )

#~~DEPENDENCIES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_dependencies ( ${PROJECT_NAME}   ${__WARRIORS}     )    #~~ 'warriors/'
add_dependencies ( ${PROJECT_NAME}   ${__COREWAR_INI}  )    #~~ 'corewar.ini'

#~~VENDORS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#~~BUILD-STEPS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# $: = command line

#~~ 1
#       $: cd <build_dir>

#~~ 2:
#       {on linux}      $: cmake -S ../../ -B .
#       {on windows}    $: cmake -S ../../ -B . -G "MinGW Makefiles"
#       -S = <src_dir> 
#       -B = <build_dir>

#~~ 3:
#       $: make

#~~ 3.5: if  ( testing    TRUE  )
#       $: ctest
#       outputs test results

#~~ 4
#       Run executable, <exe_name> = ${PROJECT_NAME}
#       $: ./<exe_name>
