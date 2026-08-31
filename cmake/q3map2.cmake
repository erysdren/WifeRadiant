
# q3map2

add_executable(q3map2
	${PROJECT_SOURCE_DIR}/tools/quake3/common/cmdlib.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/common/qimagelib.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/common/inout.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/common/md4.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/common/mutex.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/common/polylib.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/common/scriplib.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/common/threads.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/common/unzip.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/common/vfs.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/common/miniz.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/autopk3.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/brush.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/bspfile_abstract.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/bspfile_ibsp.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/bspfile_rbsp.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/bsp.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/convert_ase.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/convert_bsp.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/convert_json.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/convert_map.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/convert_obj.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/decals.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/exportents.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/facebsp.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/fog.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/games.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/help.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/image.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/leakfile.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/light_bounce.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/lightmaps_ydnar.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/light.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/light_trace.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/light_ydnar.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/main.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/map.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/minimap.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/mesh.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/model.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/patch.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/path_init.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/portals.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/prtfile.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/shaders.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/surface_extra.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/surface_foliage.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/surface_fur.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/surface_meta.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/surface.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/tjunction.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/tree.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/visflow.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/vis.cpp
	${PROJECT_SOURCE_DIR}/tools/quake3/q3map2/writebsp.cpp
)
target_link_libraries(q3map2 PRIVATE l_net filematch ddslib etclib crnlib webplib)
target_link_libraries(q3map2 PRIVATE pugixml::pugixml)
target_link_libraries(q3map2 PRIVATE assimp $<$<BOOL:${WIN32}>:ws2_32>)
target_include_directories(q3map2 PRIVATE
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/libs
	${PROJECT_SOURCE_DIR}/tools/quake3/common
)
target_compile_definitions(q3map2 PRIVATE
	RADIANT_VERSION=\"${RADIANT_VERSION}\"
	RADIANT_MAJOR_VERSION=\"${RADIANT_MAJOR_VERSION}\"
	RADIANT_MINOR_VERSION=\"${RADIANT_MINOR_VERSION}\"
	RADIANT_PATCH_VERSION=\"${RADIANT_PATCH_VERSION}\"
	RADIANT_ABOUTMSG=\"${RADIANT_ABOUTMSG}\"
	Q3MAP_VERSION=\"${Q3MAP_VERSION}\"
)
set_target_properties(q3map2
	PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${RADIANT_INSTALL_PREFIX}
		RUNTIME_OUTPUT_DIRECTORY ${RADIANT_INSTALL_PREFIX}
)
target_compile_options(q3map2 PRIVATE
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
				$<TARGET_FILE:q3map2>
			PRE_EXCLUDE_REGEXES
				"api-ms-" "ext-ms-" "Qt6"
			POST_EXCLUDE_REGEXES
				".*system32/.*\\.dll"
			DIRECTORIES
				$<TARGET_RUNTIME_DLL_DIRS:q3map2>
		)
		if(_unresolved_deps)
			message(WARNING "q3map2 unresolved dependencies: ${_unresolved_deps}")
		endif()
		file(COPY ${_resolved_deps} DESTINATION $<TARGET_FILE_DIR:q3map2>)
	]])
endif()

target_compile_definitions(q3map2 PRIVATE $<$<CONFIG:Debug>:_DEBUG> $<$<NOT:$<BOOL:${WIN32}>>:POSIX> $<$<BOOL:${WIN32}>:WIN32>)
if(WIN32)
	target_compile_definitions(q3map2 PRIVATE RADIANT_EXECUTABLE=\"exe\")
elseif(DEFINED CMAKE_SYSTEM_PROCESSOR)
	string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} SYSTEM_PROCESSOR)
	target_compile_definitions(q3map2 PRIVATE RADIANT_EXECUTABLE=\"${SYSTEM_PROCESSOR}\")
	set_target_properties(q3map2 PROPERTIES SUFFIX ".${SYSTEM_PROCESSOR}")
else()
	target_compile_definitions(q3map2 PRIVATE RADIANT_EXECUTABLE=\"unknown\")
endif()
