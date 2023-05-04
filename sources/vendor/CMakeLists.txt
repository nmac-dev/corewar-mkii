#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   CMAKE ---> ./SOURCES/VENDOR
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~GLAD~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_library               ( vendor.glad  glad/src/glad.c      )
target_include_directories( vendor.glad  PUBLIC  glad/include )

#~~GLFW~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set( GLFW_BUILD_EXAMPLES OFF )  # set extra build options
set( GLFW_BUILD_TESTS    OFF )
set( GLFW_BUILD_DOCS     OFF )
set( GLFW_INSTALL        OFF )

add_subdirectory( glfw  )

#~~IMGUI~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
add_library( vendor.imgui
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    )
target_include_directories( vendor.imgui  PUBLIC  imgui )

#~~IMGUI BACKEND~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set( IMGUI_BACKEND_DIR  imgui/backends )

add_library( vendor.imgui.backends
    ${IMGUI_BACKEND_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.cpp
    )
target_include_directories( vendor.imgui.backends  PUBLIC        ${IMGUI_BACKEND_DIR} )
target_link_libraries     ( vendor.imgui.backends  vendor.imgui  glfw                 )

#~~EXECUTABLE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
target_link_libraries( ${PROJECT_NAME}  vendor.glad  ${CMAKE_DL_LIBS} )
target_link_libraries( ${PROJECT_NAME}  glfw                          )
target_link_libraries( ${PROJECT_NAME}  vendor.imgui                  )
target_link_libraries( ${PROJECT_NAME}  vendor.imgui.backends         )
