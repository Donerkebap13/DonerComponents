cmake_minimum_required(VERSION 3.6)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake/utils.cmake)

set(project_name "gtest")

project("${project_name}")

file(GLOB_RECURSE header_files "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE source_files "${CMAKE_CURRENT_SOURCE_DIR}/source/*.*")

add_source_groups("${header_files}")
add_source_groups("${source_files}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_HAS_PTHREAD=0 -DGTEST_HAS_TR1_TUPLE=0 -D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING")

add_library("${project_name}" STATIC ${header_files} ${source_files})
set_target_properties("${project_name}" PROPERTIES LINKER_LANGUAGE CXX)
