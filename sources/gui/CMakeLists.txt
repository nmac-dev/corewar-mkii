#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   CMAKE ---> ./SOURCES/GUI
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~GUI~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_library ( source.gui       #~~ Corewar GUI
    src/memory_viewer.gui.cpp
    src/control_panel_sections.gui.cpp
    src/control_panel.gui.cpp
    )
target_include_directories ( source.gui    PUBLIC  include  )
target_link_libraries      ( source.gui
    source.core

    ${VENDOR_IMGUI_BACKENDS}
    ${VENDOR_GLAD}
    ${VENDOR_GLFW}
    )

#~~EXECUTABLE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
target_link_libraries ( ${PROJECT_NAME}    source.gui  )
