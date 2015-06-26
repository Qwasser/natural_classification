# Top-level cmake list
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( scidi_wrapper )

#set( CMAKE_CXX_FLAGS "-O0 -g3 -DDEBUG -D_DEBUG -Wall -pedantic -Who-long-long -std=c++0x" CACHE STRING "Debug options" FORCE)
#set( CMAKE_CXX_FLAGS "-O2 -DNDEBUG -Wall -pedantic -Who-long-long -std=c++0x -rdynamic" CACHE STRING "Release options" FORCE)

add_subdirectory( wrapper )
add_subdirectory( tests )
add_subdirectory( dep )
