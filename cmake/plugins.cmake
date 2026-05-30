# plugins are generally optional and extend editor functionality

function(add_plugin name)
	cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "SOURCES")
	if(EMSCRIPTEN)
		add_executable(${name} ${ARG_SOURCES})
		target_compile_options(${name} PRIVATE -sSIDE_MODULE=2 -nostdlib)
		target_link_options(${name} PRIVATE -sSIDE_MODULE=2 -sSTANDALONE_WASM=1 -sERROR_ON_UNDEFINED_SYMBOLS=0 -nostdlib)
	else()
		add_library(${name} SHARED ${ARG_SOURCES})
	endif()
	set_target_properties(${name}
		PROPERTIES
			LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/install/plugins
			RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/install/plugins
			PREFIX ""
	)
	target_compile_definitions(${name} PRIVATE $<$<CONFIG:Debug>:_DEBUG> $<$<NOT:$<BOOL:${WIN32}>>:POSIX> $<$<BOOL:${WIN32}>:WIN32>)
	target_compile_options(${name} PRIVATE
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
	target_include_directories(${name} PRIVATE
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/libs
	)
	target_compile_definitions(${name} PRIVATE QT_NO_KEYWORDS)
	target_compile_definitions(${name} PRIVATE
		RADIANT_VERSION=$<QUOTE>${RADIANT_VERSION}$<QUOTE>
		RADIANT_MAJOR_VERSION=$<QUOTE>${RADIANT_MAJOR_VERSION}$<QUOTE>
		RADIANT_MINOR_VERSION=$<QUOTE>${RADIANT_MINOR_VERSION}$<QUOTE>
		RADIANT_PATCH_VERSION=$<QUOTE>${RADIANT_PATCH_VERSION}$<QUOTE>
		RADIANT_ABOUTMSG=$<QUOTE>${RADIANT_ABOUTMSG}$<QUOTE>
	)
endfunction()

add_plugin(brushexport
	SOURCES
		${PROJECT_SOURCE_DIR}/plugins/brushexport/callbacks.cpp
		${PROJECT_SOURCE_DIR}/plugins/brushexport/export.cpp
		${PROJECT_SOURCE_DIR}/plugins/brushexport/interface.cpp
		${PROJECT_SOURCE_DIR}/plugins/brushexport/plugin.cpp
)
target_link_libraries(brushexport PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::OpenGL Qt6::OpenGLWidgets)

add_plugin(prtview
	SOURCES
		${PROJECT_SOURCE_DIR}/plugins/prtview/AboutDialog.cpp
		${PROJECT_SOURCE_DIR}/plugins/prtview/ConfigDialog.cpp
		${PROJECT_SOURCE_DIR}/plugins/prtview/LoadPortalFileDialog.cpp
		${PROJECT_SOURCE_DIR}/plugins/prtview/portals.cpp
		${PROJECT_SOURCE_DIR}/plugins/prtview/prtview.cpp
)
target_link_libraries(prtview PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::OpenGL Qt6::OpenGLWidgets)

add_plugin(sunplug
	SOURCES
		${PROJECT_SOURCE_DIR}/plugins/sunplug/sunplug.cpp
)
target_link_libraries(sunplug PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::OpenGL Qt6::OpenGLWidgets)

add_plugin(ufoaiplug
	SOURCES
		${PROJECT_SOURCE_DIR}/plugins/ufoaiplug/ufoai_filters.cpp
		${PROJECT_SOURCE_DIR}/plugins/ufoaiplug/ufoai_gtk.cpp
		${PROJECT_SOURCE_DIR}/plugins/ufoaiplug/ufoai_level.cpp
		${PROJECT_SOURCE_DIR}/plugins/ufoaiplug/ufoai.cpp
)
target_link_libraries(ufoaiplug PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::OpenGL Qt6::OpenGLWidgets)

add_plugin(meshtex
	SOURCES
		${PROJECT_SOURCE_DIR}/plugins/meshtex/GeneralFunctionDialog.cpp
		${PROJECT_SOURCE_DIR}/plugins/meshtex/GenericDialog.cpp
		${PROJECT_SOURCE_DIR}/plugins/meshtex/GenericMainMenu.cpp
		${PROJECT_SOURCE_DIR}/plugins/meshtex/GenericPluginUI.cpp
		${PROJECT_SOURCE_DIR}/plugins/meshtex/GetInfoDialog.cpp
		${PROJECT_SOURCE_DIR}/plugins/meshtex/MainMenu.cpp
		${PROJECT_SOURCE_DIR}/plugins/meshtex/MeshEntity.cpp
		${PROJECT_SOURCE_DIR}/plugins/meshtex/MeshVisitor.cpp
		${PROJECT_SOURCE_DIR}/plugins/meshtex/PluginModule.cpp
		${PROJECT_SOURCE_DIR}/plugins/meshtex/PluginRegistration.cpp
		${PROJECT_SOURCE_DIR}/plugins/meshtex/PluginUI.cpp
		${PROJECT_SOURCE_DIR}/plugins/meshtex/RefCounted.cpp
		${PROJECT_SOURCE_DIR}/plugins/meshtex/SetScaleDialog.cpp
)
target_link_libraries(meshtex PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::OpenGL Qt6::OpenGLWidgets)

if(0)
add_plugin(gensurf
	SOURCES
		${PROJECT_SOURCE_DIR}/plugins/gtkgensurf/triangle.c
		${PROJECT_SOURCE_DIR}/plugins/gtkgensurf/bitmap.cpp
		${PROJECT_SOURCE_DIR}/plugins/gtkgensurf/dec.cpp
		${PROJECT_SOURCE_DIR}/plugins/gtkgensurf/face.cpp
		${PROJECT_SOURCE_DIR}/plugins/gtkgensurf/font.cpp
		${PROJECT_SOURCE_DIR}/plugins/gtkgensurf/gendlgs.cpp
		${PROJECT_SOURCE_DIR}/plugins/gtkgensurf/genmap.cpp
		${PROJECT_SOURCE_DIR}/plugins/gtkgensurf/gensurf.cpp
		${PROJECT_SOURCE_DIR}/plugins/gtkgensurf/heretic.cpp
		${PROJECT_SOURCE_DIR}/plugins/gtkgensurf/plugin.cpp
		${PROJECT_SOURCE_DIR}/plugins/gtkgensurf/view.cpp
)
target_link_libraries(gensurf PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::OpenGL Qt6::OpenGLWidgets)
target_compile_options(gensurf PRIVATE
	$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU,Clang>>:-Wno-old-style-definition>
	$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU,Clang>>:-Wno-unused-but-set-variable>
)
endif()
