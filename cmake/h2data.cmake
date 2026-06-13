
# h2data

add_executable(h2data
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/common/bspfile.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/common/cmdlib.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/common/inout.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/common/l3dslib.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/common/lbmlib.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/common/mathlib.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/common/md4.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/common/path_init.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/common/qfiles.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/common/scriplib.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/common/threads.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/common/token.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/common/trilib.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/qcommon/reference.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/qcommon/resourcemanager.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/qcommon/skeletons.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/animcomp.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/book.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/fmodels.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/images.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/jointed.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/models.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/pics.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/qdata.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/qd_skeletons.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/sprites.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/svdcmp.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/tables.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/tmix.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/video.c
)
target_link_libraries(h2data PRIVATE l_net)
target_link_libraries(h2data PRIVATE LibXml2::LibXml2)
target_include_directories(h2data PRIVATE
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/common
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2/qcommon
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata_heretic2
	${PROJECT_SOURCE_DIR}/tools/quake2/common
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/libs
)
target_compile_definitions(h2data PRIVATE
	RADIANT_VERSION=$<QUOTE>${RADIANT_VERSION}$<QUOTE>
	RADIANT_MAJOR_VERSION=$<QUOTE>${RADIANT_MAJOR_VERSION}$<QUOTE>
	RADIANT_MINOR_VERSION=$<QUOTE>${RADIANT_MINOR_VERSION}$<QUOTE>
	RADIANT_PATCH_VERSION=$<QUOTE>${RADIANT_PATCH_VERSION}$<QUOTE>
	RADIANT_ABOUTMSG=$<QUOTE>${RADIANT_ABOUTMSG}$<QUOTE>
)
set_target_properties(h2data
	PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/install
		RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/install
)
target_compile_options(h2data PRIVATE
	$<$<BOOL:${MINGW}>:-static>
	$<$<BOOL:${MINGW}>:-static-libgcc>
	$<$<BOOL:${MINGW}>:-static-libstdc++>
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
)

if(WIN32)
	install(CODE [[
		file(GET_RUNTIME_DEPENDENCIES
			RESOLVED_DEPENDENCIES_VAR _resolved_deps
			UNRESOLVED_DEPENDENCIES_VAR _unresolved_deps
			EXECUTABLES
				$<TARGET_FILE:h2data>
			PRE_EXCLUDE_REGEXES
				"api-ms-" "ext-ms-" "Qt6"
			POST_EXCLUDE_REGEXES
				".*system32/.*\\.dll"
			DIRECTORIES
				$<TARGET_RUNTIME_DLL_DIRS:h2data>
		)
		if(_unresolved_deps)
			message(WARNING "h2data unresolved dependencies: ${_unresolved_deps}")
		endif()
		file(COPY ${_resolved_deps} DESTINATION $<TARGET_FILE_DIR:h2data>)
	]])
endif()

target_compile_definitions(h2data PRIVATE $<$<CONFIG:Debug>:_DEBUG> $<$<NOT:$<BOOL:${WIN32}>>:POSIX> $<$<BOOL:${WIN32}>:WIN32>)
if(WIN32)
	target_compile_definitions(h2data PRIVATE RADIANT_EXECUTABLE=$<QUOTE>exe$<QUOTE>)
elseif(DEFINED CMAKE_SYSTEM_PROCESSOR)
	string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} SYSTEM_PROCESSOR)
	target_compile_definitions(h2data PRIVATE RADIANT_EXECUTABLE=$<QUOTE>${SYSTEM_PROCESSOR}$<QUOTE>)
	set_target_properties(h2data PROPERTIES SUFFIX ".${SYSTEM_PROCESSOR}")
else()
	target_compile_definitions(h2data PRIVATE RADIANT_EXECUTABLE=$<QUOTE>unknown$<QUOTE>)
endif()
