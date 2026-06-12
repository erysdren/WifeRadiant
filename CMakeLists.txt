cmake_minimum_required(VERSION 3.21)
project(radiant LANGUAGES C CXX VERSION 1.6.0)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# global

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# options

include(CMakeDependentOption)

option(RADIANT_GENERATE_GAMEPACKS "Generate gamepacks" ON)
option(RADIANT_USE_ASSIMP "Build Assimp model loader plugin" ON)
option(RADIANT_SUPPORT_SOURCE "Support Source Engine formats" ON)
option(RADIANT_SUPPORT_GOLDSRC "Generate GoldSrc engine gamepacks" ON)
option(RADIANT_BUILD_MBSPC "Build mbspc" ON)
cmake_dependent_option(RADIANT_BUILD_Q3MAP2 "Build q3map2" ON RADIANT_USE_ASSIMP OFF)
option(RADIANT_BUILD_Q2MAP "Build q2map" ON)

set(RADIANT_VERSION ${PROJECT_VERSION})
set(RADIANT_MAJOR_VERSION ${PROJECT_VERSION_MAJOR})
set(RADIANT_MINOR_VERSION ${PROJECT_VERSION_MINOR})
set(RADIANT_PATCH_VERSION ${PROJECT_VERSION_PATCH})
set(RADIANT_ABOUTMSG "trans rights are human rights" CACHE STRING "")
set(Q3MAP_VERSION "2.5.17n" CACHE STRING "")

# external dependencies

cmake_policy(SET CMP0077 NEW)

include(FetchContent)

if(RADIANT_SUPPORT_SOURCE)
	FetchContent_Declare(
		sourcepp
		GIT_REPOSITORY https://github.com/craftablescience/sourcepp.git
		GIT_TAG bd60f5dd494903e2e7727ae798e1bc3f34f4a6e7
		EXCLUDE_FROM_ALL
	)

	set(SOURCEPP_LIBS_START_ENABLED OFF CACHE BOOL "" FORCE)
	set(SOURCEPP_USE_KVPP ON CACHE BOOL "" FORCE) # for VMTs
	set(SOURCEPP_USE_MDLPP ON CACHE BOOL "" FORCE) # for MDLs
	set(SOURCEPP_USE_TOOLPP ON CACHE BOOL "" FORCE) # for FGDs
	set(SOURCEPP_USE_VPKPP ON CACHE BOOL "" FORCE) # for VPKs
	set(SOURCEPP_USE_VTFPP ON CACHE BOOL "" FORCE) # for VTFs
	set(SOURCEPP_VTFPP_BUILD_WITH_COMPRESSONATOR OFF CACHE BOOL "" FORCE)
	set(SOURCEPP_VTFPP_SUPPORT_EXR OFF CACHE BOOL "" FORCE)
	set(SOURCEPP_VTFPP_SUPPORT_QOI OFF CACHE BOOL "" FORCE)
	set(SOURCEPP_VTFPP_SUPPORT_WEBP OFF CACHE BOOL "" FORCE)

	FetchContent_MakeAvailable(sourcepp)
endif()

find_package(ZLIB REQUIRED)

find_package(LibXml2)
if(NOT LibXml2_FOUND)
	FetchContent_Declare(
		libxml2
		GIT_REPOSITORY https://gitlab.gnome.org/GNOME/libxml2.git
		GIT_TAG "v2.15.1"
		EXCLUDE_FROM_ALL
	)
	set(LIBXML2_WITH_ICONV OFF CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(libxml2)
endif()

if(RADIANT_USE_ASSIMP)
	FetchContent_Declare(
		assimp
		GIT_REPOSITORY https://github.com/assimp/assimp.git
		GIT_TAG "v6.0.5"
		EXCLUDE_FROM_ALL
	)
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
endif()

# packages

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg OpenGL OpenGLWidgets)

qt_standard_project_setup()

# commandlib

add_library(commandlib STATIC
	${PROJECT_SOURCE_DIR}/libs/commandlib.cpp
)

target_link_libraries(commandlib PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::OpenGL Qt6::OpenGLWidgets)
target_include_directories(commandlib PRIVATE
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/libs
)

# mathlib

add_library(mathlib STATIC
	${PROJECT_SOURCE_DIR}/libs/mathlib/bbox.c
	${PROJECT_SOURCE_DIR}/libs/mathlib/line.c
	${PROJECT_SOURCE_DIR}/libs/mathlib/m4x4.c
	${PROJECT_SOURCE_DIR}/libs/mathlib/mathlib.c
	${PROJECT_SOURCE_DIR}/libs/mathlib/ray.c
)

target_include_directories(mathlib PRIVATE
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/libs
)

# gtkutil

add_library(gtkutil STATIC
	${PROJECT_SOURCE_DIR}/libs/gtkutil/accelerator.cpp
	${PROJECT_SOURCE_DIR}/libs/gtkutil/clipboard.cpp
	${PROJECT_SOURCE_DIR}/libs/gtkutil/dialog.cpp
	${PROJECT_SOURCE_DIR}/libs/gtkutil/entry.cpp
	${PROJECT_SOURCE_DIR}/libs/gtkutil/filechooser.cpp
	${PROJECT_SOURCE_DIR}/libs/gtkutil/glfont.cpp
	${PROJECT_SOURCE_DIR}/libs/gtkutil/glwidget.cpp
	${PROJECT_SOURCE_DIR}/libs/gtkutil/guisettings.cpp
	${PROJECT_SOURCE_DIR}/libs/gtkutil/idledraw.cpp
	${PROJECT_SOURCE_DIR}/libs/gtkutil/image.cpp
	${PROJECT_SOURCE_DIR}/libs/gtkutil/menu.cpp
	${PROJECT_SOURCE_DIR}/libs/gtkutil/messagebox.cpp
	${PROJECT_SOURCE_DIR}/libs/gtkutil/nonmodal.cpp
	${PROJECT_SOURCE_DIR}/libs/gtkutil/toolbar.cpp
	${PROJECT_SOURCE_DIR}/libs/gtkutil/widget.cpp
	${PROJECT_SOURCE_DIR}/libs/gtkutil/xorrectangle.cpp
)

target_link_libraries(gtkutil PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::OpenGL Qt6::OpenGLWidgets)
target_include_directories(gtkutil PRIVATE
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/libs
)

# l_net

add_library(l_net STATIC
	${PROJECT_SOURCE_DIR}/libs/l_net/l_net.c
	${PROJECT_SOURCE_DIR}/libs/l_net/l_net_wins.c
)

target_include_directories(l_net PRIVATE
	${PROJECT_SOURCE_DIR}/libs/l_net
	${PROJECT_SOURCE_DIR}/libs
)

# xmllib

add_library(xmllib STATIC
	${PROJECT_SOURCE_DIR}/libs/xml/ixml.cpp
	${PROJECT_SOURCE_DIR}/libs/xml/xmlelement.cpp
	${PROJECT_SOURCE_DIR}/libs/xml/xmlparser.cpp
	${PROJECT_SOURCE_DIR}/libs/xml/xmltextags.cpp
	${PROJECT_SOURCE_DIR}/libs/xml/xmlwriter.cpp
)

target_include_directories(xmllib PRIVATE
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/libs
)

target_link_libraries(xmllib PRIVATE LibXml2::LibXml2)

# quickhull

add_library(quickhull STATIC ${PROJECT_SOURCE_DIR}/libs/quickhull/QuickHull.cpp)

# filematch

add_library(filematch STATIC ${PROJECT_SOURCE_DIR}/libs/filematch/filematch.c)
target_include_directories(filematch PUBLIC ${PROJECT_SOURCE_DIR}/libs/filematch)

# ddslib

add_library(ddslib STATIC ${PROJECT_SOURCE_DIR}/libs/ddslib/ddslib.c)
target_include_directories(ddslib PUBLIC ${PROJECT_SOURCE_DIR}/libs/ddslib)

# etclib

add_library(etclib STATIC ${PROJECT_SOURCE_DIR}/libs/etclib/etclib.c)
target_include_directories(etclib PRIVATE ${PROJECT_SOURCE_DIR}/libs)
target_include_directories(etclib PUBLIC ${PROJECT_SOURCE_DIR}/libs/etclib)

# crnlib

add_library(crnlib STATIC ${PROJECT_SOURCE_DIR}/libs/crnlib/crnlib.cpp)
target_include_directories(crnlib PUBLIC ${PROJECT_SOURCE_DIR}/libs/crnlib)
target_link_libraries(crnlib PRIVATE ddslib)

# webplib

add_library(webplib STATIC ${PROJECT_SOURCE_DIR}/libs/webplib/webplib.cpp)
target_include_directories(webplib PUBLIC ${PROJECT_SOURCE_DIR}/libs/webplib)

# radiant

qt_add_executable(radiant WIN32
	${PROJECT_SOURCE_DIR}/radiant/autosave.cpp
	${PROJECT_SOURCE_DIR}/radiant/brushmanip.cpp
	${PROJECT_SOURCE_DIR}/radiant/brushmodule.cpp
	${PROJECT_SOURCE_DIR}/radiant/brushnode.cpp
	${PROJECT_SOURCE_DIR}/radiant/brush.cpp
	${PROJECT_SOURCE_DIR}/radiant/brush_primit.cpp
	${PROJECT_SOURCE_DIR}/radiant/brushtokens.cpp
	${PROJECT_SOURCE_DIR}/radiant/brushxml.cpp
	${PROJECT_SOURCE_DIR}/radiant/build.cpp
	${PROJECT_SOURCE_DIR}/radiant/camwindow.cpp
	${PROJECT_SOURCE_DIR}/radiant/clippertool.cpp
	${PROJECT_SOURCE_DIR}/radiant/colors.cpp
	${PROJECT_SOURCE_DIR}/radiant/commands.cpp
	${PROJECT_SOURCE_DIR}/radiant/console.cpp
	${PROJECT_SOURCE_DIR}/radiant/csg.cpp
	${PROJECT_SOURCE_DIR}/radiant/dialog.cpp
	${PROJECT_SOURCE_DIR}/radiant/eclass_def.cpp
	${PROJECT_SOURCE_DIR}/radiant/eclass_doom3.cpp
	$<$<NOT:$<BOOL:${RADIANT_SUPPORT_SOURCE}>>:${PROJECT_SOURCE_DIR}/radiant/eclass_fgd.cpp>
	$<$<BOOL:${RADIANT_SUPPORT_SOURCE}>:${PROJECT_SOURCE_DIR}/radiant/eclass_fgd_sourcepp.cpp>
	${PROJECT_SOURCE_DIR}/radiant/eclass.cpp
	${PROJECT_SOURCE_DIR}/radiant/eclass_xml.cpp
	${PROJECT_SOURCE_DIR}/radiant/entityinspector.cpp
	${PROJECT_SOURCE_DIR}/radiant/entitylist.cpp
	${PROJECT_SOURCE_DIR}/radiant/entity.cpp
	${PROJECT_SOURCE_DIR}/radiant/environment.cpp
	${PROJECT_SOURCE_DIR}/radiant/error.cpp
	${PROJECT_SOURCE_DIR}/radiant/feedback.cpp
	${PROJECT_SOURCE_DIR}/radiant/files.cpp
	${PROJECT_SOURCE_DIR}/radiant/filetypes.cpp
	${PROJECT_SOURCE_DIR}/radiant/filterbar.cpp
	${PROJECT_SOURCE_DIR}/radiant/filters.cpp
	${PROJECT_SOURCE_DIR}/radiant/findtexturedialog.cpp
	${PROJECT_SOURCE_DIR}/radiant/glwidget.cpp
	${PROJECT_SOURCE_DIR}/radiant/grid.cpp
	${PROJECT_SOURCE_DIR}/radiant/groupdialog.cpp
	${PROJECT_SOURCE_DIR}/radiant/gtkdlgs.cpp
	${PROJECT_SOURCE_DIR}/radiant/gtkmisc.cpp
	${PROJECT_SOURCE_DIR}/radiant/help.cpp
	$<$<BOOL:${WIN32}>:${PROJECT_SOURCE_DIR}/radiant/radiant.rc>
	${PROJECT_SOURCE_DIR}/radiant/image.cpp
	${PROJECT_SOURCE_DIR}/radiant/layerswindow.cpp
	${PROJECT_SOURCE_DIR}/radiant/mainframe.cpp
	${PROJECT_SOURCE_DIR}/radiant/main.cpp
	${PROJECT_SOURCE_DIR}/radiant/map.cpp
	${PROJECT_SOURCE_DIR}/radiant/modelwindow.cpp
	${PROJECT_SOURCE_DIR}/radiant/mru.cpp
	${PROJECT_SOURCE_DIR}/radiant/nullmodel.cpp
	${PROJECT_SOURCE_DIR}/radiant/parse.cpp
	${PROJECT_SOURCE_DIR}/radiant/patchdialog.cpp
	${PROJECT_SOURCE_DIR}/radiant/patchmanip.cpp
	${PROJECT_SOURCE_DIR}/radiant/patchmodule.cpp
	${PROJECT_SOURCE_DIR}/radiant/patch.cpp
	${PROJECT_SOURCE_DIR}/radiant/pluginapi.cpp
	${PROJECT_SOURCE_DIR}/radiant/pluginmanager.cpp
	${PROJECT_SOURCE_DIR}/radiant/pluginmenu.cpp
	${PROJECT_SOURCE_DIR}/radiant/plugin.cpp
	${PROJECT_SOURCE_DIR}/radiant/plugintoolbar.cpp
	${PROJECT_SOURCE_DIR}/radiant/points.cpp
	${PROJECT_SOURCE_DIR}/radiant/prefabs.cpp
	${PROJECT_SOURCE_DIR}/radiant/preferencedictionary.cpp
	${PROJECT_SOURCE_DIR}/radiant/preferences.cpp
	${PROJECT_SOURCE_DIR}/radiant/qe3.cpp
	${PROJECT_SOURCE_DIR}/radiant/qgl.cpp
	${PROJECT_SOURCE_DIR}/radiant/referencecache.cpp
	${PROJECT_SOURCE_DIR}/radiant/renderer.cpp
	${PROJECT_SOURCE_DIR}/radiant/renderstate.cpp
	${PROJECT_SOURCE_DIR}/radiant/scenegraph.cpp
	${PROJECT_SOURCE_DIR}/radiant/selection.cpp
	${PROJECT_SOURCE_DIR}/radiant/select.cpp
	${PROJECT_SOURCE_DIR}/radiant/server.cpp
	${PROJECT_SOURCE_DIR}/radiant/sockets.cpp
	${PROJECT_SOURCE_DIR}/radiant/stacktrace.cpp
	${PROJECT_SOURCE_DIR}/radiant/surfacedialog.cpp
	${PROJECT_SOURCE_DIR}/radiant/texmanip.cpp
	${PROJECT_SOURCE_DIR}/radiant/textures.cpp
	${PROJECT_SOURCE_DIR}/radiant/texwindow.cpp
	${PROJECT_SOURCE_DIR}/radiant/theme.cpp
	${PROJECT_SOURCE_DIR}/radiant/tools.cpp
	${PROJECT_SOURCE_DIR}/radiant/treemodel.cpp
	${PROJECT_SOURCE_DIR}/radiant/undo.cpp
	${PROJECT_SOURCE_DIR}/radiant/url.cpp
	${PROJECT_SOURCE_DIR}/radiant/view.cpp
	${PROJECT_SOURCE_DIR}/radiant/watchbsp.cpp
	${PROJECT_SOURCE_DIR}/radiant/winding.cpp
	${PROJECT_SOURCE_DIR}/radiant/windowobservers.cpp
	${PROJECT_SOURCE_DIR}/radiant/xmlstuff.cpp
	${PROJECT_SOURCE_DIR}/radiant/xywindow.cpp
)

file(GLOB RADIANT_RESOURCES "${PROJECT_SOURCE_DIR}/resources/*")
qt_add_resources(radiant "radiant"
	PREFIX
		"/"
	FILES
		${RADIANT_RESOURCES}
	BASE
		"${PROJECT_SOURCE_DIR}/resources"
)

if(0)
qt_generate_deploy_app_script(
	TARGET radiant
	OUTPUT_SCRIPT radiant_deploy_script
	NO_UNSUPPORTED_PLATFORM_ERROR
)
install(TARGETS radiant
	BUNDLE DESTINATION ${PROJECT_SOURCE_DIR}/install
	RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/install
)
install(SCRIPT ${radiant_deploy_script})
endif()

if(WIN32)
	install(CODE [[
		file(GET_RUNTIME_DEPENDENCIES
			RESOLVED_DEPENDENCIES_VAR _resolved_deps
			UNRESOLVED_DEPENDENCIES_VAR _unresolved_deps
			EXECUTABLES
				$<TARGET_FILE:radiant>
			PRE_EXCLUDE_REGEXES
				"api-ms-" "ext-ms-" "Qt6"
			POST_EXCLUDE_REGEXES
				".*system32/.*\\.dll"
			DIRECTORIES
				$<TARGET_RUNTIME_DLL_DIRS:radiant>
		)
		if(_unresolved_deps)
			message(WARNING "radiant unresolved dependencies: ${_unresolved_deps}")
		endif()
		file(COPY ${_resolved_deps} DESTINATION $<TARGET_FILE_DIR:radiant>)
	]])
endif()

target_compile_definitions(radiant PRIVATE QT_NO_KEYWORDS)
target_compile_definitions(radiant PRIVATE
	RADIANT_VERSION=$<QUOTE>${RADIANT_VERSION}$<QUOTE>
	RADIANT_MAJOR_VERSION=$<QUOTE>${RADIANT_MAJOR_VERSION}$<QUOTE>
	RADIANT_MINOR_VERSION=$<QUOTE>${RADIANT_MINOR_VERSION}$<QUOTE>
	RADIANT_PATCH_VERSION=$<QUOTE>${RADIANT_PATCH_VERSION}$<QUOTE>
	RADIANT_ABOUTMSG=$<QUOTE>${RADIANT_ABOUTMSG}$<QUOTE>
)

target_include_directories(radiant PRIVATE
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/libs
)

target_link_libraries(radiant PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::OpenGL Qt6::OpenGLWidgets)
target_link_libraries(radiant PRIVATE LibXml2::LibXml2)
target_link_libraries(radiant PRIVATE commandlib gtkutil l_net xmllib quickhull)
target_link_libraries(radiant PRIVATE $<$<BOOL:${RADIANT_SUPPORT_SOURCE}>:sourcepp::toolpp>)
target_link_libraries(radiant PRIVATE $<$<BOOL:${WIN32}>:ws2_32> $<$<BOOL:${WIN32}>:dbghelp>)
target_link_libraries(radiant PRIVATE $<$<PLATFORM_ID:Haiku>:network>)
target_compile_options(radiant PRIVATE
	$<$<BOOL:${MINGW}>:-static>
	$<$<BOOL:${MINGW}>:-static-libgcc>
	$<$<BOOL:${MINGW}>:-static-libstdc++>
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

target_compile_definitions(radiant PRIVATE $<$<CONFIG:Debug>:_DEBUG> $<$<NOT:$<BOOL:${WIN32}>>:POSIX> $<$<BOOL:${WIN32}>:WIN32>)
target_compile_definitions(commandlib PRIVATE $<$<CONFIG:Debug>:_DEBUG> $<$<NOT:$<BOOL:${WIN32}>>:POSIX> $<$<BOOL:${WIN32}>:WIN32>)
target_compile_definitions(gtkutil PRIVATE $<$<CONFIG:Debug>:_DEBUG> $<$<NOT:$<BOOL:${WIN32}>>:POSIX> $<$<BOOL:${WIN32}>:WIN32>)
target_compile_definitions(l_net PRIVATE $<$<CONFIG:Debug>:_DEBUG> $<$<NOT:$<BOOL:${WIN32}>>:POSIX> $<$<BOOL:${WIN32}>:WIN32>)
target_compile_definitions(xmllib PRIVATE $<$<CONFIG:Debug>:_DEBUG> $<$<NOT:$<BOOL:${WIN32}>>:POSIX> $<$<BOOL:${WIN32}>:WIN32>)

set_target_properties(radiant
	PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/install
		RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/install
)

file(WRITE ${PROJECT_SOURCE_DIR}/install/RADIANT_MAJOR ${RADIANT_MAJOR_VERSION})
file(WRITE ${PROJECT_SOURCE_DIR}/install/RADIANT_MINOR ${RADIANT_MINOR_VERSION})
file(WRITE ${PROJECT_SOURCE_DIR}/install/RADIANT_PATCH ${RADIANT_PATCH_VERSION})

file(GLOB RADIANT_DATA_FILES "${PROJECT_SOURCE_DIR}/resources/*")
file(COPY ${RADIANT_DATA_FILES} DESTINATION ${PROJECT_SOURCE_DIR}/install/)
file(COPY "${PROJECT_SOURCE_DIR}/docs/mouse shortcuts.txt" DESTINATION ${PROJECT_SOURCE_DIR}/install/docs/)

if(WIN32)
	target_compile_definitions(radiant PRIVATE RADIANT_EXECUTABLE=$<QUOTE>exe$<QUOTE>)
elseif(EMSCRIPTEN)
	target_compile_options(radiant PRIVATE -sMAIN_MODULE=1)
	target_compile_definitions(radiant PRIVATE RADIANT_EXECUTABLE=$<QUOTE>emscripten$<QUOTE>)
	set_target_properties(radiant PROPERTIES SUFFIX ".js")
elseif(DEFINED CMAKE_SYSTEM_PROCESSOR)
	string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} SYSTEM_PROCESSOR)
	target_compile_definitions(radiant PRIVATE RADIANT_EXECUTABLE=$<QUOTE>${SYSTEM_PROCESSOR}$<QUOTE>)
	set_target_properties(radiant PROPERTIES SUFFIX ".${SYSTEM_PROCESSOR}")
else()
	target_compile_definitions(radiant PRIVATE RADIANT_EXECUTABLE=$<QUOTE>unknown$<QUOTE>)
endif()

# gamepacks

if(RADIANT_GENERATE_GAMEPACKS)
	include(gamepacks)
endif()

# q3map2

if(RADIANT_BUILD_Q3MAP2)
	include(q3map2)
endif()

# mbspc

if(RADIANT_BUILD_MBSPC)
	include(mbspc)
endif()

# q2map

if(RADIANT_BUILD_Q2MAP)
	include(q2map)
endif()

# modules

include(modules)

# plugins

include(plugins)
