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

function(warnings_as_errors TARGET)
    get_target_property(CURRENT_COMPILE_FLAGS ${TARGET} COMPILE_FLAGS)
    if("${CURRENT_COMPILE_FLAGS}" STREQUAL "CURRENT_COMPILE_FLAGS-NOTFOUND")
        set (CURRENT_COMPILE_FLAGS "")
    endif()

	if (MSVC)
        set_target_properties (${TARGET} PROPERTIES COMPILE_FLAGS "${CURRENT_COMPILE_FLAGS} /WX")
		if(TARGET ${TARGET}_LIB)
            set_target_properties (${TARGET}_LIB PROPERTIES COMPILE_FLAGS "${CURRENT_COMPILE_FLAGS} /WX")
        endif()
    endif()
endfunction(warnings_as_errors)