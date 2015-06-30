CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( tests )

include_directories(${CMAKE_SOURCE_DIR}/dep/scidi/include
		    ${CMAKE_SOURCE_DIR}/wrapper)

file(GLOB_RECURSE SRC_LIST "*.h" "*.hpp" "*.cpp")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/dep/scidi/include/*.h")

LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/dep/scidi/lib)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} scidi.a wrapper)
