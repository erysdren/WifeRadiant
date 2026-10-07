if(NOT RADIANT_BUILD_QDATA3)
	return()
endif()

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
target_link_libraries(qdata3
	PRIVATE
		l_net
		$<$<BOOL:${WIN32}>:ws2_32>
		LibXml2::LibXml2
		$<TARGET_NAME_IF_EXISTS:Math::Math>
)
target_include_directories(qdata3
	PRIVATE
		${PROJECT_SOURCE_DIR}/tools/quake2/common
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/libs
)
set_target_properties(qdata3
	PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY $<1:${RADIANT_INSTALL_PREFIX}>
		RUNTIME_OUTPUT_DIRECTORY $<1:${RADIANT_INSTALL_PREFIX}>
)
