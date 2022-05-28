#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   CMAKE ---> PROJECT/__VENDOR
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~VENDOR~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#~~GLAD
set( __GLAD     glad  )

add_library( ${__GLAD}      ${__GLAD}/${__SRC}/glad.c  )
target_include_directories( ${__GLAD}        PRIVATE  ${__GLAD}/${__HEADERS}  )

target_include_directories( ${PROJECT_NAME}  PRIVATE  ${__GLAD}/${__HEADERS}        )
target_link_libraries(      ${PROJECT_NAME}           ${__GLAD} ${CMAKE_DL_LIBS}    )

#~~GLFW
set( __GLFW     glfw  )

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs"  )
set(GLFW_BUILD_TESTS    OFF CACHE INTERNAL "Build the GLFW test programs"     )
set(GLFW_BUILD_DOCS     OFF CACHE INTERNAL "Build the GLFW documentation"     )
set(GLFW_INSTALL        OFF CACHE INTERNAL "Generate installation target"     )

add_subdirectory( ${__GLFW}  )

target_link_libraries(     ${PROJECT_NAME}  ${__GLFW}   ${GLFW_LIBRARIES}       )
target_include_directories(${PROJECT_NAME}  PRIVATE     ${__GLFW}/${__HEADERS}  )
target_compile_definitions(${PROJECT_NAME}  PRIVATE     "GLFW_INCLUDE_NONE"     )

#~~IMGUI
set( __IMGUI    imgui )

add_library( ${__IMGUI}
    ${__IMGUI}/imgui.cpp
    ${__IMGUI}/imgui_draw.cpp
    ${__IMGUI}/imgui_tables.cpp
    ${__IMGUI}/imgui_widgets.cpp
    )
target_include_directories( ${__IMGUI}       PRIVATE     ${__IMGUI}  )

target_include_directories( ${PROJECT_NAME}  PRIVATE     ${__IMGUI}         )
target_link_libraries(      ${PROJECT_NAME}  ${__IMGUI}  ${CMAKE_DL_LIBS}   )
