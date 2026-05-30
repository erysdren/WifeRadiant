# modules are generally not optional and add support for image formats, model formats, shader formats, etc

function(add_module name)
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
			LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/install/modules
			RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/install/modules
			PREFIX ""
	)
	target_compile_definitions(${name} PRIVATE $<$<CONFIG:Debug>:_DEBUG> $<$<NOT:$<BOOL:${WIN32}>>:POSIX> $<$<BOOL:${WIN32}>:WIN32>)
	target_compile_options(${name} PRIVATE
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

add_module(archivepak
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/archivepak/archive.cpp
		${PROJECT_SOURCE_DIR}/modules/archivepak/pak.cpp
		${PROJECT_SOURCE_DIR}/modules/archivepak/plugin.cpp
)

if(RADIANT_SUPPORT_SOURCE)
	add_module(archivevpk
		SOURCES
			${PROJECT_SOURCE_DIR}/modules/archivevpk/archive.cpp
			${PROJECT_SOURCE_DIR}/modules/archivevpk/plugin.cpp
	)
	target_link_libraries(archivevpk PRIVATE sourcepp::vpkpp)
endif()

add_module(archivezip
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/archivezip/archive.cpp
		${PROJECT_SOURCE_DIR}/modules/archivezip/pkzip.cpp
		${PROJECT_SOURCE_DIR}/modules/archivezip/plugin.cpp
		${PROJECT_SOURCE_DIR}/modules/archivezip/zlibstream.cpp
)
target_link_libraries(archivezip PRIVATE ZLIB::ZLIB)

add_module(archivewad
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/archivewad/archive.cpp
		${PROJECT_SOURCE_DIR}/modules/archivewad/plugin.cpp
		${PROJECT_SOURCE_DIR}/modules/archivewad/wad.cpp
)

add_module(entity
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
)
target_link_libraries(entity PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::OpenGL Qt6::OpenGLWidgets)

add_module(image
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
)
target_link_libraries(image PRIVATE ddslib etclib crnlib webplib)

if(RADIANT_SUPPORT_SOURCE)
	add_module(imagevtf
		SOURCES
			${PROJECT_SOURCE_DIR}/modules/imagevtf/imagevtf.cpp
			${PROJECT_SOURCE_DIR}/modules/imagevtf/vtf.cpp
	)
	target_link_libraries(imagevtf PRIVATE sourcepp::vtfpp)
endif()

add_module(imagepvr
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/imagepvr/imagepvr.cpp
		${PROJECT_SOURCE_DIR}/modules/imagepvr/pvr.cpp
)

add_module(imagehl
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/imagehl/hlw.cpp
		${PROJECT_SOURCE_DIR}/modules/imagehl/imagehl.cpp
		${PROJECT_SOURCE_DIR}/modules/imagehl/mip.cpp
		${PROJECT_SOURCE_DIR}/modules/imagehl/sprite.cpp
)

add_module(imageq2
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/imageq2/imageq2.cpp
		${PROJECT_SOURCE_DIR}/modules/imageq2/wal.cpp
		${PROJECT_SOURCE_DIR}/modules/imageq2/wal32.cpp
)

if(RADIANT_USE_ASSIMP)
	add_module(assmodel
		SOURCES
			${PROJECT_SOURCE_DIR}/modules/assmodel/mdlimage.cpp
			${PROJECT_SOURCE_DIR}/modules/assmodel/model.cpp
			${PROJECT_SOURCE_DIR}/modules/assmodel/plugin.cpp
	)
	target_link_libraries(assmodel PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::OpenGL Qt6::OpenGLWidgets)
	target_link_libraries(assmodel PRIVATE assimp)
endif()

add_module(model
	SOURCES
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
)
target_link_libraries(model PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::OpenGL Qt6::OpenGLWidgets)
if(RADIANT_SUPPORT_SOURCE)
	target_link_libraries(model PRIVATE sourcepp::mdlpp)
else()
	target_compile_definitions(model PRIVATE NO_SOURCEMDL=1)
endif()

add_module(mapq3
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/mapq3/parse.cpp
		${PROJECT_SOURCE_DIR}/modules/mapq3/plugin.cpp
		${PROJECT_SOURCE_DIR}/modules/mapq3/write.cpp
)

if(RADIANT_SUPPORT_SOURCE)
	add_module(mapvmf
		SOURCES
			${PROJECT_SOURCE_DIR}/modules/mapvmf/plugin.cpp
	)
	target_link_libraries(mapvmf PRIVATE sourcepp::kvpp)
endif()

add_module(mapxml
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/mapxml/plugin.cpp
		${PROJECT_SOURCE_DIR}/modules/mapxml/xmlparse.cpp
		${PROJECT_SOURCE_DIR}/modules/mapxml/xmlwrite.cpp
)
target_link_libraries(mapxml PRIVATE LibXml2::LibXml2)

add_module(shaders
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/shaders/plugin.cpp
		${PROJECT_SOURCE_DIR}/modules/shaders/shaders.cpp
)
target_link_libraries(shaders PRIVATE commandlib)
target_link_libraries(shaders PRIVATE LibXml2::LibXml2)
if(RADIANT_SUPPORT_SOURCE)
	target_link_libraries(shaders PRIVATE sourcepp::kvpp)
else()
	target_compile_definitions(shaders PRIVATE NO_SOURCEVMT=1)
endif()

add_module(vfspk3
	SOURCES
		${PROJECT_SOURCE_DIR}/modules/vfspk3/archive.cpp
		${PROJECT_SOURCE_DIR}/modules/vfspk3/vfs.cpp
		${PROJECT_SOURCE_DIR}/modules/vfspk3/vfspk3.cpp
)
target_link_libraries(vfspk3 PRIVATE LibXml2::LibXml2)
target_link_libraries(vfspk3 PRIVATE filematch)
