#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   CMAKE ---> ./SOURCES/GUI
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~GUI~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_library ( ${SOURCE_COREWAR_GUI}       #~~ Corewar GUI
    ${__SRC}/memory_viewer.${__GUI}.cpp
    ${__SRC}/control_panel_sections.${__GUI}.cpp
    ${__SRC}/control_panel.${__GUI}.cpp
    )
target_include_directories ( ${SOURCE_COREWAR_GUI}    PUBLIC  ${__HEADERS}  )
target_link_libraries      ( ${SOURCE_COREWAR_GUI}
    ${SOURCE_COREWAR}

    ${VENDOR_IMGUI_BACKENDS}
    ${VENDOR_GLAD}
    ${VENDOR_GLFW}
    )

#~~EXECUTABLE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
target_link_libraries ( ${PROJECT_NAME}    ${SOURCE_COREWAR_GUI}  )
