if(NOT RADIANT_GENERATE_DOCS)
	return()
endif()

find_program(PANDOC_EXECUTABLE pandoc)

if(NOT PANDOC_EXECUTABLE)
	message(WARNING "Failed to find pandoc, cannot build docs")
	return()
endif()

function(radiant_add_docs name)
	cmake_parse_arguments(PARSE_ARGV 1 ARG "" "ROOT_PAGE;INPUT_PATH;OUTPUT_PATH;CSS_FILE" "")
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
	if(ARG_ROOT_PAGE)
		cmake_path(GET ARG_ROOT_PAGE STEM root_page_stem)
		set(html_file "${ARG_OUTPUT_PATH}/${root_page_stem}.html")
		add_custom_command(
			OUTPUT ${html_file}
			COMMAND ${CMAKE_COMMAND} -E rm -r ${ARG_OUTPUT_PATH}
			COMMAND ${PANDOC_EXECUTABLE} ARGS --lua-filter="${PROJECT_SOURCE_DIR}/cmake/include-files.lua" --toc --toc-depth 2 -f markdown-smart -t chunkedhtml ${CSS_FILE} -s -o ${ARG_OUTPUT_PATH} ${ARG_ROOT_PAGE}
			COMMAND ${CMAKE_COMMAND} -E copy -t ${ARG_OUTPUT_PATH} ${dependency_files}
			WORKING_DIRECTORY ${ARG_INPUT_PATH}
		)
		add_custom_target(${name}-${root_page_stem} DEPENDS ${html_file})
		add_dependencies(${name} ${name}-${root_page_stem})
	else()
		foreach(markdown_file IN LISTS markdown_files)
			cmake_path(GET markdown_file STEM markdown_file_stem)
			set(html_file "${ARG_OUTPUT_PATH}/${markdown_file_stem}.html")
			add_custom_command(
				OUTPUT ${html_file}
				COMMAND ${PANDOC_EXECUTABLE} ARGS --toc --toc-depth 2 -f markdown-smart -t html ${CSS_FILE} -s -o ${html_file} ${markdown_file}
				COMMAND ${CMAKE_COMMAND} -E copy -t ${ARG_OUTPUT_PATH} ${dependency_files}
			)
			add_custom_target(${name}-${markdown_file_stem} DEPENDS ${html_file})
			add_dependencies(${name} ${name}-${markdown_file_stem})
		endforeach()
	endif()
endfunction()

# wrmap shader manual
radiant_add_docs(wrmap-shader-manual
	ROOT_PAGE ${PROJECT_SOURCE_DIR}/docs/wrmap-shader-manual/index.md
	INPUT_PATH ${PROJECT_SOURCE_DIR}/docs/wrmap-shader-manual
	OUTPUT_PATH ${RADIANT_INSTALL_PREFIX}/docs/wrmap-shader-manual
	CSS_FILE "default.css"
)
