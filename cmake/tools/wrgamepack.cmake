if(NOT RADIANT_BUILD_WRGAMEPACK)
	return()
endif()

radiant_add_tool(wrgamepack
	SOURCES
		${PROJECT_SOURCE_DIR}/tools/wrgamepack/main.cpp
	DEPENDENCIES
		pugixml::pugixml
)
