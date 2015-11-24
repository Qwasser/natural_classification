CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( wrapper )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pthread")

include_directories(${scidi_wrapper_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/dep/scidi/include)
include_directories(${CMAKE_SOURCE_DIR}/dep/sd/include)


file(GLOB_RECURSE SOURCES "*.h" "*.hpp" "*.cpp")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/dep/scidi/include/*.h")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/dep/sd/include/*.h")

set(wxWidgets_CONFIGURATION msu)
find_package(wxWidgets COMPONENTS core base xml REQUIRED)
set(wxWidgets_CONFIG_OPTIONS SHARED=1)
include(${wxWidgets_USE_FILE})

LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/dep/scidi/lib
                 ${CMAKE_SOURCE_DIR}/dep/sd/lib)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} scidi.a libdiscovery.a ${wxWidgets_LIBRARIES})

# Swig wrapper
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

INCLUDE_DIRECTORIES(${wrapper_SOURCE_DIR})

SET(CMAKE_SWIG_FLAGS "")

SET_SOURCE_FILES_PROPERTIES(scidi_wrapper.i PROPERTIES CPLUSPLUS ON)
SWIG_ADD_MODULE(scidi_wrapper python scidi_wrapper.i scidi_wrapper.cpp ruleparser.cpp sdrulegenerator.cpp MyClassificator.cpp CIdelObject.cpp idealizer.cpp)
SWIG_LINK_LIBRARIES(scidi_wrapper scidi.a libdiscovery.a ${PYTHON_LIBRARIES} ${wxWidgets_LIBRARIES})
