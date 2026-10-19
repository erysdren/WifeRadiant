if(NOT RADIANT_GENERATE_DOCS)
	return()
endif()

find_program(PANDOC_EXECUTABLE pandoc)

if(NOT PANDOC_EXECUTABLE)
	message(WARNING "Failed to find pandoc, cannot build docs")
	return()
endif()

function(radiant_add_docs name)
	cmake_parse_arguments(PARSE_ARGV 1 ARG "" "INPUT_PATH;OUTPUT_PATH;CSS_FILE" "")
	if(ARG_CSS_FILE)
		set(CSS_FILE -c ${ARG_CSS_FILE})
	endif()
	add_custom_target(${name} ALL)
	file(GLOB markdown_files LIST_DIRECTORIES FALSE
		"${ARG_INPUT_PATH}/*.md"
	)
	file(GLOB dependency_files LIST_DIRECTORIES FALSE
		"${ARG_INPUT_PATH}/*.css"
		"${ARG_INPUT_PATH}/*.png"
		"${ARG_INPUT_PATH}/*.jpg"
	)
	if(dependency_files)
		file(COPY ${dependency_files} DESTINATION ${ARG_OUTPUT_PATH})
	endif()
	foreach(markdown_file IN LISTS markdown_files)
		cmake_path(GET markdown_file STEM markdown_file_stem)
		set(html_file "${ARG_OUTPUT_PATH}/${markdown_file_stem}.html")
		add_custom_command(
			OUTPUT ${html_file}
			COMMAND ${PANDOC_EXECUTABLE} ARGS -f markdown-smart -t html ${CSS_FILE} -s -o ${html_file} ${markdown_file}
		)
		add_custom_target(${name}-${markdown_file_stem} DEPENDS ${html_file})
		add_dependencies(${name} ${name}-${markdown_file_stem})
	endforeach()
endfunction()

# wrmap shader manual
radiant_add_docs(wrmap-shader-manual
	INPUT_PATH ${PROJECT_SOURCE_DIR}/docs/wrmap-shader-manual
	OUTPUT_PATH ${RADIANT_INSTALL_PREFIX}/docs/wrmap-shader-manual
	CSS_FILE "default.css"
)
