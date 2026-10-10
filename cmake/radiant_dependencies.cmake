
cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0135 NEW)

include(FetchContent)

find_package(Math)

# zlib-ng

FetchContent_Declare(
	ZLIB
	GIT_REPOSITORY "https://github.com/zlib-ng/zlib-ng"
	GIT_TAG "stable"
	EXCLUDE_FROM_ALL
	OVERRIDE_FIND_PACKAGE
)
set(ZLIB_COMPAT ON CACHE BOOL "" FORCE)
set(ZLIB_ALIASES ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(ZLIB)
set(ZLIB_INCLUDE_DIR ${zlib_BINARY_DIR})
set(ZLIB_LIBRARIES $<TARGET_FILE:zlibstatic>)

# sourcepp

FetchContent_Declare(
	sourcepp
	GIT_REPOSITORY https://github.com/craftablescience/sourcepp.git
	GIT_TAG "v2026.9.11"
	EXCLUDE_FROM_ALL
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
set(MZ_ZLIB_FLAVOR "zlib" CACHE BOOL "" FORCE)
set(MZ_BZIP2 OFF CACHE BOOL "" FORCE)
set(SOURCEPP_LIBS_START_ENABLED OFF CACHE BOOL "" FORCE)
set(SOURCEPP_USE_KVPP ON CACHE BOOL "" FORCE) # for VMTs
set(SOURCEPP_USE_MDLPP ON CACHE BOOL "" FORCE) # for MDLs
set(SOURCEPP_USE_TOOLPP ON CACHE BOOL "" FORCE) # for FGDs
set(SOURCEPP_USE_VPKPP ON CACHE BOOL "" FORCE) # for VPKs
set(SOURCEPP_USE_VTFPP ON CACHE BOOL "" FORCE) # for VTFs
set(SOURCEPP_VTFPP_BUILD_WITH_COMPRESSONATOR OFF CACHE BOOL "" FORCE)
set(SOURCEPP_VTFPP_SUPPORT_EXR OFF CACHE BOOL "" FORCE)
set(SOURCEPP_VTFPP_SUPPORT_JXL OFF CACHE BOOL "" FORCE)
set(SOURCEPP_VTFPP_SUPPORT_QOI OFF CACHE BOOL "" FORCE)
set(SOURCEPP_VTFPP_SUPPORT_WEBP OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(sourcepp)

# ericw-tools

if(WRMAP_WITH_ERICWTOOLS)
	FetchContent_Declare(
		ericw-tools
		GIT_REPOSITORY "https://github.com/ericwa/ericw-tools.git"
		GIT_TAG "2.0.0-alpha11"
		EXCLUDE_FROM_ALL
		OVERRIDE_FIND_PACKAGE
		GIT_SHALLOW TRUE
		GIT_PROGRESS TRUE
	)
	set(DISABLE_TESTS ON CACHE BOOL "")
	set(DISABLE_DOCS ON CACHE BOOL "")
	set(ENABLE_LIGHTPREVIEW OFF CACHE BOOL "")
	set(SKIP_TBB_INSTALL ON CACHE BOOL "")
	set(SKIP_EMBREE_INSTALL ON CACHE BOOL "")
	FetchContent_MakeAvailable(ericw-tools)
endif()

# pugixml

FetchContent_Declare(
	pugixml
	GIT_REPOSITORY "https://github.com/zeux/pugixml.git"
	GIT_TAG "v1.16"
	EXCLUDE_FROM_ALL
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(pugixml)

# libxml2

FetchContent_Declare(
	LibXml2
	GIT_REPOSITORY "https://gitlab.gnome.org/GNOME/libxml2.git"
	GIT_TAG "v2.15.4"
	EXCLUDE_FROM_ALL
	OVERRIDE_FIND_PACKAGE
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
set(LIBXML2_WITH_ICONV OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(LibXml2)

# stb

FetchContent_Declare(
	stb
	GIT_REPOSITORY "https://github.com/nothings/stb.git"
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(stb)

# cgltf

FetchContent_Declare(
	cgltf
	GIT_REPOSITORY "https://github.com/jkuhlmann/cgltf.git"
	GIT_SHALLOW TRUE
	GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(cgltf)

# assimp

if(RADIANT_USE_ASSIMP)
	FetchContent_Declare(
		assimp
		GIT_REPOSITORY https://github.com/assimp/assimp.git
		GIT_TAG "v6.0.5"
		EXCLUDE_FROM_ALL
		OVERRIDE_FIND_PACKAGE
		GIT_SHALLOW TRUE
		GIT_PROGRESS TRUE
	)
	set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
	set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
	set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
	set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
	set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_MDL_IMPORTER OFF CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_MD2_IMPORTER OFF CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_MD3_IMPORTER OFF CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_MD5_IMPORTER OFF CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_MDC_IMPORTER OFF CACHE BOOL "" FORCE)
	set(ASSIMP_BUILD_HMP_IMPORTER OFF CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(assimp)

	if(TARGET assimp)
		find_path(MINIZIP_INCLUDE_DIR unzip.h PATH_SUFFIXES minizip)
		if(MINIZIP_INCLUDE_DIR)
			target_include_directories(assimp PRIVATE ${MINIZIP_INCLUDE_DIR})
		endif()
	endif()

	message(STATUS "Building with assimp")
endif()

# qt6

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg OpenGL OpenGLWidgets)

message(STATUS "Qt6 version is ${Qt6_VERSION}")
