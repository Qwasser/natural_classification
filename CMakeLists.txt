# Top-level cmake list
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( scidi_wrapper C CXX)

set( CMAKE_BUILD_TYPE Debug)
set( CMAKE_CXX_FLAGS "-O0 -g3 -DDEBUG -D_DEBUG -Wall -pedantic -std=c++11" CACHE STRING "Debug options" FORCE)
set( CMAKE_CXX_FLAGS "-O2 -DNDEBUG -Wall -pedantic -std=c++11 -rdynamic" CACHE STRING "Release options" FORCE)

enable_testing()

set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/ext)
add_subdirectory(${EXT_PROJECTS_DIR}/gtest)

add_subdirectory( dep )
add_subdirectory( led_data_generator )
add_subdirectory( wrapper )

