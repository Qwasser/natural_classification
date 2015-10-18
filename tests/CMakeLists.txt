CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( tests )

include_directories(${scidi_wrapper_SOURCE_DIR}
                    ${scidi_wrapper_SOURCE_DIR}/wrapper
                    ${scidi_wrapper_SOURCE_DIR}/dep/scidi/include
                    ${scidi_wrapper_SOURCE_DIR}/dep/sd/include)

file(GLOB_RECURSE SRC_LIST "*.h" "*.hpp" "*.cpp")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/dep/scidi/include/*.h")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/dep/sd/include/*.h")

LINK_DIRECTORIES(${scidi_wrapper_SOURCE_DIR}/dep/scidi/lib)
LINK_DIRECTORIES(${scidi_wrapper_SOURCE_DIR}/dep/sd/lib)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} scidi.a libdiscovery.a wrapper)
