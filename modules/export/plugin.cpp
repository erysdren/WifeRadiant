/*
    Copyright (C) 2025-2026 erysdren (it/its)

    This file is part of WifeRadiant.

    WifeRadiant is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    WifeRadiant is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with WifeRadiant.  If not, see <https://www.gnu.org/licenses/>.
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

#include "obj.h"
#include "gltf.h"

class ExportDependencies :
	public GlobalRadiantModuleRef,
	public GlobalBrushModuleRef,
	public GlobalPatchModuleRef,
	public GlobalFiletypesModuleRef,
	public GlobalScripLibModuleRef,
	public GlobalEntityClassManagerModuleRef,
	public GlobalSceneGraphModuleRef
{
public:
	ExportDependencies() :
		GlobalBrushModuleRef( GlobalRadiant().getRequiredGameDescriptionKeyValue( "brushtypes" ) ),
		GlobalPatchModuleRef( GlobalRadiant().getRequiredGameDescriptionKeyValue( "patchtypes" ) ),
		GlobalEntityClassManagerModuleRef( GlobalRadiant().getRequiredGameDescriptionKeyValue( "entityclass" ) ){
	}
};


class ExportGltfAPI final : public TypeSystemRef, public SceneExport
{
public:
	typedef SceneExport Type;
	STRING_CONSTANT( Name, "gltf" );

	virtual EFeatureFlags getFeatureFlags() const override {
		return EFeatureFlags::eBrushes | EFeatureFlags::ePatches | EFeatureFlags::eLights | EFeatureFlags::eEntities;
	}

	virtual const char* getName() const override {
		return "glTF";
	}

	virtual const char* getExtension() const override {
		return "gltf";
	}

	SceneExport* getTable(){
		return this;
	}

	virtual void writeGraph( scene::Node& root, GraphTraversalFunc traverse, FileOutputStream& outputStream ) const override {
		writeGLTF(root, traverse, outputStream);
	}
};

typedef SingletonModule<ExportGltfAPI, ExportDependencies> ExportGltfModule;

ExportGltfModule g_ExportGltfModule;


class ExportObjAPI final : public TypeSystemRef, public SceneExport
{
public:
	typedef SceneExport Type;
	STRING_CONSTANT( Name, "obj" );

	virtual EFeatureFlags getFeatureFlags() const override {
		return EFeatureFlags::eBrushes;
	}

	virtual const char* getName() const override {
		return "Wavefront OBJ";
	}

	virtual const char* getExtension() const override {
		return "obj";
	}

	SceneExport* getTable(){
		return this;
	}

	virtual void writeGraph( scene::Node& root, GraphTraversalFunc traverse, FileOutputStream& outputStream ) const override {
		writeOBJ(root, traverse, outputStream);
	}
};

typedef SingletonModule<ExportObjAPI, ExportDependencies> ExportObjModule;

ExportObjModule g_ExportObjModule;

extern "C" void RADIANT_DLLEXPORT Radiant_RegisterModules( ModuleServer& server ){
	initialiseModule( server );

	g_ExportGltfModule.selfRegister();
	g_ExportObjModule.selfRegister();
}
