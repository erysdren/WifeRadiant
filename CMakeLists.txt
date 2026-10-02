cmake_minimum_required(VERSION 3.27)
project(WifeRadiant LANGUAGES C CXX VERSION 2.0.0)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# global options

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(MSVC)
	set(CMAKE_RC_FLAGS "/nologo ${CMAKE_RC_FLAGS}")
	# set(CMAKE_EXE_LINKER_FLAGS "/VERBOSE /TIME ${CMAKE_EXE_LINKER_FLAGS}")
	# set(CMAKE_SHARED_LINKER_FLAGS "/VERBOSE /TIME ${CMAKE_SHARED_LINKER_FLAGS}")
	# set(CMAKE_STATIC_LINKER_FLAGS "/VERBOSE /TIME ${CMAKE_STATIC_LINKER_FLAGS}")
endif()

# options

include(CMakeDependentOption)

option(RADIANT_USE_ASSIMP "Build Assimp model loader plugin" ON)
option(RADIANT_USE_GLES2 "Use OpenGL ES 2.0 instead of OpenGL 2.0" OFF)
option(RADIANT_GENERATE_GAMEPACKS "Generate gamepacks" ON)
cmake_dependent_option(RADIANT_GENERATE_USER_GAMEPACKS "Generate user gamepacks" ON RADIANT_GENERATE_GAMEPACKS OFF)
cmake_dependent_option(RADIANT_GENERATE_IDTECH2_GAMEPACKS "Generate idTech 2 gamepacks" ON RADIANT_GENERATE_GAMEPACKS OFF)
cmake_dependent_option(RADIANT_GENERATE_IDTECH3_GAMEPACKS "Generate idTech 3 gamepacks" ON RADIANT_GENERATE_GAMEPACKS OFF)
cmake_dependent_option(RADIANT_GENERATE_IDTECH4_GAMEPACKS "Generate idTech 4 gamepacks" ON RADIANT_GENERATE_GAMEPACKS OFF)
cmake_dependent_option(RADIANT_GENERATE_SOURCE_GAMEPACKS "Generate Source Engine gamepacks" ON RADIANT_GENERATE_GAMEPACKS OFF)
cmake_dependent_option(RADIANT_GENERATE_GOLDSRC_GAMEPACKS "Generate GoldSrc gamepacks" ON RADIANT_GENERATE_GAMEPACKS OFF)
option(RADIANT_BUILD_TOOLS "Build editor tools" ON)
cmake_dependent_option(RADIANT_BUILD_MBSPC "Build mbspc" ON RADIANT_BUILD_TOOLS OFF)
cmake_dependent_option(RADIANT_BUILD_WRMAP "Build wrmap compiler" ON "RADIANT_BUILD_TOOLS;RADIANT_USE_ASSIMP" OFF)
cmake_dependent_option(WRMAP_WITH_ERICWTOOLS "Embed the ericw-tools frontend into wrmap" OFF RADIANT_BUILD_WRMAP OFF) # note: doesn't work yet
cmake_dependent_option(RADIANT_BUILD_Q2MAP "Build q2map" ON RADIANT_BUILD_TOOLS OFF)
cmake_dependent_option(RADIANT_BUILD_H2DATA "Build h2data" ON RADIANT_BUILD_TOOLS OFF)
cmake_dependent_option(RADIANT_BUILD_QDATA3 "Build qdata3" ON RADIANT_BUILD_TOOLS OFF)

# install prefix

if(NOT RADIANT_INSTALL_PREFIX)
	set(RADIANT_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install")
endif()

# revision

set(RADIANT_VERSION ${PROJECT_VERSION} CACHE BOOL "" FORCE)
set(RADIANT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE BOOL "" FORCE)
set(RADIANT_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE BOOL "" FORCE)
set(RADIANT_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE BOOL "" FORCE)
set(RADIANT_ABOUTMSG "fix your hearts or die" CACHE BOOL "" FORCE)
set(RADIANT_XMLNS "https://wiferadiant.org/xml/radiant")
set(Q3MAP_VERSION_MAJOR 2 CACHE BOOL "" FORCE)
set(Q3MAP_VERSION_MINOR 5 CACHE BOOL "" FORCE)
set(Q3MAP_VERSION_PATCH 17 CACHE BOOL "" FORCE)
set(Q3MAP_VERSION "${Q3MAP_VERSION_MAJOR}.${Q3MAP_VERSION_MINOR}.${Q3MAP_VERSION_PATCH}n" CACHE BOOL "" FORCE) # last q3map2 version before fork
set(WRMAP_VERSION_MAJOR 2 CACHE BOOL "" FORCE)
set(WRMAP_VERSION_MINOR 5 CACHE BOOL "" FORCE)
set(WRMAP_VERSION_PATCH 17 CACHE BOOL "" FORCE)
set(WRMAP_VERSION "${WRMAP_VERSION_MAJOR}.${WRMAP_VERSION_MINOR}.${WRMAP_VERSION_PATCH}w" CACHE BOOL "" FORCE)
set(WRMAP_MOTD "your map saw my transgender boobs" CACHE BOOL "" FORCE) # :3

file(WRITE ${RADIANT_INSTALL_PREFIX}/RADIANT_MAJOR ${RADIANT_VERSION_MAJOR})
file(WRITE ${RADIANT_INSTALL_PREFIX}/RADIANT_MINOR ${RADIANT_VERSION_MINOR})
file(WRITE ${RADIANT_INSTALL_PREFIX}/RADIANT_PATCH ${RADIANT_VERSION_PATCH})

# git revision

include(radiant_revision)

# common compile options and definitions

include(radiant_common)

# external dependencies

include(radiant_dependencies)

# setup qt project

if(Qt6_VERSION VERSION_GREATER 6.2.4)
	qt_standard_project_setup()
endif()

# helper function for the various static libs that radiant uses

function(radiant_add_library name)
	cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "SOURCES;INCLUDE_DIRECTORIES;DEPENDENCIES;COMPILE_DEFINITIONS;COMPILE_OPTIONS")
	set(target "wiferadiant-library-${name}")
	add_library(${target} STATIC ${ARG_SOURCES})
	radiant_add_common(${target})
	target_link_libraries(${target}
		PRIVATE
			${ARG_DEPENDENCIES}
	)
	target_compile_definitions(${target}
		PRIVATE
			${ARG_COMPILE_DEFINITIONS}
	)
	target_compile_options(${target}
		PRIVATE
			${ARG_COMPILE_OPTIONS}
	)
	target_include_directories(${target}
		PUBLIC
			${ARG_INCLUDE_DIRECTORIES}
	)
endfunction()

# stb

radiant_add_library(stb
	SOURCES
		${PROJECT_SOURCE_DIR}/libs/stb_image.c
	INCLUDE_DIRECTORIES
		${stb_SOURCE_DIR}
)

# commandlib

radiant_add_library(commandlib
	SOURCES
		${PROJECT_SOURCE_DIR}/libs/commandlib.cpp
	INCLUDE_DIRECTORIES
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/libs
	DEPENDENCIES
		Qt6::Core
		Qt6::Gui
		Qt6::Widgets
		Qt6::Svg
		Qt6::OpenGL
		Qt6::OpenGLWidgets
)

# mathlib

radiant_add_library(mathlib
	SOURCES
		${PROJECT_SOURCE_DIR}/libs/mathlib/bbox.c
		${PROJECT_SOURCE_DIR}/libs/mathlib/line.c
		${PROJECT_SOURCE_DIR}/libs/mathlib/m4x4.c
		${PROJECT_SOURCE_DIR}/libs/mathlib/mathlib.c
		${PROJECT_SOURCE_DIR}/libs/mathlib/ray.c
	INCLUDE_DIRECTORIES
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/libs
)

# gtkutil

radiant_add_library(gtkutil
	SOURCES
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
	INCLUDE_DIRECTORIES
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/libs
	DEPENDENCIES
		Qt6::Core
		Qt6::Gui
		Qt6::Widgets
		Qt6::Svg
		Qt6::OpenGL
		Qt6::OpenGLWidgets
)

# l_net

radiant_add_library(l_net
	SOURCES
		${PROJECT_SOURCE_DIR}/libs/l_net/l_net.c
		${PROJECT_SOURCE_DIR}/libs/l_net/l_net_wins.c
	INCLUDE_DIRECTORIES
		${PROJECT_SOURCE_DIR}/libs/l_net
		${PROJECT_SOURCE_DIR}/libs
)

# xmllib

radiant_add_library(xmllib
	SOURCES
		${PROJECT_SOURCE_DIR}/libs/xml/ixml.cpp
		${PROJECT_SOURCE_DIR}/libs/xml/xmlelement.cpp
		${PROJECT_SOURCE_DIR}/libs/xml/xmlparser.cpp
		${PROJECT_SOURCE_DIR}/libs/xml/xmltextags.cpp
		${PROJECT_SOURCE_DIR}/libs/xml/xmlwriter.cpp
	INCLUDE_DIRECTORIES
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/libs
	DEPENDENCIES
		LibXml2::LibXml2
)

# quickhull

radiant_add_library(quickhull
	SOURCES
		${PROJECT_SOURCE_DIR}/libs/quickhull/QuickHull.cpp
)

# filematch

radiant_add_library(filematch
	SOURCES
		${PROJECT_SOURCE_DIR}/libs/filematch/filematch.c
	INCLUDE_DIRECTORIES
		${PROJECT_SOURCE_DIR}/libs/filematch
)

# ddslib

radiant_add_library(ddslib
	SOURCES
		${PROJECT_SOURCE_DIR}/libs/ddslib/ddslib.c
	INCLUDE_DIRECTORIES
		${PROJECT_SOURCE_DIR}/libs/ddslib
)

# etclib

radiant_add_library(etclib
	SOURCES
		${PROJECT_SOURCE_DIR}/libs/etclib/etclib.c
	INCLUDE_DIRECTORIES
		${PROJECT_SOURCE_DIR}/libs/etclib
		${PROJECT_SOURCE_DIR}/libs
)

# crnlib

radiant_add_library(crnlib
	SOURCES
		${PROJECT_SOURCE_DIR}/libs/crnlib/crnlib.cpp
	INCLUDE_DIRECTORIES
		${PROJECT_SOURCE_DIR}/libs/crnlib
	DEPENDENCIES
		wiferadiant-library-ddslib
)

# webplib

radiant_add_library(webplib
	SOURCES
		${PROJECT_SOURCE_DIR}/libs/webplib/webplib.cpp
	INCLUDE_DIRECTORIES
		${PROJECT_SOURCE_DIR}/libs/webplib
)

# radiant

if(WIN32)
	set(RADIANT_RC_ORIGINAL_FILENAME "WifeRadiant.${RADIANT_EXECUTABLE_SUFFIX}")
	set(RADIANT_RC_ICON_FILENAME "${PROJECT_SOURCE_DIR}/resources/bitmaps/logo.ico")
	set(RADIANT_RC_FILEVERSION "${RADIANT_VERSION_MAJOR},${RADIANT_VERSION_MINOR},${RADIANT_VERSION_PATCH},0")
	configure_file("${PROJECT_SOURCE_DIR}/radiant/radiant.rc.in" "${PROJECT_BINARY_DIR}/radiant.rc" @ONLY)
endif()

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
	${PROJECT_SOURCE_DIR}/radiant/eclass_fgd.cpp
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
	$<$<BOOL:${WIN32}>:${PROJECT_BINARY_DIR}/radiant.rc>
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

radiant_add_common(radiant)

file(GLOB RADIANT_RESOURCES "${PROJECT_SOURCE_DIR}/resources/*")
qt_add_resources(radiant "radiant"
	PREFIX
		"/"
	FILES
		${RADIANT_RESOURCES}
	BASE
		"${PROJECT_SOURCE_DIR}/resources"
)

if(APPLE)
	# FIXME: shitty hack
	add_custom_command(
		TARGET radiant
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${RADIANT_INSTALL_PREFIX}/gamepacks/" "${RADIANT_INSTALL_PREFIX}/WifeRadiant.app/Contents/Resources/gamepacks/"
		VERBATIM
	)
	set(radiant_icon_macos "${PROJECT_SOURCE_DIR}/resources/bitmaps/wiferadiant.icns")
	set_source_files_properties(${RADIANT_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
	set_source_files_properties(${radiant_icon_macos} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
	set_target_properties(radiant
		PROPERTIES
			MACOSX_BUNDLE TRUE
			MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
			MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
			MACOSX_BUNDLE_GUI_IDENTIFIER "biz.wifesoft-gameware.wiferadiant"
			MACOSX_BUNDLE_ICON_FILE "wiferadiant.icns"
	)
	qt_generate_deploy_app_script(
		TARGET radiant
		OUTPUT_SCRIPT radiant_deploy_script
		NO_UNSUPPORTED_PLATFORM_ERROR
	)
	install(TARGETS radiant
		BUNDLE DESTINATION ${RADIANT_INSTALL_PREFIX}
		RUNTIME DESTINATION ${RADIANT_INSTALL_PREFIX}
	)
	install(SCRIPT ${radiant_deploy_script})
endif()

target_include_directories(radiant
	PRIVATE
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/libs
)

if(EMSCRIPTEN)
	set_target_properties(radiant
		PROPERTIES
			OUTPUT_NAME WifeRadiant
			LIBRARY_OUTPUT_DIRECTORY ${RADIANT_INSTALL_PREFIX}
			RUNTIME_OUTPUT_DIRECTORY ${RADIANT_INSTALL_PREFIX}
	)
else()
	set_target_properties(radiant
		PROPERTIES
			OUTPUT_NAME WifeRadiant
			LIBRARY_OUTPUT_DIRECTORY $<1:${RADIANT_INSTALL_PREFIX}>
			RUNTIME_OUTPUT_DIRECTORY $<1:${RADIANT_INSTALL_PREFIX}>
	)
endif()

target_link_libraries(radiant
	PRIVATE
		Qt6::Core
		Qt6::Gui
		Qt6::Widgets
		Qt6::Svg
		Qt6::OpenGL
		Qt6::OpenGLWidgets
		LibXml2::LibXml2
		wiferadiant-library-commandlib
		wiferadiant-library-gtkutil
		wiferadiant-library-l_net
		wiferadiant-library-xmllib
		wiferadiant-library-quickhull
		sourcepp::toolpp
		$<$<BOOL:${WIN32}>:ws2_32>
		$<$<BOOL:${WIN32}>:dbghelp>
		$<$<PLATFORM_ID:Haiku>:network>
)

if(NOT APPLE)
	file(GLOB RADIANT_DATA_FILES "${PROJECT_SOURCE_DIR}/resources/*")
	file(COPY ${RADIANT_DATA_FILES} DESTINATION "${RADIANT_INSTALL_PREFIX}/")
	file(COPY "${PROJECT_SOURCE_DIR}/docs" DESTINATION "${RADIANT_INSTALL_PREFIX}/")
	file(WRITE "${RADIANT_INSTALL_PREFIX}/settings/settings.txt" "local settings go here")
endif()

# gamepacks

if(RADIANT_GENERATE_GAMEPACKS OR RADIANT_GENERATE_USER_GAMEPACKS)
	include(radiant_gamepacks)
endif()

# tools

include(radiant_tools)

# modules

include(radiant_modules)

# plugins

include(radiant_plugins)
