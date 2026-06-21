
function(radiant_add_gamepack name)
	cmake_parse_arguments(PARSE_ARGV 1 ARG
		"HAS_BASEGAME;USE_NEW_OUTPUT_SEPARATOR;SUPPORT_PATCHES;SUPPORT_OUTPUTS;SUPPORT_LIGHTMAP_SCALE;SUPPORT_WADS"
		"BUILD_MENU_FILENAME;MAP_BACKUP_EXTENSION;MAP_EXTENSION;SHADER_PATH;DEFAULT_SCALE;DEFAULT_LIGHTMAP_SCALE;ENTITY_CLASS_TYPES;ENTITY_CLASS;SHADER_TYPE;GAME_TYPE;ENTITIES_FILENAME;ENTITIES;BASE_TITLE;BASE_GAMEDIR;TITLE;GAMEDIR;PATH_WIN32;PATH_LINUX;PATH_MACOS;EXECUTABLE_WIN32;EXECUTABLE_LINUX;EXECUTABLE_MACOS"
		"ARCHIVE_TYPES;TEXTURE_TYPES;MODEL_TYPES;SOUND_TYPES;MAP_TYPES;BRUSH_TYPES;PATCH_TYPES;KNOWN_TITLES;KNOWN_GAMEDIRS"
	)
	file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/install/gamepacks/games/")
	file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/install/gamepacks/${name}.game/")
	if(EXISTS "${PROJECT_SOURCE_DIR}/cmake/gamepacks/${name}.game/")
		file(COPY "${PROJECT_SOURCE_DIR}/cmake/gamepacks/${name}.game/" DESTINATION "${PROJECT_SOURCE_DIR}/install/gamepacks/${name}.game/")
	endif()
	if(ARG_BUILD_MENU_FILENAME)
		file(COPY "${PROJECT_SOURCE_DIR}/cmake/gamepacks/${ARG_BUILD_MENU_FILENAME}" DESTINATION "${PROJECT_SOURCE_DIR}/install/gamepacks/${name}.game/")
		file(RENAME "${PROJECT_SOURCE_DIR}/install/gamepacks/${name}.game/${ARG_BUILD_MENU_FILENAME}" "${PROJECT_SOURCE_DIR}/install/gamepacks/${name}.game/default_build_menu.xml")
	endif()
	set(target gamepack_${name})
	add_custom_target(${target})
	if(ARG_HAS_BASEGAME)
		set_property(TARGET ${target} PROPERTY HAS_BASEGAME 1)
		set_property(TARGET ${target} PROPERTY BASE_GAMEDIR ${ARG_BASE_GAMEDIR})
		set_property(TARGET ${target} PROPERTY BASE_TITLE ${ARG_BASE_TITLE})
		set_property(TARGET ${target} PROPERTY KNOWN_GAMEDIRS ${ARG_KNOWN_GAMEDIRS})
		set_property(TARGET ${target} PROPERTY KNOWN_TITLES ${ARG_KNOWN_TITLES})
	else()
		set_property(TARGET ${target} PROPERTY HAS_BASEGAME 0)
	endif()
	if(ARG_USE_NEW_OUTPUT_SEPARATOR)
		set_property(TARGET ${target} PROPERTY USE_NEW_OUTPUT_SEPARATOR 1)
	else()
		set_property(TARGET ${target} PROPERTY USE_NEW_OUTPUT_SEPARATOR 0)
	endif()
	if(ARG_SUPPORT_PATCHES) # note: reversed
		set_property(TARGET ${target} PROPERTY SUPPORT_PATCHES 0)
	else()
		set_property(TARGET ${target} PROPERTY SUPPORT_PATCHES 1)
	endif()
	if(ARG_SUPPORT_OUTPUTS) # note: reversed
		set_property(TARGET ${target} PROPERTY SUPPORT_OUTPUTS 0)
	else()
		set_property(TARGET ${target} PROPERTY SUPPORT_OUTPUTS 1)
	endif()
	if(ARG_SUPPORT_LIGHTMAP_SCALE) # note: reversed
		set_property(TARGET ${target} PROPERTY SUPPORT_LIGHTMAP_SCALE 0)
	else()
		set_property(TARGET ${target} PROPERTY SUPPORT_LIGHTMAP_SCALE 1)
	endif()
	if(ARG_SUPPORT_WADS)
		set_property(TARGET ${target} PROPERTY SUPPORT_WADS 1)
	else()
		set_property(TARGET ${target} PROPERTY SUPPORT_WADS 0)
	endif()
	set_property(TARGET ${target} PROPERTY GAME_TYPE ${ARG_GAME_TYPE})
	set_property(TARGET ${target} PROPERTY TITLE ${ARG_TITLE})
	set_property(TARGET ${target} PROPERTY GAMEDIR ${ARG_GAMEDIR})
	set_property(TARGET ${target} PROPERTY PATH_WIN32 ${ARG_PATH_WIN32})
	set_property(TARGET ${target} PROPERTY PATH_LINUX ${ARG_PATH_LINUX})
	set_property(TARGET ${target} PROPERTY PATH_MACOS ${ARG_PATH_MACOS})
	set_property(TARGET ${target} PROPERTY EXECUTABLE_WIN32 ${ARG_EXECUTABLE_WIN32})
	set_property(TARGET ${target} PROPERTY EXECUTABLE_LINUX ${ARG_EXECUTABLE_LINUX})
	set_property(TARGET ${target} PROPERTY EXECUTABLE_MACOS ${ARG_EXECUTABLE_MACOS})
	set_property(TARGET ${target} PROPERTY ENTITIES ${ARG_ENTITIES})
	set_property(TARGET ${target} PROPERTY ENTITIES_FILENAME ${ARG_ENTITIES_FILENAME})
	if(ARG_DEFAULT_SCALE)
		set_property(TARGET ${target} PROPERTY DEFAULT_SCALE ${ARG_DEFAULT_SCALE})
	else()
		set_property(TARGET ${target} PROPERTY DEFAULT_SCALE "1.0")
	endif()
	if(ARG_DEFAULT_LIGHTMAP_SCALE)
		set_property(TARGET ${target} PROPERTY DEFAULT_LIGHTMAP_SCALE ${ARG_DEFAULT_LIGHTMAP_SCALE})
	else()
		set_property(TARGET ${target} PROPERTY DEFAULT_LIGHTMAP_SCALE "16")
	endif()
	if(ARG_MAP_EXTENSION)
		set_property(TARGET ${target} PROPERTY MAP_EXTENSION ${ARG_MAP_EXTENSION})
	else()
		set_property(TARGET ${target} PROPERTY MAP_EXTENSION ".map")
	endif()
	if(ARG_MAP_BACKUP_EXTENSION)
		set_property(TARGET ${target} PROPERTY MAP_BACKUP_EXTENSION ${ARG_MAP_BACKUP_EXTENSION})
	else()
		set_property(TARGET ${target} PROPERTY MAP_BACKUP_EXTENSION ".bak")
	endif()
	set_property(TARGET ${target} PROPERTY ENTITY_CLASS_TYPES ${ARG_ENTITY_CLASS_TYPES})
	set_property(TARGET ${target} PROPERTY ENTITY_CLASS ${ARG_ENTITY_CLASS})
	set_property(TARGET ${target} PROPERTY SHADER_TYPE ${ARG_SHADER_TYPE})
	set_property(TARGET ${target} PROPERTY SHADER_PATH ${ARG_SHADER_PATH})
	set_property(TARGET ${target} PROPERTY ARCHIVE_TYPES ${ARG_ARCHIVE_TYPES})
	set_property(TARGET ${target} PROPERTY TEXTURE_TYPES ${ARG_TEXTURE_TYPES})
	set_property(TARGET ${target} PROPERTY MODEL_TYPES ${ARG_MODEL_TYPES})
	set_property(TARGET ${target} PROPERTY SOUND_TYPES ${ARG_SOUND_TYPES})
	set_property(TARGET ${target} PROPERTY MAP_TYPES ${ARG_MAP_TYPES})
	set_property(TARGET ${target} PROPERTY BRUSH_TYPES ${ARG_BRUSH_TYPES})
	set_property(TARGET ${target} PROPERTY PATCH_TYPES ${ARG_PATCH_TYPES})
	file(GENERATE
		OUTPUT "${PROJECT_SOURCE_DIR}/install/gamepacks/games/${name}.game"
		CONTENT
[[<?xml version="1.0"?>
<game
  type="$<TARGET_PROPERTY:GAME_TYPE>"
  index="1"
  name="$<TARGET_PROPERTY:TITLE>"
  enginepath_win32="$<TARGET_PROPERTY:PATH_WIN32>"
  enginepath_linux="$<TARGET_PROPERTY:PATH_LINUX>"
  enginepath_macos="$<TARGET_PROPERTY:PATH_MACOS>"
  engine_win32="$<TARGET_PROPERTY:EXECUTABLE_WIN32>"
  engine_linux="$<TARGET_PROPERTY:EXECUTABLE_LINUX>"
  engine_macos="$<TARGET_PROPERTY:EXECUTABLE_MACOS>"
  basegame="$<IF:$<TARGET_PROPERTY:HAS_BASEGAME>,$<TARGET_PROPERTY:BASE_GAMEDIR>,$<TARGET_PROPERTY:GAMEDIR>>"
  basegamename="$<IF:$<TARGET_PROPERTY:HAS_BASEGAME>,$<TARGET_PROPERTY:BASE_TITLE>,$<TARGET_PROPERTY:TITLE>>"
  knowngames="$<LIST:JOIN,$<TARGET_PROPERTY:KNOWN_GAMEDIRS>, >"
  knowngamenames="$<LIST:JOIN,$<TARGET_PROPERTY:KNOWN_TITLES>,$<SEMICOLON>>"
  defaultgame="$<IF:$<TARGET_PROPERTY:HAS_BASEGAME>,$<LIST:GET,$<TARGET_PROPERTY:KNOWN_GAMEDIRS>,-1>,$<TARGET_PROPERTY:GAMEDIR>>"
  shaderpath="$<TARGET_PROPERTY:SHADER_PATH>"
  archivetypes="$<LIST:JOIN,$<TARGET_PROPERTY:ARCHIVE_TYPES>, >"
  texturetypes="$<LIST:JOIN,$<TARGET_PROPERTY:TEXTURE_TYPES>, >"
  modeltypes="$<LIST:JOIN,$<TARGET_PROPERTY:MODEL_TYPES>, >"
  soundtypes="$<LIST:JOIN,$<TARGET_PROPERTY:SOUND_TYPES>, >"
  maptypes="$<LIST:JOIN,$<TARGET_PROPERTY:MAP_TYPES>, >"
  shaders="$<TARGET_PROPERTY:SHADER_TYPE>"
  entityclass="$<TARGET_PROPERTY:ENTITY_CLASS>"
  entityclasstype="$<LIST:JOIN,$<TARGET_PROPERTY:ENTITY_CLASS_TYPES>, >"
  entities="$<TARGET_PROPERTY:ENTITIES>"
  entitiesfilename="$<TARGET_PROPERTY:ENTITIES_FILENAME>"
  brushtypes="$<LIST:JOIN,$<TARGET_PROPERTY:BRUSH_TYPES>, >"
  patchtypes="$<LIST:JOIN,$<TARGET_PROPERTY:PATCH_TYPES>, >"
  default_scale="$<TARGET_PROPERTY:DEFAULT_SCALE>"
  default_lightmapscale="$<TARGET_PROPERTY:DEFAULT_LIGHTMAP_SCALE>"
  no_patch="$<TARGET_PROPERTY:SUPPORT_PATCHES>"
  no_plugins="0"
  no_bsp_monitor="1"
  no_autocaulk="1"
  no_outputs="$<TARGET_PROPERTY:SUPPORT_OUTPUTS>"
  no_lightmapscale="$<TARGET_PROPERTY:SUPPORT_LIGHTMAP_SCALE>"
  show_wads="$<TARGET_PROPERTY:SUPPORT_WADS>"
  mapextension="$<TARGET_PROPERTY:MAP_EXTENSION>"
  mapbackupextension="$<TARGET_PROPERTY:MAP_BACKUP_EXTENSION>"
  use_new_output_separator="$<TARGET_PROPERTY:USE_NEW_OUTPUT_SEPARATOR>"
/>
]]
	TARGET ${target}
	)
endfunction()

# source engine gamepacks
include(gamepacks/source)

# goldsrc engine gamepacks
include(gamepacks/goldsrc)

# idtech3 gamepacks
include(gamepacks/idtech3)

# put your custom gamepacks in here:
# cmake/gamepacks/user.cmake
include(gamepacks/user OPTIONAL)
