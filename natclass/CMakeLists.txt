cmake_minimum_required( VERSION 2.8.12 )

set(PROJECT_NAME_STR natclass)
project(${PROJECT_NAME_STR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pthread -std=c++11")
set(NATCLASS_INCLUDES ${PROJECT_SOURCE_DIR}/include)

include_directories(${NATCLASS_INCLUDES} ${JSON_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB HEADER_FILES "${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME_STR}/*.h")

add_library(${PROJECT_NAME_STR} SHARED ${SOURCES} ${HEADER_FILES})

# Tests
enable_testing()
set(PROJECT_TEST_NAME ${PROJECT_NAME_STR}_test)
include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)
add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
add_dependencies(${PROJECT_TEST_NAME} googletest)

target_link_libraries(${PROJECT_TEST_NAME}
                      ${PROJECT_NAME_STR}
                      ${GTEST_LIBS_DIR}/libgtest.a
                      ${GTEST_LIBS_DIR}/libgtest_main.a)

add_test(test ${PROJECT_TEST_NAME})
