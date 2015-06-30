CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( wrapper )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

file(GLOB_RECURSE SOURCES "*.h" "*.hpp" "*.cpp")

include_directories(${CMAKE_SOURCE_DIR}/dep/scidi/include)

LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/dep/scidi/lib)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_link_libraries(${PROJECT_NAME} scidi.a)
