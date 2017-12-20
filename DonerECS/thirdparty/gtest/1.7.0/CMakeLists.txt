cmake_minimum_required(VERSION 3.6)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake/utils.cmake)

project("gtest")

file(GLOB_RECURSE header_files "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE source_files "${CMAKE_CURRENT_SOURCE_DIR}/source/*.*")

add_source_groups("${header_files}")
add_source_groups("${source_files}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library("gtest" STATIC ${header_files} ${source_files})
set_target_properties("gtest" PROPERTIES LINKER_LANGUAGE CXX)