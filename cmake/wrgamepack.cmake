
# wrgamepack

add_executable(wrgamepack
	${PROJECT_SOURCE_DIR}/tools/wrgamepack/main.cpp
)
radiant_add_common(wrgamepack)
target_link_libraries(wrgamepack
	PRIVATE
		pugixml::pugixml
)
set_target_properties(wrgamepack
	PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY $<1:${RADIANT_INSTALL_PREFIX}>
		RUNTIME_OUTPUT_DIRECTORY $<1:${RADIANT_INSTALL_PREFIX}>
)
