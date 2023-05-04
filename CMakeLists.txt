#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   /
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~PROJECT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cmake_minimum_required(VERSION 3.20)

project(corewar-mkii)

set( CMAKE_CXX_STANDARD       17 )
set( CXX_STANDARD_REQUIRED    ON )

#~~SCRIPTS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
include(cmake/utils.cmake)

#~~TEST~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
include(CTest)    #~~ BUILD_TESTING = ON
# set( BUILD_TESTING    OFF  )

if(BUILD_TESTING)
    enable_testing()
    message("\n\t . . . Testing enabled . . . \n")
endif()

#~~EXECUTABLE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_executable( ${PROJECT_NAME}    sources/gui/src/corewar.gui.cpp  )

if(WIN32)
    # create exe instance without console
    set_target_properties( ${PROJECT_NAME}
        PROPERTIES
            WIN32_EXECUTABLE true
    )
endif(WIN32)

#~~SOURCES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_subdirectory(sources)

#~~RESOURCES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
func_add_target_dir( ${PROJECT_NAME}
    docs
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
    )

func_add_target_dir( ${PROJECT_NAME}
    warriors
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
    )

func_add_target_file( source.core
    core.ini
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
    )

func_add_target_file( source.gui
    gui.ini
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
    )

func_add_target_file( ${PROJECT_NAME}
    README.md
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
    )

func_add_target_file( ${PROJECT_NAME}
    LICENSE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
    )

#~~INSTALL~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set( DEST_INSTALL ${CMAKE_BINARY_DIR}/${PROJECT_NAME} )    # create portable program in build

install( TARGETS    ${PROJECT_NAME}  DESTINATION  ${DEST_INSTALL} )
install( DIRECTORY  DIR.warrior      DESTINATION  ${DEST_INSTALL} )
install( DIRECTORY  DIR.docs         DESTINATION  ${DEST_INSTALL} )
install( FILES
            FILE.core.ini
            FILE.gui.ini
            FILE.README.md
            FILE.LICENSE
        DESTINATION  ${DEST_INSTALL}
    )
