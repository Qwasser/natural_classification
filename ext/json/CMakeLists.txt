cmake_minimum_required(VERSION 2.8.8)
project(gtest_builder C CXX)
include(ExternalProject)

ExternalProject_Add(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
# Disable install step
    INSTALL_COMMAND ""
)

# Specify include dir
ExternalProject_Get_Property(json source_dir)
set(JSON_INCLUDE_DIRS ${source_dir}/src PARENT_SCOPE)
