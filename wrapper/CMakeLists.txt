CMAKE_MINIMUM_REQUIRED( VERSION 2.8.12 )

set(PROJECT_NAME_STR wrapper)
project(${PROJECT_NAME_STR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pthread")
set(WRAPPER_INCLUDES ${PROJECT_SOURCE_DIR}/include)

include_directories(${WRAPPER_INCLUDES})
include_directories(${CMAKE_SOURCE_DIR}/dep/scidi/include)
include_directories(${CMAKE_SOURCE_DIR}/dep/sd/include)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB HEADER_FILES "${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME_STR}/*.h")

LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/dep/scidi/lib
                 ${CMAKE_SOURCE_DIR}/dep/sd/lib)

add_library(${PROJECT_NAME_STR} SHARED ${SOURCES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME_STR} scidi.a)

# Tests
enable_testing()
set(PROJECT_TEST_NAME ${PROJECT_NAME_STR}_test)
include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/test/*.cpp)
add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
add_dependencies(${PROJECT_TEST_NAME} googletest)

target_link_libraries(${PROJECT_TEST_NAME}
                                      wrapper
                                      ${GTEST_LIBS_DIR}/libgtest.a
                                      ${GTEST_LIBS_DIR}/libgtest_main.a
                                      )
add_test(test ${PROJECT_TEST_NAME})


FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(PythonLibs 2.7.9 EXACT REQUIRED)

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${WRAPPER_INCLUDES})

SET(CMAKE_SWIG_FLAGS "")

SET_SOURCE_FILES_PROPERTIES(swig/wrapper.i PROPERTIES CPLUSPLUS ON)
SWIG_ADD_MODULE(wrapper python swig/wrapper.i ${SOURCES})

SWIG_LINK_LIBRARIES(wrapper scidi.a ${PYTHON_LIBRARIES})
