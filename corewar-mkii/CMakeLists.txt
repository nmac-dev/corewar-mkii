#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   CMAKE ---> PROJECT
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cmake_minimum_required (VERSION 3.20)

#~~PROJECT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
project (corewar-mkii)

include (CTest)    #~~ BUILD_TESTING = TRUE
# set ( BUILD_TESTING    OFF  )

#~~COMPILER~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set ( CMAKE_CXX_STANDARD       17  )
set ( CXX_STANDARD_REQUIRED    ON  )

#~~SETUP~~~~-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set ( __OS_SOURCE             os.source           )    #~~ OS
set ( __COREWAR_SOURCE        corewar.source      )    #~~ COREWAR
set ( __COREWAR_GUI_SOURCE    corewar.gui.source  )    #~~ GUI

set ( __SRC        src               )      #~~ SOURCE
set ( __HEADERS    include           )
set ( __OS         OS                )
set ( __SRC_OS     ${__SRC}/${__OS}  )
set ( __GUI        gui               )

set ( __VENDOR      vendor    )             #~~ VENDOR
set ( __IMGUI       imgui     )
set ( __BACKENDS    backends  )
set ( __GLAD        glad      )
set ( __GLFW        glfw      )

set( __IMGUI_BACKEND_DIR  ${__VENDOR}/${__IMGUI}/${__BACKENDS}  )    #~~ GUI Backend (imgui)

#~~EXECUTABLE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
include_directories ( ${__HEADERS}  )
include_directories ( ${__IMGUI_BACKEND_DIR}  )
include_directories ( ${__GUI}/${__HEADERS}   )

add_executable( ${PROJECT_NAME}    # PROJECT EXE
        ${PROJECT_SOURCE_DIR}/${__IMGUI_BACKEND_DIR}/imgui_impl_glfw.cpp
        ${PROJECT_SOURCE_DIR}/${__IMGUI_BACKEND_DIR}/imgui_impl_opengl3.cpp

        ${__GUI}/${__SRC}/memory_viewer.${__GUI}.cpp
        ${__GUI}/${__SRC}/control_panel_sections.${__GUI}.cpp
        ${__GUI}/${__SRC}/control_panel.${__GUI}.cpp

        ${__GUI}/${__SRC}/corewar.${__GUI}.cpp
    )

#~~TEST-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
if (BUILD_TESTING)

    enable_testing ()
    message ("\n\t . . . Testing enabled . . . \n")

    set ( __TEST            test                  )
    set ( __TEST_HEADERS    ${__TEST}/test_suite  )

    add_subdirectory ( ${__TEST}  )

endif ()
#~~SOURCE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_subdirectory ( ${__SRC}  )

#~~RESOURCES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set ( __WARRIORS            warriors     )
set ( __COREWAR_INI         corewar.ini  )
set ( __GUI_INI             .gui.ini     )
set ( __DOCS                docs         )
set ( __README              README.md    )

add_custom_target ( ${__WARRIORS}    ALL            #~~ 'warriors/'
    COMMAND ${CMAKE_COMMAND}    -E copy_directory
    ${PROJECT_SOURCE_DIR}/${__WARRIORS}
    ${CMAKE_BINARY_DIR}/${__WARRIORS}
    COMMENT "Added \"${__WARRIORS}/\""
    )

add_custom_target ( ${__COREWAR_INI}    ALL         #~~ 'corewar.ini'
    COMMAND ${CMAKE_COMMAND}    -E copy
    ${PROJECT_SOURCE_DIR}/${__COREWAR_INI}
    ${CMAKE_BINARY_DIR}/${__COREWAR_INI}
    COMMENT "Added \"${__COREWAR_INI}\""
    )

add_custom_target ( ${__GUI_INI}    ALL             #~~ 'corewar.gui.ini'
    COMMAND ${CMAKE_COMMAND}    -E copy
    ${PROJECT_SOURCE_DIR}/${__GUI_INI}
    ${CMAKE_BINARY_DIR}/${__GUI_INI}
    COMMENT "Added \"${__GUI_INI}\""
    )

add_custom_target ( ${__README}    ALL              #~~ 'README.md'
    COMMAND ${CMAKE_COMMAND}    -E copy
    ${PROJECT_SOURCE_DIR}/${__README}
    ${CMAKE_BINARY_DIR}/${__README}
    COMMENT "Added \"${__README}\""
    )

add_custom_target ( ${__DOCS}    ALL                #~~ 'docs/'
    COMMAND ${CMAKE_COMMAND}    -E copy_directory
    ${PROJECT_SOURCE_DIR}/${__DOCS}
    ${CMAKE_BINARY_DIR}/${__DOCS}
    COMMENT "Added \"${__DOCS}/\""
    )

#~~DEPENDENCIES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_dependencies ( ${PROJECT_NAME}   ${__WARRIORS}     )        #~~ 'warriors/'
add_dependencies ( ${PROJECT_NAME}   ${__COREWAR_INI}  )        #~~ 'corewar.ini'
add_dependencies ( ${PROJECT_NAME}   ${__GUI_INI}      )        #~~ '.gui.ini'
add_dependencies ( ${PROJECT_NAME}   ${__README}       )        #~~ 'README.md'
add_dependencies ( ${PROJECT_NAME}   ${__DOCS}         )        #~~ 'docs/'

#~~VENDOR~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_subdirectory(${__VENDOR})

#~~BUILD-STEPS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
