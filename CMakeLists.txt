cmake_minimum_required(VERSION 3.6)

enable_language(CXX)

include(ExternalProject)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/utils.cmake)

set(project_name "DonerECS")
set(ide_group "${project_name}")

file(GLOB_RECURSE header_files "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE source_files "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

add_source_groups("${header_files}")
add_source_groups("${source_files}")

include_directories ("include")

add_library("${project_name}" STATIC ${header_files} ${source_files})

set_target_properties("${project_name}" PROPERTIES LINKER_LANGUAGE CXX)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set_target_properties ("${project_name}" PROPERTIES FOLDER "${ide_group}")

if(NOT DEFINED MAX_ENTITIES)
	set(MAX_ENTITIES 4096)
endif()
target_compile_definitions("${project_name}" PUBLIC -DMAX_ENTITIES=${MAX_ENTITIES})

if(NOT DEFINED MAX_TAGS)	
	set(MAX_TAGS 64)
endif()
target_compile_definitions("${project_name}" PUBLIC -DMAX_TAGS=${MAX_TAGS})
		
warnings_as_errors("${project_name}")

if(DECS_ENABLE_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
	set(CMAKE_FIND_LIBRARY_PREFIXES "" "lib")
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".a" ".dll")
	find_package(GTest)
	
	if(GTEST_FOUND)
		set(tests_project_name "${project_name}_tests")
		project("${tests_project_name}")
		
		file(GLOB_RECURSE test_header_files "${CMAKE_CURRENT_SOURCE_DIR}/tests/include/*.h")
		file(GLOB_RECURSE test_source_files "${CMAKE_CURRENT_SOURCE_DIR}/tests/source/*.cpp")

		add_source_groups("${test_header_files}")
		add_source_groups("${test_source_files}")
		
		include_directories("${CMAKE_CURRENT_SOURCE_DIR}/tests/include")		
		include_directories("${GTEST_INCLUDE_DIRS}")
		add_executable ("${tests_project_name}" "${test_header_files}" "${test_source_files}")
		
		set_target_properties("${tests_project_name}" PROPERTIES LINKER_LANGUAGE CXX)
		set_target_properties ("${tests_project_name}" PROPERTIES FOLDER "${ide_group}/tests")
		
		target_link_libraries("${tests_project_name}" "${project_name}" "${GTEST_BOTH_LIBRARIES}")
		
		warnings_as_errors("${tests_project_name}")
	else()
		MESSAGE( STATUS "GTest is not installed in your system! Can't generate tests")
	endif()
endif()
