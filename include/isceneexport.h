/*
   Copyright (C) 2001-2006, William Joseph.
   All Rights Reserved.

   This file is part of GtkRadiant.

   GtkRadiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GtkRadiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include "generic/constant.h"
#include "imap.h"
#include "stream/filestream.h"


/// \brief A module that writes scene data in a specific format.
class SceneExport
{
public:
	INTEGER_CONSTANT( Version, 1 );
	STRING_CONSTANT( Name, "sceneexport" );

	/// \brief Flags describing what features this exporter supports.
	enum EFeatureFlags {
		eBrushes = 1 << 0, ///< can export brushes
		ePatches = 1 << 1, ///< can export patch meshes
		eLights = 1 << 2, ///< can export lights
		eEntities = 1 << 3, ///< can export entities
		eLayers = 1 << 4, ///< can export layer information
	};

	/// \brief Returns a set of flags determining what features are available in this exporter.
	virtual EFeatureFlags getFeatureFlags() const = 0;

	/// \brief returns a null-terminated string with a user-facing name of the format.
	virtual const char* getName() const = 0;

	/// \brief returns a null-terminated string with the file extension of the format (no leading period).
	virtual const char* getExtension() const = 0;

	/// \brief Write the map graph obtained by applying \p traverse to \p root into \p outputStream.
	virtual void writeGraph( scene::Node& root, GraphTraversalFunc traverse, FileOutputStream& outputStream ) const = 0;
};

inline SceneExport::EFeatureFlags operator|(SceneExport::EFeatureFlags a, SceneExport::EFeatureFlags b)
{
	return static_cast<SceneExport::EFeatureFlags>(static_cast<int>(a) | static_cast<int>(b));
}

template<typename Type>
class ModuleRef;
typedef ModuleRef<SceneExport> SceneExportModuleRef;

template<typename Type>
class Modules;
typedef Modules<SceneExport> SceneExportModules;

template<typename Type>
class ModulesRef;
typedef ModulesRef<SceneExport> SceneExportModulesRef;
