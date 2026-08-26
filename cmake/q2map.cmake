
# q2map

add_executable(q2map
	${PROJECT_SOURCE_DIR}/tools/quake2/common/bspfile.c
	${PROJECT_SOURCE_DIR}/tools/quake2/common/cmdlib.c
	${PROJECT_SOURCE_DIR}/tools/quake2/common/inout.c
	${PROJECT_SOURCE_DIR}/tools/quake2/common/l3dslib.c
	${PROJECT_SOURCE_DIR}/tools/quake2/common/lbmlib.c
	${PROJECT_SOURCE_DIR}/tools/quake2/common/mathlib.c
	${PROJECT_SOURCE_DIR}/tools/quake2/common/md4.c
	${PROJECT_SOURCE_DIR}/tools/quake2/common/path_init.c
	${PROJECT_SOURCE_DIR}/tools/quake2/common/polylib.c
	${PROJECT_SOURCE_DIR}/tools/quake2/common/scriplib.c
	${PROJECT_SOURCE_DIR}/tools/quake2/common/threads.c
	${PROJECT_SOURCE_DIR}/tools/quake2/common/trilib.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/brushbsp.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/csg.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/faces.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/flow.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/glfile.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/leakfile.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/lightmap.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/main.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/map.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/nodraw.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/patches.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/portals.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/prtfile.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/qbsp.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/qrad.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/qvis.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/textures.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/trace.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/tree.c
	${PROJECT_SOURCE_DIR}/tools/quake2/q2map/writebsp.c
)
target_link_libraries(q2map PRIVATE l_net $<$<BOOL:${WIN32}>:ws2_32>)
target_link_libraries(q2map PRIVATE LibXml2::LibXml2)
target_include_directories(q2map PRIVATE
	${PROJECT_SOURCE_DIR}/tools/quake2/common
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/libs
)
target_compile_definitions(q2map PRIVATE
	RADIANT_VERSION=\"${RADIANT_VERSION}\"
	RADIANT_MAJOR_VERSION=\"${RADIANT_MAJOR_VERSION}\"
	RADIANT_MINOR_VERSION=\"${RADIANT_MINOR_VERSION}\"
	RADIANT_PATCH_VERSION=\"${RADIANT_PATCH_VERSION}\"
	RADIANT_ABOUTMSG=\"${RADIANT_ABOUTMSG}\"
)
set_target_properties(q2map
	PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${RADIANT_INSTALL_PREFIX}
		RUNTIME_OUTPUT_DIRECTORY ${RADIANT_INSTALL_PREFIX}
)
target_compile_options(q2map PRIVATE
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
				$<TARGET_FILE:q2map>
			PRE_EXCLUDE_REGEXES
				"api-ms-" "ext-ms-" "Qt6"
			POST_EXCLUDE_REGEXES
				".*system32/.*\\.dll"
			DIRECTORIES
				$<TARGET_RUNTIME_DLL_DIRS:q2map>
		)
		if(_unresolved_deps)
			message(WARNING "q2map unresolved dependencies: ${_unresolved_deps}")
		endif()
		file(COPY ${_resolved_deps} DESTINATION $<TARGET_FILE_DIR:q2map>)
	]])
endif()

target_compile_definitions(q2map PRIVATE $<$<CONFIG:Debug>:_DEBUG> $<$<NOT:$<BOOL:${WIN32}>>:POSIX> $<$<BOOL:${WIN32}>:WIN32>)
if(WIN32)
	target_compile_definitions(q2map PRIVATE RADIANT_EXECUTABLE=\"exe\")
elseif(DEFINED CMAKE_SYSTEM_PROCESSOR)
	string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} SYSTEM_PROCESSOR)
	target_compile_definitions(q2map PRIVATE RADIANT_EXECUTABLE=\"${SYSTEM_PROCESSOR}\")
	set_target_properties(q2map PROPERTIES SUFFIX ".${SYSTEM_PROCESSOR}")
else()
	target_compile_definitions(q2map PRIVATE RADIANT_EXECUTABLE=\"unknown\")
endif()
