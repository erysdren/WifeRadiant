
# qdata3

add_executable(qdata3
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
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata/images.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata/models.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata/qdata.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata/sprites.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata/tables.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata/video.c
)
radiant_add_common(qdata3)
target_link_libraries(qdata3 PRIVATE l_net $<$<BOOL:${WIN32}>:ws2_32>)
target_link_libraries(qdata3 PRIVATE LibXml2::LibXml2)
target_link_libraries(qdata3 PRIVATE $<TARGET_NAME_IF_EXISTS:Math::Math>)
target_include_directories(qdata3 PRIVATE
	${PROJECT_SOURCE_DIR}/tools/quake2/common
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/libs
)
set_target_properties(qdata3
	PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${RADIANT_INSTALL_PREFIX}
		RUNTIME_OUTPUT_DIRECTORY ${RADIANT_INSTALL_PREFIX}
)

if(WIN32)
	install(CODE [[
		file(GET_RUNTIME_DEPENDENCIES
			RESOLVED_DEPENDENCIES_VAR _resolved_deps
			UNRESOLVED_DEPENDENCIES_VAR _unresolved_deps
			EXECUTABLES
				$<TARGET_FILE:qdata3>
			PRE_EXCLUDE_REGEXES
				"api-ms-" "ext-ms-" "Qt6"
			POST_EXCLUDE_REGEXES
				".*system32/.*\\.dll"
			DIRECTORIES
				$<TARGET_RUNTIME_DLL_DIRS:qdata3>
		)
		if(_unresolved_deps)
			message(WARNING "qdata3 unresolved dependencies: ${_unresolved_deps}")
		endif()
		file(COPY ${_resolved_deps} DESTINATION $<TARGET_FILE_DIR:qdata3>)
	]])
endif()
