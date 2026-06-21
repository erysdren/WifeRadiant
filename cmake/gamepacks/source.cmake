if(NOT RADIANT_SUPPORT_SOURCE)
	return()
endif()

# Counter-Strike: Source
radiant_add_gamepack(css
	SUPPORT_LIGHTMAP_SCALE
	SUPPORT_OUTPUTS
	GAME_TYPE "source"
	HAS_BASEGAME
	BASE_TITLE "Half-Life 2"
	BASE_GAMEDIR "hl2"
	TITLE "Counter-Strike: Source"
	KNOWN_GAMEDIRS "cstrike"
	KNOWN_TITLES "Counter-Strike: Source"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Counter-Strike Source/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Counter-Strike Source/"
	EXECUTABLE_WIN32 "cstrike.exe"
	EXECUTABLE_LINUX "cstrike.sh"
	EXECUTABLE_MACOS "cstrike.sh"
	ENTITIES_FILENAME "cstrike.fgd"
	SHADER_TYPE "source"
	SHADER_PATH "materials"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	DEFAULT_SCALE "0.25"
	DEFAULT_LIGHTMAP_SCALE "16"
	MAP_EXTENSION ".vmf"
	MAP_BACKUP_EXTENSION ".vmx"
	BUILD_MENU_FILENAME "default_build_menu_source.xml"
	ARCHIVE_TYPES "vpk" "gma" "gcf"
	TEXTURE_TYPES "vtf" "tth"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav" "mp3"
	MAP_TYPES "mapvmf"
	BRUSH_TYPES "source"
	PATCH_TYPES "quake3"
)

# D.I.P.R.I.P. Warm Up
radiant_add_gamepack(diprip
	SUPPORT_LIGHTMAP_SCALE
	SUPPORT_OUTPUTS
	GAME_TYPE "source"
	HAS_BASEGAME
	BASE_TITLE "Source SDK Base 2007"
	BASE_GAMEDIR "vpks"
	TITLE "D.I.P.R.I.P. Warm Up"
	KNOWN_GAMEDIRS "diprip"
	KNOWN_TITLES "D.I.P.R.I.P. Warm Up"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Source SDK Base 2007/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Source SDK Base 2007/"
	EXECUTABLE_WIN32 "hl2.exe"
	EXECUTABLE_LINUX "hl2.sh"
	EXECUTABLE_MACOS "hl2.sh"
	ENTITIES_FILENAME "diprip.fgd"
	SHADER_TYPE "source"
	SHADER_PATH "materials"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	DEFAULT_SCALE "0.25"
	DEFAULT_LIGHTMAP_SCALE "16"
	MAP_EXTENSION ".vmf"
	MAP_BACKUP_EXTENSION ".vmx"
	BUILD_MENU_FILENAME "default_build_menu_source.xml"
	ARCHIVE_TYPES "vpk" "gma" "gcf"
	TEXTURE_TYPES "vtf" "tth"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav" "mp3"
	MAP_TYPES "mapvmf"
	BRUSH_TYPES "source"
	PATCH_TYPES "quake3"
)

# Day of Defeat: Source
radiant_add_gamepack(dods
	SUPPORT_LIGHTMAP_SCALE
	SUPPORT_OUTPUTS
	GAME_TYPE "source"
	HAS_BASEGAME
	BASE_TITLE "Half-Life 2"
	BASE_GAMEDIR "hl2"
	TITLE "Day of Defeat: Source"
	KNOWN_GAMEDIRS "dod"
	KNOWN_TITLES "Day of Defeat: Source"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Day of Defeat Source/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Day of Defeat Source/"
	EXECUTABLE_WIN32 "dod.exe"
	EXECUTABLE_LINUX "dod.sh"
	EXECUTABLE_MACOS "dod.sh"
	ENTITIES_FILENAME "dod.fgd"
	SHADER_TYPE "source"
	SHADER_PATH "materials"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	DEFAULT_SCALE "0.25"
	DEFAULT_LIGHTMAP_SCALE "16"
	MAP_EXTENSION ".vmf"
	MAP_BACKUP_EXTENSION ".vmx"
	BUILD_MENU_FILENAME "default_build_menu_source.xml"
	ARCHIVE_TYPES "vpk" "gma" "gcf"
	TEXTURE_TYPES "vtf" "tth"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav" "mp3"
	MAP_TYPES "mapvmf"
	BRUSH_TYPES "source"
	PATCH_TYPES "quake3"
)

# Garry's Mod
radiant_add_gamepack(garrysmod
	SUPPORT_LIGHTMAP_SCALE
	SUPPORT_OUTPUTS
	GAME_TYPE "source"
	HAS_BASEGAME
	BASE_TITLE "Source Engine Base Content"
	BASE_GAMEDIR "sourceengine"
	TITLE "Garry's Mod"
	KNOWN_GAMEDIRS "garrysmod"
	KNOWN_TITLES "Garry's Mod"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/GarrysMod/"
	PATH_LINUX "~/.steam/steam/steamapps/common/GarrysMod/"
	EXECUTABLE_WIN32 "hl2.exe"
	EXECUTABLE_LINUX "hl2.sh"
	EXECUTABLE_MACOS "hl2.sh"
	ENTITIES_FILENAME "garrysmod.fgd"
	SHADER_TYPE "source"
	SHADER_PATH "materials"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	DEFAULT_SCALE "0.25"
	DEFAULT_LIGHTMAP_SCALE "16"
	MAP_EXTENSION ".vmf"
	MAP_BACKUP_EXTENSION ".vmx"
	BUILD_MENU_FILENAME "default_build_menu_source.xml"
	ARCHIVE_TYPES "vpk" "gma" "gcf"
	TEXTURE_TYPES "vtf" "tth"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav" "mp3"
	MAP_TYPES "mapvmf"
	BRUSH_TYPES "source"
	PATCH_TYPES "quake3"
)

# Half-Life 2
radiant_add_gamepack(hl2
	SUPPORT_LIGHTMAP_SCALE
	SUPPORT_OUTPUTS
	GAME_TYPE "source"
	TITLE "Half-Life 2"
	GAMEDIR "hl2"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Half-Life 2/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Half-Life 2/"
	EXECUTABLE_WIN32 "hl2.exe"
	EXECUTABLE_LINUX "hl2.sh"
	EXECUTABLE_MACOS "hl2.sh"
	ENTITIES_FILENAME "halflife2.fgd"
	SHADER_TYPE "source"
	SHADER_PATH "materials"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	DEFAULT_SCALE "0.25"
	DEFAULT_LIGHTMAP_SCALE "16"
	MAP_EXTENSION ".vmf"
	MAP_BACKUP_EXTENSION ".vmx"
	BUILD_MENU_FILENAME "default_build_menu_source.xml"
	ARCHIVE_TYPES "vpk" "gma" "gcf"
	TEXTURE_TYPES "vtf" "tth"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav" "mp3"
	MAP_TYPES "mapvmf"
	BRUSH_TYPES "source"
	PATCH_TYPES "quake3"
)

# Half-Life 2: Episode One
radiant_add_gamepack(episodic
	SUPPORT_LIGHTMAP_SCALE
	SUPPORT_OUTPUTS
	GAME_TYPE "source"
	HAS_BASEGAME
	BASE_TITLE "Half-Life 2"
	BASE_GAMEDIR "hl2"
	TITLE "Half-Life 2: Episode One"
	KNOWN_GAMEDIRS "episodic"
	KNOWN_TITLES "Half-Life 2: Episode One"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Half-Life 2/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Half-Life 2/"
	EXECUTABLE_WIN32 "hl2.exe"
	EXECUTABLE_LINUX "hl2.sh"
	EXECUTABLE_MACOS "hl2.sh"
	ENTITIES_FILENAME "halflife2.fgd"
	SHADER_TYPE "source"
	SHADER_PATH "materials"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	DEFAULT_SCALE "0.25"
	DEFAULT_LIGHTMAP_SCALE "16"
	MAP_EXTENSION ".vmf"
	MAP_BACKUP_EXTENSION ".vmx"
	BUILD_MENU_FILENAME "default_build_menu_source.xml"
	ARCHIVE_TYPES "vpk" "gma" "gcf"
	TEXTURE_TYPES "vtf" "tth"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav" "mp3"
	MAP_TYPES "mapvmf"
	BRUSH_TYPES "source"
	PATCH_TYPES "quake3"
)

# Half-Life 2: Episode Two
radiant_add_gamepack(ep2
	SUPPORT_LIGHTMAP_SCALE
	SUPPORT_OUTPUTS
	GAME_TYPE "source"
	HAS_BASEGAME
	BASE_TITLE "Half-Life 2"
	BASE_GAMEDIR "hl2"
	TITLE "Half-Life 2: Episode Two"
	KNOWN_GAMEDIRS "ep2"
	KNOWN_TITLES "Half-Life 2: Episode Two"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Half-Life 2/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Half-Life 2/"
	EXECUTABLE_WIN32 "hl2.exe"
	EXECUTABLE_LINUX "hl2.sh"
	EXECUTABLE_MACOS "hl2.sh"
	ENTITIES_FILENAME "halflife2.fgd"
	SHADER_TYPE "source"
	SHADER_PATH "materials"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	DEFAULT_SCALE "0.25"
	DEFAULT_LIGHTMAP_SCALE "16"
	MAP_EXTENSION ".vmf"
	MAP_BACKUP_EXTENSION ".vmx"
	BUILD_MENU_FILENAME "default_build_menu_source.xml"
	ARCHIVE_TYPES "vpk" "gma" "gcf"
	TEXTURE_TYPES "vtf" "tth"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav" "mp3"
	MAP_TYPES "mapvmf"
	BRUSH_TYPES "source"
	PATCH_TYPES "quake3"
)

# Half-Life 2: Lost Coast
radiant_add_gamepack(lostcoast
	SUPPORT_LIGHTMAP_SCALE
	SUPPORT_OUTPUTS
	GAME_TYPE "source"
	HAS_BASEGAME
	BASE_TITLE "Half-Life 2"
	BASE_GAMEDIR "hl2"
	TITLE "Half-Life 2: Lost Coast"
	KNOWN_GAMEDIRS "lostcoast"
	KNOWN_TITLES "Half-Life 2: Lost Coast"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Half-Life 2/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Half-Life 2/"
	EXECUTABLE_WIN32 "hl2.exe"
	EXECUTABLE_LINUX "hl2.sh"
	EXECUTABLE_MACOS "hl2.sh"
	ENTITIES_FILENAME "halflife2.fgd"
	SHADER_TYPE "source"
	SHADER_PATH "materials"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	DEFAULT_SCALE "0.25"
	DEFAULT_LIGHTMAP_SCALE "16"
	MAP_EXTENSION ".vmf"
	MAP_BACKUP_EXTENSION ".vmx"
	BUILD_MENU_FILENAME "default_build_menu_source.xml"
	ARCHIVE_TYPES "vpk" "gma" "gcf"
	TEXTURE_TYPES "vtf" "tth"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav" "mp3"
	MAP_TYPES "mapvmf"
	BRUSH_TYPES "source"
	PATCH_TYPES "quake3"
)

# Half-Life 2: Deathmatch
radiant_add_gamepack(hl2mp
	SUPPORT_LIGHTMAP_SCALE
	SUPPORT_OUTPUTS
	GAME_TYPE "source"
	HAS_BASEGAME
	BASE_TITLE "Half-Life 2"
	BASE_GAMEDIR "hl2"
	TITLE "Half-Life 2: Deathmatch"
	KNOWN_GAMEDIRS "hl2mp"
	KNOWN_TITLES "Half-Life 2: Deathmatch"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Half-Life 2 Deathmatch/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Half-Life 2 Deathmatch/"
	EXECUTABLE_WIN32 "hl2mp.exe"
	EXECUTABLE_LINUX "hl2mp.sh"
	EXECUTABLE_MACOS "hl2mp.sh"
	ENTITIES_FILENAME "hl2mp.fgd"
	SHADER_TYPE "source"
	SHADER_PATH "materials"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	DEFAULT_SCALE "0.25"
	DEFAULT_LIGHTMAP_SCALE "16"
	MAP_EXTENSION ".vmf"
	MAP_BACKUP_EXTENSION ".vmx"
	BUILD_MENU_FILENAME "default_build_menu_source.xml"
	ARCHIVE_TYPES "vpk" "gma" "gcf"
	TEXTURE_TYPES "vtf" "tth"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav" "mp3"
	MAP_TYPES "mapvmf"
	BRUSH_TYPES "source"
	PATCH_TYPES "quake3"
)

# Portal
radiant_add_gamepack(portal
	SUPPORT_LIGHTMAP_SCALE
	SUPPORT_OUTPUTS
	GAME_TYPE "source"
	HAS_BASEGAME
	BASE_TITLE "Half-Life 2"
	BASE_GAMEDIR "hl2"
	TITLE "Portal"
	KNOWN_GAMEDIRS "portal"
	KNOWN_TITLES "Portal"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Portal/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Portal/"
	EXECUTABLE_WIN32 "hl2.exe"
	EXECUTABLE_LINUX "hl2.sh"
	EXECUTABLE_MACOS "hl2.sh"
	ENTITIES_FILENAME "portal.fgd"
	SHADER_TYPE "source"
	SHADER_PATH "materials"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	DEFAULT_SCALE "0.25"
	DEFAULT_LIGHTMAP_SCALE "16"
	MAP_EXTENSION ".vmf"
	MAP_BACKUP_EXTENSION ".vmx"
	BUILD_MENU_FILENAME "default_build_menu_source.xml"
	ARCHIVE_TYPES "vpk" "gma" "gcf"
	TEXTURE_TYPES "vtf" "tth"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav" "mp3"
	MAP_TYPES "mapvmf"
	BRUSH_TYPES "source"
	PATCH_TYPES "quake3"
)

# Portal 2
radiant_add_gamepack(portal2
	SUPPORT_LIGHTMAP_SCALE
	SUPPORT_OUTPUTS
	GAME_TYPE "source"
	USE_NEW_OUTPUT_SEPARATOR
	TITLE "Portal 2"
	GAMEDIR "portal2"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Portal 2/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Portal 2/"
	EXECUTABLE_WIN32 "portal2.exe"
	EXECUTABLE_LINUX "portal2.sh"
	EXECUTABLE_MACOS "portal2.sh"
	ENTITIES_FILENAME "portal2.fgd"
	SHADER_TYPE "source"
	SHADER_PATH "materials"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	DEFAULT_SCALE "0.25"
	DEFAULT_LIGHTMAP_SCALE "16"
	MAP_EXTENSION ".vmf"
	MAP_BACKUP_EXTENSION ".vmx"
	BUILD_MENU_FILENAME "default_build_menu_source.xml"
	ARCHIVE_TYPES "vpk" "gma" "gcf"
	TEXTURE_TYPES "vtf" "tth"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav" "mp3"
	MAP_TYPES "mapvmf"
	BRUSH_TYPES "source"
	PATCH_TYPES "quake3"
)

# SiN Episodes: Emergence
radiant_add_gamepack(sinepisodes
	SUPPORT_LIGHTMAP_SCALE
	SUPPORT_OUTPUTS
	GAME_TYPE "source"
	HAS_BASEGAME
	BASE_TITLE "SiN Episodes: Emergence Base Content"
	BASE_GAMEDIR "vpks"
	TITLE "SiN Episodes: Emergence"
	KNOWN_GAMEDIRS "SE1"
	KNOWN_TITLES "SiN Episodes: Emergence"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/SiN Episodes Emergence/"
	PATH_LINUX "~/.steam/steam/steamapps/common/SiN Episodes Emergence/"
	EXECUTABLE_WIN32 "SinEpisodes.exe"
	EXECUTABLE_LINUX "SinEpisodes.sh"
	EXECUTABLE_MACOS "SinEpisodes.sh"
	ENTITIES_FILENAME "SinEpisodes.fgd"
	SHADER_TYPE "source"
	SHADER_PATH "materials"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	DEFAULT_SCALE "0.25"
	DEFAULT_LIGHTMAP_SCALE "16"
	MAP_EXTENSION ".vmf"
	MAP_BACKUP_EXTENSION ".vmx"
	BUILD_MENU_FILENAME "default_build_menu_source.xml"
	ARCHIVE_TYPES "vpk" "gma" "gcf"
	TEXTURE_TYPES "vtf" "tth"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav" "mp3"
	MAP_TYPES "mapvmf"
	BRUSH_TYPES "source"
	PATCH_TYPES "quake3"
)

# Team Fortress 2
radiant_add_gamepack(tf2
	SUPPORT_LIGHTMAP_SCALE
	SUPPORT_OUTPUTS
	GAME_TYPE "source"
	HAS_BASEGAME
	BASE_TITLE "Half-Life 2"
	BASE_GAMEDIR "hl2"
	TITLE "Team Fortress 2"
	KNOWN_GAMEDIRS "tf"
	KNOWN_TITLES "Team Fortress 2"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Team Fortress 2/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Team Fortress 2/"
	EXECUTABLE_WIN32 "tf.exe"
	EXECUTABLE_LINUX "tf.sh"
	EXECUTABLE_MACOS "tf.sh"
	ENTITIES_FILENAME "tf.fgd"
	SHADER_TYPE "source"
	SHADER_PATH "materials"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	DEFAULT_SCALE "0.25"
	DEFAULT_LIGHTMAP_SCALE "16"
	MAP_EXTENSION ".vmf"
	MAP_BACKUP_EXTENSION ".vmx"
	BUILD_MENU_FILENAME "default_build_menu_source.xml"
	ARCHIVE_TYPES "vpk" "gma" "gcf"
	TEXTURE_TYPES "vtf" "tth"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav" "mp3"
	MAP_TYPES "mapvmf"
	BRUSH_TYPES "source"
	PATCH_TYPES "quake3"
)

# Vampire: The Masquerade - Bloodlines
radiant_add_gamepack(vampire
	SUPPORT_LIGHTMAP_SCALE
	SUPPORT_OUTPUTS
	GAME_TYPE "source"
	HAS_BASEGAME
	BASE_TITLE "Vampire: The Masquerade - Bloodlines"
	BASE_GAMEDIR "Vampire"
	TITLE "Vampire: The Masquerade - Bloodlines"
	KNOWN_GAMEDIRS "Unofficial_Patch"
	KNOWN_TITLES "Vampire: The Masquerade - Bloodlines Unofficial Patch"
	PATH_WIN32 "C:/Program Files (x86)/Steam/steamapps/common/Vampire The Masquerade - Bloodlines/"
	PATH_LINUX "~/.steam/steam/steamapps/common/Vampire The Masquerade - Bloodlines/"
	EXECUTABLE_WIN32 "Vampire.exe"
	EXECUTABLE_LINUX "Vampire.sh"
	EXECUTABLE_MACOS "Vampire.sh"
	ENTITIES_FILENAME "vampire.fgd"
	SHADER_TYPE "source"
	SHADER_PATH "materials"
	ENTITY_CLASS "quake3"
	ENTITY_CLASS_TYPES "fgd"
	ENTITIES "source"
	DEFAULT_SCALE "0.25"
	DEFAULT_LIGHTMAP_SCALE "16"
	MAP_EXTENSION ".vmf"
	MAP_BACKUP_EXTENSION ".vmx"
	BUILD_MENU_FILENAME "default_build_menu_source.xml"
	ARCHIVE_TYPES "vpk" "gma" "gcf"
	TEXTURE_TYPES "vtf" "tth"
	MODEL_TYPES "mdl"
	SOUND_TYPES "wav" "mp3"
	MAP_TYPES "mapvmf"
	BRUSH_TYPES "source"
	PATCH_TYPES "quake3"
)
