if(NOT RADIANT_BUILD_TOOLS)
	return()
endif()

function(radiant_add_tool name)
	cmake_parse_arguments(PARSE_ARGV 1 ARG "WIN32" "" "SOURCES;INCLUDE_DIRECTORIES;DEPENDENCIES;COMPILE_DEFINITIONS;COMPILE_OPTIONS")
	set(target "wiferadiant-tool-${name}")
	if(ARG_WIN32)
		add_executable(${target} WIN32 ${ARG_SOURCES})
	else()
		add_executable(${target} ${ARG_SOURCES})
	endif()
	radiant_add_common(${target})
	target_link_libraries(${target}
		PRIVATE
			${ARG_DEPENDENCIES}
	)
	target_compile_definitions(${target}
		PRIVATE
			${ARG_COMPILE_DEFINITIONS}
	)
	target_compile_options(${target}
		PRIVATE
			${ARG_COMPILE_OPTIONS}
	)
	target_include_directories(${target}
		PRIVATE
			${ARG_INCLUDE_DIRECTORIES}
	)
	set_target_properties(${target}
		PROPERTIES
			OUTPUT_NAME ${name}
			LIBRARY_OUTPUT_DIRECTORY $<1:${RADIANT_INSTALL_PREFIX}>
			RUNTIME_OUTPUT_DIRECTORY $<1:${RADIANT_INSTALL_PREFIX}>
	)
endfunction()

file(GLOB tools "${PROJECT_SOURCE_DIR}/cmake/tools/*.cmake")
foreach(tool IN LISTS tools)
	include(${tool})
endforeach()
