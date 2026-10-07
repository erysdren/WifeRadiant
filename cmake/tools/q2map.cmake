if(NOT RADIANT_BUILD_Q2MAP)
	return()
endif()

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
radiant_add_common(q2map)
target_link_libraries(q2map
	PRIVATE
		l_net
		$<$<BOOL:${WIN32}>:ws2_32>
		LibXml2::LibXml2
		$<TARGET_NAME_IF_EXISTS:Math::Math>
)
target_include_directories(q2map
	PRIVATE
		${PROJECT_SOURCE_DIR}/tools/quake2/common
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/libs
)
set_target_properties(q2map
	PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY $<1:${RADIANT_INSTALL_PREFIX}>
		RUNTIME_OUTPUT_DIRECTORY $<1:${RADIANT_INSTALL_PREFIX}>
)
