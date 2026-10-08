
# system suffixes

if(EMSCRIPTEN)
	set(RADIANT_EXECUTABLE_SUFFIX "wasm")
	set(RADIANT_LIBRARY_SUFFIX "wasm")
elseif(CMAKE_SYSTEM_PROCESSOR)
	string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} _system_processor)
	if(_system_processor STREQUAL "amd64" OR _system_processor STREQUAL "x64")
		set(_system_processor "x86_64")
	endif()
	set(RADIANT_EXECUTABLE_SUFFIX ${_system_processor}${CMAKE_EXECUTABLE_SUFFIX})
	set(RADIANT_LIBRARY_SUFFIX ${_system_processor}${CMAKE_SHARED_LIBRARY_SUFFIX})
else()
	message(FATAL_ERROR "can't determine system processor (no CMAKE_SYSTEM_PROCESSOR?)")
endif()

# compiler options

set(RADIANT_COMMON_OPTIONS
	$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU,Clang>>:-Wreorder>
	$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU,Clang>>:-fno-rtti>
	$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU,Clang>>:-fpermissive>
	$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<C_COMPILER_ID:GNU,Clang>>:-W>
	$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<C_COMPILER_ID:GNU,Clang>>:-Wall>
	$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<C_COMPILER_ID:GNU,Clang>>:-Wcast-align>
	$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<C_COMPILER_ID:GNU,Clang>>:-Wcast-qual>
	$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<C_COMPILER_ID:GNU,Clang>>:-Wno-unused-parameter>
	$<$<AND:$<COMPILE_LANGUAGE:C,CXX>,$<C_COMPILER_ID:GNU,Clang>>:-Wno-unused-function>
	$<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU,Clang>>:-fno-strict-aliasing>
	$<$<CXX_COMPILER_ID:MSVC>:/wd4267> # warning C4267: '=': conversion from 'size_t' to 'unsigned long', possible loss of data
)

# compile definitions

set(RADIANT_COMMON_DEFINITIONS
	QT_NO_KEYWORDS
	RADIANT_VERSION=\"${RADIANT_VERSION}\"
	RADIANT_VERSION_MAJOR=\"${RADIANT_VERSION_MAJOR}\"
	RADIANT_VERSION_MINOR=\"${RADIANT_VERSION_MINOR}\"
	RADIANT_VERSION_PATCH=\"${RADIANT_VERSION_PATCH}\"
	RADIANT_GIT_REVISION=\"${RADIANT_GIT_REVISION}\"
	RADIANT_GIT_DATE=\"${RADIANT_GIT_DATE}\"
	RADIANT_GIT_BRANCH=\"${RADIANT_GIT_BRANCH}\"
	RADIANT_ABOUTMSG=\"${RADIANT_ABOUTMSG}\"
	$<$<CONFIG:Debug>:_DEBUG>
	$<$<NOT:$<BOOL:${WIN32}>>:POSIX>
	$<$<BOOL:${WIN32}>:WIN32>
	RADIANT_EXECUTABLE_SUFFIX=\"${RADIANT_EXECUTABLE_SUFFIX}\"
	RADIANT_LIBRARY_SUFFIX=\"${RADIANT_LIBRARY_SUFFIX}\"
	WRMAP_VERSION=\"${WRMAP_VERSION}\"
	WRMAP_MOTD=\"${WRMAP_MOTD}\"
	Q3MAP_VERSION=\"${Q3MAP_VERSION}\"
	$<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
	$<$<BOOL:${RADIANT_USE_GLES2}>:RADIANT_USE_GLES2=1>
)

# helper function to add common options and definitions

function(radiant_add_common target)
	target_compile_options(${target} PRIVATE ${RADIANT_COMMON_OPTIONS})
	target_compile_definitions(${target} PRIVATE ${RADIANT_COMMON_DEFINITIONS})
	get_target_property(_target_type ${target} TYPE)
	if(_target_type STREQUAL "EXECUTABLE")
		set_target_properties(${target}
			PROPERTIES
				SUFFIX ".${RADIANT_EXECUTABLE_SUFFIX}"
		)
		if(WIN32)
			set_property(TARGET ${target} PROPERTY RUNTIME_DLL_DIRS $<TARGET_RUNTIME_DLL_DIRS:${target}>)
			set_property(TARGET ${target} PROPERTY FILE $<TARGET_FILE:${target}>)
			set_property(TARGET ${target} PROPERTY FILE_DIR $<TARGET_FILE_DIR:${target}>)
			file(GENERATE
				OUTPUT "${PROJECT_BINARY_DIR}/${target}_install.cmake"
				CONTENT [[
file(GET_RUNTIME_DEPENDENCIES
	RESOLVED_DEPENDENCIES_VAR _resolved_deps
	UNRESOLVED_DEPENDENCIES_VAR _unresolved_deps
	EXECUTABLES
		$<GENEX_EVAL:$<TARGET_PROPERTY:FILE>>
	PRE_EXCLUDE_REGEXES
		"api-ms-" "ext-ms-" "Qt6"
	POST_EXCLUDE_REGEXES
		".*system32/.*\\.dll"
	DIRECTORIES
		$<GENEX_EVAL:$<TARGET_PROPERTY:RUNTIME_DLL_DIRS>>
)
if(_unresolved_deps)
	message(WARNING "$<GENEX_EVAL:$<TARGET_PROPERTY:NAME>> unresolved dependencies: ${_unresolved_deps}")
endif()
file(COPY ${_resolved_deps} DESTINATION $<GENEX_EVAL:$<TARGET_PROPERTY:FILE_DIR>>)
]]
				TARGET ${target}
			)
			install(SCRIPT "${PROJECT_BINARY_DIR}/${target}_install.cmake")
		endif()
		if(EMSCRIPTEN)
			target_compile_options(${target} PRIVATE -sMAIN_MODULE=1)
		endif()
	elseif(_target_type STREQUAL "SHARED_LIBRARY")
		set_target_properties(${target}
			PROPERTIES
				SUFFIX ".${RADIANT_LIBRARY_SUFFIX}"
				PREFIX ""
		)
		if(EMSCRIPTEN)
			target_compile_options(${target} PRIVATE -sSIDE_MODULE=2 -nostdlib)
			target_link_options(${target} PRIVATE -sSIDE_MODULE=2 -sSTANDALONE_WASM=1 -sERROR_ON_UNDEFINED_SYMBOLS=0 -nostdlib)
		endif()
	endif()
endfunction()
