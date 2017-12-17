cmake_minimum_required(VERSION 3.6)
set(DECS_MAJOR_VERSION 1)
set(DECS_MINOR_VERSION 0)
set(DECS_PATCH_VERSION 0)
set(DECS_VERSION "${DECS_MAJOR_VERSION}.${DECS_MINOR_VERSION}.${DECS_PATCH_VERSION}")

enable_language(CXX)

include(ExternalProject)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/utils.cmake)

set(project_name "DonerECS")
set(ide_group "${project_name}")

project("${project_name}")

message(STATUS "${project_name} version ${DECS_VERSION}")

if ((MAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

file(GLOB_RECURSE header_files "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE source_files "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

add_source_groups("${header_files}")
add_source_groups("${source_files}")

include_directories ("include")

# Library installation directory
if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR lib)
endif()
set(libdir ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

if (DECS_BUILD_SHARED)
	message(STATUS "-- Building shared libraries (-DDECS_BUILD_SHARED=0 to only build static libraries)")
	add_library("${project_name}" SHARED ${header_files} ${source_files})
	
	set_target_properties("${project_name}" PROPERTIES
		OUTPUT_NAME "${project_name}"
		DEBUG_POSTFIX -d
		VERSION ${DECS_VERSION}
		SOVERSION ${DECS_MAJOR_VERSION}
		FOLDER "${project_name}")
else()
	add_library("${project_name}" STATIC ${header_files} ${source_files})
	
	set_target_properties("${project_name}" PROPERTIES DEBUG_POSTFIX -d FOLDER "${project_name}")
endif()

set(install_libs "${project_name}")
set_property(TARGET "${project_name}" APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

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
	
	set(GTEST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/googletest/lib/${CMAKE_CONFIGURATION_TYPES}")
	set(GTEST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/googletest/include")
	
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
		MESSAGE( STATUS "GTest is not found in your system! Can't generate tests")
	endif()
endif()

if (NOT WINDOWS OR CYGWIN)
    set(donerECS_libs -ldonerECS)

    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/donerECS.pc
        DESTINATION "${libdir}/pkgconfig"
        )
endif()


install(
	DIRECTORY "${project_name}"
	DESTINATION "include"
	FILES_MATCHING PATTERN "*.h"
	)

install(
	TARGETS "${install_libs}"
	LIBRARY DESTINATION "${libdir}"
	ARCHIVE DESTINATION "${libdir}"
	RUNTIME DESTINATION "bin"
	)
	