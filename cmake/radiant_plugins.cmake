# plugins are generally optional and extend editor functionality

function(radiant_add_plugin name)
	cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "SOURCES;INCLUDE_DIRECTORIES;DEPENDENCIES;COMPILE_DEFINITIONS;COMPILE_OPTIONS")
	set(target "wiferadiant-plugin-${name}")
	if(EMSCRIPTEN)
		add_executable(${target} ${ARG_SOURCES})
	else()
		add_library(${target} SHARED ${ARG_SOURCES})
	endif()
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
		PRIVATE
			${ARG_INCLUDE_DIRECTORIES}
	)
	set_target_properties(${target}
		PROPERTIES
			OUTPUT_NAME ${name}
			LIBRARY_OUTPUT_DIRECTORY $<1:${RADIANT_INSTALL_PREFIX}/plugins>
			RUNTIME_OUTPUT_DIRECTORY $<1:${RADIANT_INSTALL_PREFIX}/plugins>
	)
	target_link_options(${target}
		PRIVATE
			$<$<C_COMPILER_ID:GNU,Clang>:-Wl,--no-undefined>
	)
	target_include_directories(${target} PRIVATE
		${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/libs
	)
endfunction()

radiant_add_plugin(brushexport
	SOURCES
		${PROJECT_SOURCE_DIR}/plugins/brushexport/callbacks.cpp
		${PROJECT_SOURCE_DIR}/plugins/brushexport/export.cpp
		${PROJECT_SOURCE_DIR}/plugins/brushexport/interface.cpp
		${PROJECT_SOURCE_DIR}/plugins/brushexport/plugin.cpp
	DEPENDENCIES
		Qt6::Core
		Qt6::Gui
		Qt6::Widgets
		Qt6::Svg
		Qt6::OpenGL
		Qt6::OpenGLWidgets
)

radiant_add_plugin(prtview
	SOURCES
		${PROJECT_SOURCE_DIR}/plugins/prtview/AboutDialog.cpp
		${PROJECT_SOURCE_DIR}/plugins/prtview/ConfigDialog.cpp
		${PROJECT_SOURCE_DIR}/plugins/prtview/LoadPortalFileDialog.cpp
		${PROJECT_SOURCE_DIR}/plugins/prtview/portals.cpp
		${PROJECT_SOURCE_DIR}/plugins/prtview/prtview.cpp
	DEPENDENCIES
		Qt6::Core
		Qt6::Gui
		Qt6::Widgets
		Qt6::Svg
		Qt6::OpenGL
		Qt6::OpenGLWidgets
)

radiant_add_plugin(sunplug
	SOURCES
		${PROJECT_SOURCE_DIR}/plugins/sunplug/sunplug.cpp
	DEPENDENCIES
		Qt6::Core
		Qt6::Gui
		Qt6::Widgets
		Qt6::Svg
		Qt6::OpenGL
		Qt6::OpenGLWidgets
)

radiant_add_plugin(ufoaiplug
	SOURCES
		${PROJECT_SOURCE_DIR}/plugins/ufoaiplug/ufoai_filters.cpp
		${PROJECT_SOURCE_DIR}/plugins/ufoaiplug/ufoai_gtk.cpp
		${PROJECT_SOURCE_DIR}/plugins/ufoaiplug/ufoai_level.cpp
		${PROJECT_SOURCE_DIR}/plugins/ufoaiplug/ufoai.cpp
	DEPENDENCIES
		Qt6::Core
		Qt6::Gui
		Qt6::Widgets
		Qt6::Svg
		Qt6::OpenGL
		Qt6::OpenGLWidgets
)

radiant_add_plugin(meshtex
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
	DEPENDENCIES
		Qt6::Core
		Qt6::Gui
		Qt6::Widgets
		Qt6::Svg
		Qt6::OpenGL
		Qt6::OpenGLWidgets
)

radiant_add_plugin(bobtoolz
	SOURCES
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/bobToolz-GTK.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/bsploader.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/cportals.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/DBobView.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/DBrush.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/DEntity.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/DEPair.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/dialogs/dialogs-gtk.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/DMap.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/DPatch.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/DPlane.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/DPoint.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/DShape.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/DTrainDrawer.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/DTreePlanter.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/DVisDrawer.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/DWinding.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/funchandlers-GTK.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/lists.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/misc.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/ScriptParser.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/shapes.cpp
		${PROJECT_SOURCE_DIR}/plugins/bobtoolz/visfind.cpp
	DEPENDENCIES
		commandlib
		mathlib
		Qt6::Core
		Qt6::Gui
		Qt6::Widgets
		Qt6::Svg
		Qt6::OpenGL
		Qt6::OpenGLWidgets
)

radiant_add_plugin(shaderplug
	SOURCES
		${PROJECT_SOURCE_DIR}/plugins/shaderplug/shaderplug.cpp
	DEPENDENCIES
		xmllib
		LibXml2::LibXml2
)

if(0)
radiant_add_plugin(gensurf
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
target_link_libraries(gensurf PRIVATE mathlib)
target_link_libraries(gensurf PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::OpenGL Qt6::OpenGLWidgets)
target_compile_options(gensurf PRIVATE
	$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU,Clang>>:-Wno-old-style-definition>
	$<$<AND:$<COMPILE_LANGUAGE:C>,$<C_COMPILER_ID:GNU,Clang>>:-Wno-unused-but-set-variable>
)
endif()
