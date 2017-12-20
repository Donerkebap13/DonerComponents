cmake_minimum_required(VERSION 3.6)

function(add_source_groups files)
	if(files)
		foreach(file ${files})
			add_file_to_source_group(${file})
		endforeach()
	endif()
endfunction(add_source_groups)

function(add_file_to_source_group file)
    get_filename_component (file_path ${file} PATH)
    file (RELATIVE_PATH rel_file_path ${CMAKE_CURRENT_LIST_DIR} ${file_path})
	if(NOT "${rel_file_path}" STREQUAL "")
		string (REPLACE "/" "\\" rel_file_path ${rel_file_path})
	endif()
	source_group ("${rel_file_path}" FILES ${file})
endfunction(add_file_to_source_group)

function(set_compile_flags TARGET)
    get_target_property(CURRENT_COMPILE_FLAGS ${TARGET} COMPILE_FLAGS)
    if("${CURRENT_COMPILE_FLAGS}" STREQUAL "CURRENT_COMPILE_FLAGS-NOTFOUND")
        set (CURRENT_COMPILE_FLAGS "")
    endif()
	get_target_property(CURRENT_COMPILE_FLAGS_DEBUG ${TARGET} COMPILE_FLAGS_DEBUG)
    if("${CURRENT_COMPILE_FLAGS_DEBUG}" STREQUAL "CURRENT_COMPILE_FLAGS-NOTFOUND")
        set (CURRENT_COMPILE_FLAGS_DEBUG "")
    endif()
	get_target_property(CURRENT_COMPILE_FLAGS_RELEASE ${TARGET} COMPILE_FLAGS_RELEASE)
    if("${CURRENT_COMPILE_FLAGS_RELEASE}" STREQUAL "CURRENT_COMPILE_FLAGS-NOTFOUND")
        set (CURRENT_COMPILE_FLAGS_RELEASE "")
    endif()
	
	if (CMAKE_CXX_COMPILER_ID MATCHES "(GNU|.*Clang)")
		set(CURRENT_COMPILE_FLAGS "${CURRENT_COMPILE_FLAGS} -pedantic -Werror -Wall -Wextra -Wno-unused-parameter -Wno-error=unused-variable -Wno-error=sign-compare")
		set(CURRENT_COMPILE_FLAGS_DEBUG "${CURRENT_COMPILE_FLAGS_DEBUG} -O0 -g")
		set(CURRENT_COMPILE_FLAGS_RELEASE "${CURRENT_COMPILE_FLAGS_RELEASE} -g -O2 -DNDEBUG")
		
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL 'MSVC')
		# /Zi - Produces a program database (PDB) that contains type information and symbolic debugging information for use with the debugger.
		# /FS - Allows multiple cl.exe processes to write to the same .pdb file
		# /DEBUG - Enable debug during linking
		# /Od - Disables optimization
		# /WX - Warnings as errors
		set(CURRENT_COMPILE_FLAGS "${CURRENT_COMPILE_FLAGS} /WX")
		set(CURRENT_COMPILE_FLAGS_DEBUG "${CURRENT_COMPILE_FLAGS_DEBUG} /Zi /FS /DEBUG /Od /MDd")
		# /Ox - Full optimization
		set(CURRENT_COMPILE_FLAGS_RELEASE "${CURRENT_COMPILE_FLAGS_RELEASE} /Ox -DNDEBUG")
	endif()
	
	set_target_properties (${TARGET} PROPERTIES COMPILE_FLAGS "${CURRENT_COMPILE_FLAGS}")
	set_target_properties (${TARGET} PROPERTIES COMPILE_FLAGS_DEBUG "${CURRENT_COMPILE_FLAGS_DEBUG}")
	set_target_properties (${TARGET} PROPERTIES COMPILE_FLAGS_RELEASE "${CURRENT_COMPILE_FLAGS_RELEASE}")
endfunction(set_compile_flags)
