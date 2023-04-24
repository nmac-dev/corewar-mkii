#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   CMAKE ---> ./SOURCES/VENDOR
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set( BUILD_SHARED_LIBS    OFF  ) # build static vendor libraries

#~~GLAD~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_library                ( ${VENDOR_GLAD}    ${__GLAD}/src/glad.c       )
target_include_directories ( ${VENDOR_GLAD}    PUBLIC  ${__GLAD}/include  )

#~~GLFW~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set( GLFW_HEADERS_DIR    ${__GLFW}/${__HEADERS}  )

set( GLFW_BUILD_EXAMPLES    OFF  )  # set extra build options
set( GLFW_BUILD_TESTS       OFF  )
set( GLFW_BUILD_DOCS        OFF  )
set( GLFW_INSTALL           OFF  )

add_subdirectory ( ${__GLFW}  )

target_include_directories ( ${VENDOR_GLFW}    PUBLIC  ${GLFW_HEADERS_DIR}  )

#~~IMGUI~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_library ( ${VENDOR_IMGUI}
    ${__IMGUI}/imgui.cpp
    ${__IMGUI}/imgui_draw.cpp
    ${__IMGUI}/imgui_tables.cpp
    ${__IMGUI}/imgui_widgets.cpp
    )
target_include_directories ( ${VENDOR_IMGUI}    PUBLIC  ${__IMGUI}  )

#~~IMGUI BACKEND~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set( IMGUI_BACKEND_DIR    ${__IMGUI}/${__BACKENDS}  )

add_library ( ${VENDOR_IMGUI_BACKENDS}
    ${IMGUI_BACKEND_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.cpp
    )
target_include_directories ( ${VENDOR_IMGUI_BACKENDS}    PUBLIC  ${IMGUI_BACKEND_DIR}     )
target_link_libraries      ( ${VENDOR_IMGUI_BACKENDS}    ${VENDOR_IMGUI}  ${VENDOR_GLFW}  )

#~~EXECUTABLE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
target_link_libraries      ( ${PROJECT_NAME}    ${VENDOR_GLAD}  ${CMAKE_DL_LIBS}  )
target_link_libraries      ( ${PROJECT_NAME}    ${VENDOR_GLFW}                    )
target_link_libraries      ( ${PROJECT_NAME}    ${VENDOR_IMGUI}                   )
target_link_libraries      ( ${PROJECT_NAME}    ${VENDOR_IMGUI_BACKENDS}          )
