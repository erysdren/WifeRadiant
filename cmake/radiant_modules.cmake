# modules are generally not optional and add support for image formats, model formats, shader formats, etc

function(radiant_add_module name)
	cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "SOURCES;INCLUDE_DIRECTORIES;DEPENDENCIES;COMPILE_DEFINITIONS;COMPILE_OPTIONS")
	set(target "wiferadiant-module-${name}")
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
			LIBRARY_OUTPUT_DIRECTORY $<1:${RADIANT_INSTALL_PREFIX}/modules>
			RUNTIME_OUTPUT_DIRECTORY $<1:${RADIANT_INSTALL_PREFIX}/modules>
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

radiant_add_module(archivepak
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/archivepak/archive.cpp
		${PROJECT_SOURCE_DIR}/modules/archivepak/pak.cpp
		${PROJECT_SOURCE_DIR}/modules/archivepak/plugin.cpp
)

radiant_add_module(archivevpk
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/archivevpk/archive.cpp
		${PROJECT_SOURCE_DIR}/modules/archivevpk/plugin.cpp
	DEPENDENCIES
		sourcepp::vpkpp
)

radiant_add_module(archivezip
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/archivezip/archive.cpp
		${PROJECT_SOURCE_DIR}/modules/archivezip/pkzip.cpp
		${PROJECT_SOURCE_DIR}/modules/archivezip/plugin.cpp
		${PROJECT_SOURCE_DIR}/modules/archivezip/zlibstream.cpp
	DEPENDENCIES
		${ZLIB_LIBRARIES}
	INCLUDE_DIRECTORIES
		${ZLIB_INCLUDE_DIR}
)

radiant_add_module(archivewad
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/archivewad/archive.cpp
		${PROJECT_SOURCE_DIR}/modules/archivewad/plugin.cpp
		${PROJECT_SOURCE_DIR}/modules/archivewad/wad.cpp
)

radiant_add_module(entity
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/entity/angle.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/angles.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/colour.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/doom3group.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/eclassmodel.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/entity.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/filters.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/generic.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/group.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/light.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/miscmodel.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/model.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/modelskinkey.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/namedentity.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/origin.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/plugin.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/rotation.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/scale.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/skincache.cpp
		${PROJECT_SOURCE_DIR}/modules/entity/targetable.cpp
	DEPENDENCIES
		Qt6::Core
		Qt6::Gui
		Qt6::Widgets
		Qt6::Svg
		Qt6::OpenGL
		Qt6::OpenGLWidgets
)

radiant_add_module(image
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/image/bmp.cpp
		${PROJECT_SOURCE_DIR}/modules/image/crn.cpp
		${PROJECT_SOURCE_DIR}/modules/image/dds.cpp
		${PROJECT_SOURCE_DIR}/modules/image/image.cpp
		${PROJECT_SOURCE_DIR}/modules/image/ktx.cpp
		${PROJECT_SOURCE_DIR}/modules/image/pcx.cpp
		${PROJECT_SOURCE_DIR}/modules/image/stb.cpp
		${PROJECT_SOURCE_DIR}/modules/image/tga.cpp
		${PROJECT_SOURCE_DIR}/modules/image/webp.cpp
	DEPENDENCIES
		wiferadiant-library-ddslib
		wiferadiant-library-etclib
		wiferadiant-library-crnlib
		wiferadiant-library-webplib
		wiferadiant-library-stb
)

radiant_add_module(imagevtf
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/imagevtf/imagevtf.cpp
		${PROJECT_SOURCE_DIR}/modules/imagevtf/vtf.cpp
	DEPENDENCIES
		sourcepp::vtfpp
)

radiant_add_module(imagepvr
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/imagepvr/imagepvr.cpp
		${PROJECT_SOURCE_DIR}/modules/imagepvr/pvr.cpp
)

radiant_add_module(imagehl
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/imagehl/hlw.cpp
		${PROJECT_SOURCE_DIR}/modules/imagehl/imagehl.cpp
		${PROJECT_SOURCE_DIR}/modules/imagehl/mip.cpp
		${PROJECT_SOURCE_DIR}/modules/imagehl/sprite.cpp
)

radiant_add_module(imageq2
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/imageq2/imageq2.cpp
		${PROJECT_SOURCE_DIR}/modules/imageq2/wal.cpp
		${PROJECT_SOURCE_DIR}/modules/imageq2/wal32.cpp
)

if(RADIANT_USE_ASSIMP)
	radiant_add_module(assmodel
		SOURCES
			${PROJECT_SOURCE_DIR}/modules/assmodel/mdlimage.cpp
			${PROJECT_SOURCE_DIR}/modules/assmodel/model.cpp
			${PROJECT_SOURCE_DIR}/modules/assmodel/plugin.cpp
		DEPENDENCIES
			Qt6::Core
			Qt6::Gui
			Qt6::Widgets
			Qt6::Svg
			Qt6::OpenGL
			Qt6::OpenGLWidgets
			assimp
	)
endif()

radiant_add_module(model
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/model/ghoul2.cpp
		${PROJECT_SOURCE_DIR}/modules/model/md2.cpp
		${PROJECT_SOURCE_DIR}/modules/model/md3.cpp
		${PROJECT_SOURCE_DIR}/modules/model/md3normals.cpp
		${PROJECT_SOURCE_DIR}/modules/model/md5.cpp
		${PROJECT_SOURCE_DIR}/modules/model/mdc.cpp
		${PROJECT_SOURCE_DIR}/modules/model/mdl.cpp
		${PROJECT_SOURCE_DIR}/modules/model/sourcemdl.cpp
		${PROJECT_SOURCE_DIR}/modules/model/mdlformat.cpp
		${PROJECT_SOURCE_DIR}/modules/model/mdlimage.cpp
		${PROJECT_SOURCE_DIR}/modules/model/mdlnormals.cpp
		${PROJECT_SOURCE_DIR}/modules/model/model.cpp
		${PROJECT_SOURCE_DIR}/modules/model/plugin.cpp
	DEPENDENCIES
		Qt6::Core
		Qt6::Gui
		Qt6::Widgets
		Qt6::Svg
		Qt6::OpenGL
		Qt6::OpenGLWidgets
		sourcepp::mdlpp
)

radiant_add_module(mapq3
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/mapq3/parse.cpp
		${PROJECT_SOURCE_DIR}/modules/mapq3/plugin.cpp
		${PROJECT_SOURCE_DIR}/modules/mapq3/write.cpp
)

radiant_add_module(mapvmf
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/mapvmf/plugin.cpp
	DEPENDENCIES
		sourcepp::kvpp
)

radiant_add_module(mapxml
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/mapxml/plugin.cpp
		${PROJECT_SOURCE_DIR}/modules/mapxml/xmlparse.cpp
		${PROJECT_SOURCE_DIR}/modules/mapxml/xmlwrite.cpp
	DEPENDENCIES
		LibXml2::LibXml2
)

radiant_add_module(shaders
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/shaders/plugin.cpp
		${PROJECT_SOURCE_DIR}/modules/shaders/shaders.cpp
	DEPENDENCIES
		wiferadiant-library-commandlib
		LibXml2::LibXml2
		sourcepp::kvpp
)

radiant_add_module(vfspk3
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/vfspk3/archive.cpp
		${PROJECT_SOURCE_DIR}/modules/vfspk3/vfs.cpp
		${PROJECT_SOURCE_DIR}/modules/vfspk3/vfspk3.cpp
	DEPENDENCIES
		LibXml2::LibXml2
		wiferadiant-library-filematch
)
