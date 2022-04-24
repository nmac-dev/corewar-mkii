# !/bin/cmake -S <src_dir> -B <build_dir>
# windows: cmake -S <src_dir> -B <build_dir> -G "MinGW Makefiles"
cmake_minimum_required (VERSION 3.10)

# Target
project(corewar-cli)

# Compiler
if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

# Headers
include_directories(${PROJECT_SOURCE_DIR} 
	inc/ 
	inc/template/
	)

# Sources
add_executable (${PROJECT} corewar-cli
	src/asm.cpp
	src/parser.cpp
	src/mars.cpp
	src/pcb.cpp
	src/scheduler.cpp
	src/core.cpp
	src/corewar-cli.cpp
	)

# Resources
add_custom_target(warriors_dir ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${PROJECT_SOURCE_DIR}/warriors/
	${PROJECT_BINARY_DIR}/warriors/
	COMMENT "Copying 'warriors/' to binary directory"
	)
	
configure_file(
	${PROJECT_SOURCE_DIR}/corewar.ini 
	${PROJECT_BINARY_DIR}/corewar.ini 
	COPYONLY
	)

add_dependencies(corewar-cli warriors_dir)

# file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
# add_executable (${PROJECT} corewar-cli ${SOURCE_FILES})
