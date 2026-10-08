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

#include "plugin.h"

#include "iscriplib.h"
#include "ibrush.h"
#include "ipatch.h"
#include "ifiletypes.h"
#include "ieclass.h"
#include "qerplugin.h"

#include "modulesystem/singletonmodule.h"
#include "typesystem.h"

#include "isceneexport.h"

#include "debugging/debugging.h"

class ExportObjDependencies :
	public GlobalRadiantModuleRef,
	public GlobalBrushModuleRef,
	public GlobalPatchModuleRef,
	public GlobalFiletypesModuleRef,
	public GlobalScripLibModuleRef,
	public GlobalEntityClassManagerModuleRef,
	public GlobalSceneGraphModuleRef
{
public:
	ExportObjDependencies() :
		GlobalBrushModuleRef( GlobalRadiant().getRequiredGameDescriptionKeyValue( "brushtypes" ) ),
		GlobalPatchModuleRef( GlobalRadiant().getRequiredGameDescriptionKeyValue( "patchtypes" ) ),
		GlobalEntityClassManagerModuleRef( GlobalRadiant().getRequiredGameDescriptionKeyValue( "entityclass" ) ){
	}
};

class ExportObjAPI : public TypeSystemRef, public SceneExporter
{
public:
	typedef SceneExporter Type;
	STRING_CONSTANT( Name, "obj" );

	ExportObjAPI() { }
	~ExportObjAPI() { }

	SceneExporter* getTable(){
		return this;
	}

	virtual void writeGraph( scene::Node& root, GraphTraversalFunc traverse, FileOutputStream& outputStream ) const override {

	}
};

typedef SingletonModule<ExportObjAPI, ExportObjDependencies> ExportObjModule;

ExportObjModule g_ExportObjModule;

extern "C" void RADIANT_DLLEXPORT Radiant_RegisterModules( ModuleServer& server ){
	initialiseModule( server );

	g_ExportObjModule.selfRegister();
}
