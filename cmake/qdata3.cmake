
# qdata3

add_executable(qdata3
		${PROJECT_SOURCE_DIR}/tools/quake2/common/bspfile.cpp
		${PROJECT_SOURCE_DIR}/tools/quake2/common/cmdlib.cpp
		${PROJECT_SOURCE_DIR}/tools/quake2/common/inout.cpp
		${PROJECT_SOURCE_DIR}/tools/quake2/common/l3dslib.cpp
		${PROJECT_SOURCE_DIR}/tools/quake2/common/lbmlib.cpp
		${PROJECT_SOURCE_DIR}/tools/quake2/common/mathlib.cpp
		${PROJECT_SOURCE_DIR}/tools/quake2/common/md4.cpp
		${PROJECT_SOURCE_DIR}/tools/quake2/common/path_init.cpp
		${PROJECT_SOURCE_DIR}/tools/quake2/common/polylib.cpp
		${PROJECT_SOURCE_DIR}/tools/quake2/common/scriplib.cpp
		${PROJECT_SOURCE_DIR}/tools/quake2/common/threads.cpp
		${PROJECT_SOURCE_DIR}/tools/quake2/common/trilib.cpp
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata/images.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata/models.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata/qdata.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata/sprites.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata/tables.c
	${PROJECT_SOURCE_DIR}/tools/quake2/qdata/video.c
)
target_link_libraries(qdata3 PRIVATE l_net $<$<BOOL:${WIN32}>:ws2_32>)
target_link_libraries(qdata3 PRIVATE pugixml::pugixml)
target_include_directories(qdata3 PRIVATE
	${PROJECT_SOURCE_DIR}/tools/quake2/common
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/libs
)
target_compile_definitions(qdata3 PRIVATE
	RADIANT_VERSION=\"${RADIANT_VERSION}\"
	RADIANT_MAJOR_VERSION=\"${RADIANT_MAJOR_VERSION}\"
	RADIANT_MINOR_VERSION=\"${RADIANT_MINOR_VERSION}\"
	RADIANT_PATCH_VERSION=\"${RADIANT_PATCH_VERSION}\"
	RADIANT_ABOUTMSG=\"${RADIANT_ABOUTMSG}\"
)
set_target_properties(qdata3
	PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/install
		RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/install
)
target_compile_options(qdata3 PRIVATE
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

target_compile_definitions(qdata3 PRIVATE $<$<CONFIG:Debug>:_DEBUG> $<$<NOT:$<BOOL:${WIN32}>>:POSIX> $<$<BOOL:${WIN32}>:WIN32>)
if(WIN32)
	target_compile_definitions(qdata3 PRIVATE RADIANT_EXECUTABLE=\"exe\")
elseif(DEFINED CMAKE_SYSTEM_PROCESSOR)
	string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} SYSTEM_PROCESSOR)
	target_compile_definitions(qdata3 PRIVATE RADIANT_EXECUTABLE=\"${SYSTEM_PROCESSOR}\")
	set_target_properties(qdata3 PROPERTIES SUFFIX ".${SYSTEM_PROCESSOR}")
else()
	target_compile_definitions(qdata3 PRIVATE RADIANT_EXECUTABLE=\"unknown\")
endif()
