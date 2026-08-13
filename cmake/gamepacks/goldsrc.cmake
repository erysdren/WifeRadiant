if(NOT RADIANT_SUPPORT_GOLDSRC)
	return()
endif()

# Half-Life
radiant_add_gamepack(hl
	WRITE_DEFAULT_KEYVALUES
	SUPPORT_WADS
	GAME_TYPE "hl"
	TITLE "Half-Life"
	GAMEDIR "valve"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Half-Life/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Half-Life/"
	EXECUTABLE_WIN32 "hl.exe"
	EXECUTABLE_LINUX "hl.sh"
	EXECUTABLE_MACOS "hl.sh"
	ENTITIES_FILENAME "halflife.fgd"
	SHADER_TYPE "quake3"
	SHADER_PATH "scripts"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	BUILD_MENU_FILENAME "default_build_menu_goldsrc_ericwtools.xml"
	ARCHIVE_TYPES "pak" "wad"
	TEXTURE_TYPES "hlw" "spr" "mdl"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav"
	MAP_TYPES "maphl"
	BRUSH_TYPES "halflife"
	PATCH_TYPES "quake3"
	SHADER_CAULK "null"
	SHADER_NODRAW "null"
)

# Half-Life: Blue Shift
radiant_add_gamepack(bshift
	WRITE_DEFAULT_KEYVALUES
	SUPPORT_WADS
	GAME_TYPE "hl"
	HAS_BASEGAME
	BASE_TITLE "Half-Life"
	BASE_GAMEDIR "valve"
	TITLE "Half-Life: Blue Shift"
	KNOWN_GAMEDIRS "bshift"
	KNOWN_TITLES "Half-Life: Blue Shift"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Half-Life/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Half-Life/"
	EXECUTABLE_WIN32 "hl.exe"
	EXECUTABLE_LINUX "hl.sh"
	EXECUTABLE_MACOS "hl.sh"
	ENTITIES_FILENAME "bshift.fgd"
	SHADER_TYPE "quake3"
	SHADER_PATH "scripts"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	BUILD_MENU_FILENAME "default_build_menu_goldsrc_ericwtools.xml"
	ARCHIVE_TYPES "pak" "wad"
	TEXTURE_TYPES "hlw" "spr" "mdl"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav"
	MAP_TYPES "maphl"
	BRUSH_TYPES "halflife"
	PATCH_TYPES "quake3"
	SHADER_CAULK "null"
	SHADER_NODRAW "null"
)

# Half-Life: Opposing Force
radiant_add_gamepack(gearbox
	WRITE_DEFAULT_KEYVALUES
	SUPPORT_WADS
	GAME_TYPE "hl"
	HAS_BASEGAME
	BASE_TITLE "Half-Life"
	BASE_GAMEDIR "valve"
	TITLE "Half-Life: Opposing Force"
	KNOWN_GAMEDIRS "gearbox"
	KNOWN_TITLES "Half-Life: Opposing Force"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Half-Life/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Half-Life/"
	EXECUTABLE_WIN32 "hl.exe"
	EXECUTABLE_LINUX "hl.sh"
	EXECUTABLE_MACOS "hl.sh"
	ENTITIES_FILENAME "halflife-op4.fgd"
	SHADER_TYPE "quake3"
	SHADER_PATH "scripts"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	BUILD_MENU_FILENAME "default_build_menu_goldsrc_ericwtools.xml"
	ARCHIVE_TYPES "pak" "wad"
	TEXTURE_TYPES "hlw" "spr" "mdl"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav"
	MAP_TYPES "maphl"
	BRUSH_TYPES "halflife"
	PATCH_TYPES "quake3"
	SHADER_CAULK "null"
	SHADER_NODRAW "null"
)

# Team Fortress Classic
radiant_add_gamepack(tfc
	WRITE_DEFAULT_KEYVALUES
	SUPPORT_WADS
	GAME_TYPE "hl"
	HAS_BASEGAME
	BASE_TITLE "Half-Life"
	BASE_GAMEDIR "valve"
	TITLE "Team Fortress Classic"
	KNOWN_GAMEDIRS "tfc"
	KNOWN_TITLES "Team Fortress Classic"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Half-Life/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Half-Life/"
	EXECUTABLE_WIN32 "hl.exe"
	EXECUTABLE_LINUX "hl.sh"
	EXECUTABLE_MACOS "hl.sh"
	ENTITIES_FILENAME "tfc.fgd"
	SHADER_TYPE "quake3"
	SHADER_PATH "scripts"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	BUILD_MENU_FILENAME "default_build_menu_goldsrc_ericwtools.xml"
	ARCHIVE_TYPES "pak" "wad"
	TEXTURE_TYPES "hlw" "spr" "mdl"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav"
	MAP_TYPES "maphl"
	BRUSH_TYPES "halflife"
	PATCH_TYPES "quake3"
	SHADER_CAULK "null"
	SHADER_NODRAW "null"
)

# Day of Defeat
radiant_add_gamepack(dod
	WRITE_DEFAULT_KEYVALUES
	SUPPORT_WADS
	GAME_TYPE "hl"
	HAS_BASEGAME
	BASE_TITLE "Half-Life"
	BASE_GAMEDIR "valve"
	TITLE "Day of Defeat"
	KNOWN_GAMEDIRS "dod"
	KNOWN_TITLES "Day of Defeat"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Half-Life/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Half-Life/"
	EXECUTABLE_WIN32 "hl.exe"
	EXECUTABLE_LINUX "hl.sh"
	EXECUTABLE_MACOS "hl.sh"
	ENTITIES_FILENAME "dod.fgd"
	SHADER_TYPE "quake3"
	SHADER_PATH "scripts"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	BUILD_MENU_FILENAME "default_build_menu_goldsrc_ericwtools.xml"
	ARCHIVE_TYPES "pak" "wad"
	TEXTURE_TYPES "hlw" "spr" "mdl"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav"
	MAP_TYPES "maphl"
	BRUSH_TYPES "halflife"
	PATCH_TYPES "quake3"
	SHADER_CAULK "null"
	SHADER_NODRAW "null"
)

# Counter-Strike
radiant_add_gamepack(cstrike
	WRITE_DEFAULT_KEYVALUES
	SUPPORT_WADS
	GAME_TYPE "hl"
	HAS_BASEGAME
	BASE_TITLE "Half-Life"
	BASE_GAMEDIR "valve"
	TITLE "Counter-Strike"
	KNOWN_GAMEDIRS "cstrike"
	KNOWN_TITLES "Counter-Strike"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Half-Life/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Half-Life/"
	EXECUTABLE_WIN32 "hl.exe"
	EXECUTABLE_LINUX "hl.sh"
	EXECUTABLE_MACOS "hl.sh"
	ENTITIES_FILENAME "cstrike.fgd"
	SHADER_TYPE "quake3"
	SHADER_PATH "scripts"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	BUILD_MENU_FILENAME "default_build_menu_goldsrc_ericwtools.xml"
	ARCHIVE_TYPES "pak" "wad"
	TEXTURE_TYPES "hlw" "spr" "mdl"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav"
	MAP_TYPES "maphl"
	BRUSH_TYPES "halflife"
	PATCH_TYPES "quake3"
	SHADER_CAULK "null"
	SHADER_NODRAW "null"
)

# Ricochet
radiant_add_gamepack(ricochet
	WRITE_DEFAULT_KEYVALUES
	SUPPORT_WADS
	GAME_TYPE "hl"
	HAS_BASEGAME
	BASE_TITLE "Half-Life"
	BASE_GAMEDIR "valve"
	TITLE "Ricochet"
	KNOWN_GAMEDIRS "ricochet"
	KNOWN_TITLES "Ricochet"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Half-Life/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Half-Life/"
	EXECUTABLE_WIN32 "hl.exe"
	EXECUTABLE_LINUX "hl.sh"
	EXECUTABLE_MACOS "hl.sh"
	ENTITIES_FILENAME "ricochet.fgd"
	SHADER_TYPE "quake3"
	SHADER_PATH "scripts"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	BUILD_MENU_FILENAME "default_build_menu_goldsrc_ericwtools.xml"
	ARCHIVE_TYPES "pak" "wad"
	TEXTURE_TYPES "hlw" "spr" "mdl"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav"
	MAP_TYPES "maphl"
	BRUSH_TYPES "halflife"
	PATCH_TYPES "quake3"
	SHADER_CAULK "null"
	SHADER_NODRAW "null"
)

# Gunman Chronicles
radiant_add_gamepack(rewolf
	WRITE_DEFAULT_KEYVALUES
	SUPPORT_WADS
	GAME_TYPE "hl"
	TITLE "Gunman Chronicles"
	GAMEDIR "rewolf"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Half-Life/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Half-Life/"
	EXECUTABLE_WIN32 "hl.exe"
	EXECUTABLE_LINUX "hl.sh"
	EXECUTABLE_MACOS "hl.sh"
	ENTITIES_FILENAME "gunman.fgd"
	SHADER_TYPE "quake3"
	SHADER_PATH "scripts"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	BUILD_MENU_FILENAME "default_build_menu_goldsrc_ericwtools.xml"
	ARCHIVE_TYPES "pak" "wad"
	TEXTURE_TYPES "hlw" "spr" "mdl"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav"
	MAP_TYPES "maphl"
	BRUSH_TYPES "halflife"
	PATCH_TYPES "quake3"
	SHADER_CAULK "null"
	SHADER_NODRAW "null"
)
