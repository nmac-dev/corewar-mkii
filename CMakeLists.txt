#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   CMAKE ---> ./
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cmake_minimum_required (VERSION 3.20)

#~~PROJECT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
project (corewar-mkii)

include (CTest)    #~~ BUILD_TESTING = ON
# set ( BUILD_TESTING    OFF  )

#~~COMPILER~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set ( CMAKE_CXX_STANDARD       17  )
set ( CXX_STANDARD_REQUIRED    ON  )

#~~SETUP~~~~-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set ( SOURCE_OS           source.os           )             #~~ SOURCE Libraries
set ( SOURCE_COREWAR      source.corewar      )
set ( SOURCE_COREWAR_GUI  source.corewar.gui  )

set ( __SOURCES    sources           )
set ( __COREWAR    corewar           )
set ( __SRC        src               )
set ( __HEADERS    include           )
set ( __OS         OS                )
set ( __SRC_OS     ${__SRC}/${__OS}  )
set ( __GUI        gui               )

if (BUILD_TESTING)
    set ( TESTER    tester  )                               #~~ TESTER file
    set ( __TEST    test    )

    enable_testing ()
    message ("\n\t . . . Testing enabled . . . \n")
endif ()

set ( VENDOR_IMGUI             vendor.imgui           )     #~~ VENDOR Libraries
set ( VENDOR_IMGUI_BACKENDS    vendor.imgui.backends  )
set ( VENDOR_GLAD              vendor.glad            )
set ( VENDOR_GLFW              glfw                   ) # named by GLFW

set ( __VENDOR      vendor    )
set ( __IMGUI       imgui     )
set ( __BACKENDS    backends  )
set ( __GLAD        glad      )
set ( __GLFW        glfw      )

#~~EXECUTABLE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_executable( ${PROJECT_NAME}    sources/gui/src/corewar.gui.cpp  )

if (WIN32)
    set_target_properties( ${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE true  ) # create exe instance without console
endif (WIN32)

#~~SOURCES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_subdirectory( ${__SOURCES}  )

#~~RESOURCES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set ( __WARRIORS       warriors     )
set ( __COREWAR_INI    corewar.ini  )
set ( __GUI_INI        .gui.ini     )
set ( __DOCS           docs         )
set ( __README         README.md    )
set ( __LICENSE        LICENSE      )

add_custom_target ( ${__DOCS}    ALL                #~~ 'docs/'
    COMMAND ${CMAKE_COMMAND}    -E copy_directory
    ${PROJECT_SOURCE_DIR}/${__DOCS}
    ${CMAKE_BINARY_DIR}/${__DOCS}
    COMMENT "Added \"${__DOCS}/\""
    )

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

add_custom_target ( ${__GUI_INI}    ALL             #~~ '.gui.ini'
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

add_custom_target ( ${__LICENSE}    ALL              #~~ 'LICENSE'
    COMMAND ${CMAKE_COMMAND}    -E copy
    ${PROJECT_SOURCE_DIR}/${__LICENSE}
    ${CMAKE_BINARY_DIR}/${__LICENSE}
    COMMENT "Added \"${__LICENSE}\""
)

#~~DEPENDENCIES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_dependencies ( ${PROJECT_NAME}         ${__DOCS}         )        #~~ 'docs/'
add_dependencies ( ${PROJECT_NAME}         ${__WARRIORS}     )        #~~ 'warriors/'
add_dependencies ( ${PROJECT_NAME}         ${__COREWAR_INI}  )        #~~ 'corewar.ini'
add_dependencies ( ${SOURCE_COREWAR_GUI}   ${__GUI_INI}      )        #~~ '.gui.ini'
add_dependencies ( ${PROJECT_NAME}         ${__README}       )        #~~ 'README.md'
add_dependencies ( ${PROJECT_NAME}         ${__LICENSE}      )        #~~ 'LICENSE'

#~~INSTALL~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set ( DEST_INSTALL ${CMAKE_BINARY_DIR}/${PROJECT_NAME} )    # create portable program in build

install ( TARGETS    ${PROJECT_NAME}                                            DESTINATION  ${DEST_INSTALL}  )
install ( DIRECTORY  ${__WARRIORS}                                              DESTINATION  ${DEST_INSTALL}  )
install ( DIRECTORY  ${__DOCS}                                                  DESTINATION  ${DEST_INSTALL}  )
install ( FILES      ${__COREWAR_INI}  ${__GUI_INI}  ${__README}  ${__LICENSE}  DESTINATION  ${DEST_INSTALL}  )
