if(NOT RADIANT_BUILD_TOOLS)
	return()
endif()

file(GLOB tools "${PROJECT_SOURCE_DIR}/cmake/tools/*.cmake")
foreach(tool IN LISTS tools)
	include(${tool})
endforeach()
