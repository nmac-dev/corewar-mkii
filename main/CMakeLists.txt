# !/bin/cmake -S <src_dir> -B <build_dir>
# bash: 	  cmake -S ../../ -B .
# powershell: cmake -S ../../ -B . -G "MinGW Makefiles"

##########################
##    CMakeLists.txt    ##
##########################

## CMake
cmake_minimum_required (VERSION 3.10)

## Target
project(corewar-cli)

## Compiler
set(CMAKE_CXX_STANDARD 17)

set_target_properties(${PROJECT} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

## Headers
include_directories(${PROJECT_SOURCE_DIR} 
	inc/ 
	inc/template/
	)

## Sources
add_executable (${PROJECT} corewar-cli
	src/asm.cpp
	src/parser.cpp
	src/mars.cpp
	src/pcb.cpp
	src/scheduler.cpp
	src/core.cpp
	src/corewar-cli.cpp # only used to debug and developer
	)

## Resources
add_custom_target(warriors ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${PROJECT_SOURCE_DIR}/warriors
	${PROJECT_BINARY_DIR}/warriors
	COMMENT "Copying 'warriors/'"
	)
	
configure_file(
	${PROJECT_SOURCE_DIR}/corewar.ini 
	${PROJECT_BINARY_DIR}/corewar.ini 
	COPYONLY
	)

add_dependencies(corewar-cli warriors)

## Automated fetch: source files (not recommended)

# file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
#
# add_executable (${PROJECT} corewar-cli 
#	${SOURCE_FILES}
#	)
