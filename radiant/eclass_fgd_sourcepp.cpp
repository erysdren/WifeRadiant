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

#include "eclass_fgd_sourcepp.h"

#include "debugging/debugging.h"

#include <map>
#include <set>
#include <format>

#include "texwindow.h"

#include "iscriplib.h"

#include "string/string.h"
#include "eclasslib.h"
#include "os/path.h"
#include "stream/stringstream.h"
#include "stringio.h"
#include "stream/textfilestream.h"
#include "math/vector.h"

#include "entityinspector.h"

#include <toolpp/toolpp.h>

typedef std::map<const char*, EntityClass*, RawStringLessNoCase> EntityClasses;
static EntityClasses g_EntityClassFGD_classes;
typedef std::map<CopiedString, ListAttributeType> ListAttributeTypes;
ListAttributeTypes g_listTypesFGD;

static std::vector<toolpp::FGD::Entity::ClassProperty>::const_iterator findClassProperty( const toolpp::FGD::Entity& entity, const char *name ) {
	auto first = entity.classProperties.cbegin();
	auto last = entity.classProperties.cend();
	while ( first != last ) {
		if ( (*first).name == name ) {
			return first;
		}
		++first;
	}
	return last;
}

static void addChoicesToEntity( EntityClass* entityClass, const std::vector<toolpp::FGD::Entity::FieldChoices>& fields ) {
	for ( const auto& field : fields ) {
		EntityClassAttribute attribute;
		attribute.m_name = field.name;
		attribute.m_displayName = field.displayName;

		std::string listTypeName = std::format("{}_{}", entityClass->name(), field.name);
		attribute.m_type = listTypeName;
		ListAttributeType& listType = g_listTypesFGD[listTypeName.c_str()];
		for ( const auto& choice : field.choices ) {
			listType.push_back( choice.displayName.data(), choice.value.data() );
		}

		attribute.m_value = field.valueDefault;
		attribute.m_description = field.description;
		EntityClass_insertAttribute( *entityClass, field.name.data(), attribute );
	}
}

static void addIOToEntity( EntityClass* entityClass, const std::vector<toolpp::FGD::Entity::IO>& inputs, const std::vector<toolpp::FGD::Entity::IO>& outputs ) {
	for ( const auto& input : inputs ) {
		EntityClassAttribute attribute;
		attribute.m_name = input.name;
		attribute.m_displayName = input.name;
		attribute.m_type = input.valueType;
		attribute.m_description = input.description;
		EntityClass_insertInput( *entityClass, attribute.m_name.c_str(), attribute );
	}
	for ( const auto& output : outputs ) {
		EntityClassAttribute attribute;
		attribute.m_name = output.name;
		attribute.m_displayName = output.name;
		attribute.m_type = output.valueType;
		attribute.m_description = output.description;
		EntityClass_insertOutput( *entityClass, attribute.m_name.c_str(), attribute );
	}
}

static void addFieldsToEntity( EntityClass* entityClass, const std::vector<toolpp::FGD::Entity::Field>& fields ) {
	for ( const auto& field : fields ) {
		EntityClassAttribute attribute;
		attribute.m_name = field.name;
		attribute.m_displayName = field.displayName;
		if ( field.valueType == "studio" || field.valueType == "model" ) {
			attribute.m_type = "model";
			if ( !entityClass->m_modelpath.empty() && !field.valueDefault.empty() ) {
				entityClass->miscmodel_is = true;
				entityClass->m_modelpath = field.valueDefault;
			}
		} else if ( field.valueType == "color255" || field.valueType == "color") {
			attribute.m_type = "color";
		} else if ( field.valueType == "material" || field.valueType == "shader" ) {
			attribute.m_type = "shader";
		} else {
			// FIXME: add proper handlers for more Source-specific types in entityinspector.cpp
			// attribute.m_type = field.valueType;
			attribute.m_type = "string";
		}
		attribute.m_value = field.valueDefault;
		attribute.m_description = field.description;
		EntityClass_insertAttribute( *entityClass, field.name.data(), attribute );
	}
}

static void addFlagsToEntity( EntityClass* entityClass, const std::vector<toolpp::FGD::Entity::FieldFlags>& fields ) {
	for ( const auto& field : fields ) {
		std::string name = field.name.data();
		entityClass->flags[name].displayName = field.displayName;
		for ( const auto& flag : field.flags ) {
			if ( flag.value <= 0) {
				continue;
			}
			const size_t bit = std::log2( flag.value );
			EntityClassAttribute *attribute = &EntityClass_insertAttribute( *entityClass, name.c_str(), EntityClassAttribute( "flag", name.c_str() ) ).second;
			attribute->m_displayName = flag.displayName;
			attribute->m_description = flag.description;
			entityClass->flags[name].flags[bit].displayName = flag.displayName;
			entityClass->flags[name].flags[bit].attribute = attribute;
			g_entityFlagFields[name] += 1;
		}
	}
}

static void addColorToEntity( EntityClass* entityClass, const toolpp::FGD::Entity& entity ) {
	if ( entityClass->colorSpecified ) {
		return;
	}
	if ( auto colorProperty = findClassProperty( entity, "color" ); colorProperty != entity.classProperties.end() ) {
		if ( std::sscanf( (*colorProperty).arguments.data(), "%f%f%f", &entityClass->color.x(), &entityClass->color.y(), &entityClass->color.z() ) == 3 ) {
			entityClass->colorSpecified = true;
			entityClass->color.x() /= 255.0;
			entityClass->color.y() /= 255.0;
			entityClass->color.z() /= 255.0;
		}
	}
}

static void addSizeToEntity( EntityClass* entityClass, const toolpp::FGD::Entity& entity ) {
	if ( entityClass->sizeSpecified || !entityClass->fixedsize ) {
		return;
	}
	if ( auto sizeProperty = findClassProperty( entity, "size" ); sizeProperty != entity.classProperties.end() ) {
		StringInputStream istream( (*sizeProperty).arguments );
		Tokeniser& tokeniser = GlobalScriptLibrary().m_pfnNewScriptTokeniser( istream );

		Tokeniser_getFloat(tokeniser, entityClass->mins.x());
		Tokeniser_getFloat(tokeniser, entityClass->mins.y());
		Tokeniser_getFloat(tokeniser, entityClass->mins.z());

		const char *token = tokeniser.getToken();
		if (!token || string_empty(token))
		{
			entityClass->maxs = entityClass->mins;
			vector3_negate( entityClass->mins );
		}
		else if (string_equal(token, ","))
		{
			Tokeniser_getFloat(tokeniser, entityClass->maxs.x());
			Tokeniser_getFloat(tokeniser, entityClass->maxs.y());
			Tokeniser_getFloat(tokeniser, entityClass->maxs.z());
		}
		else
		{
			entityClass->maxs.x() = std::stof(token);
			Tokeniser_getFloat(tokeniser, entityClass->maxs.y());
			Tokeniser_getFloat(tokeniser, entityClass->maxs.z());
		}

		entityClass->sizeSpecified = true;
	}
}

static void addModelToEntity( EntityClass* entityClass, const toolpp::FGD::Entity& entity ) {
	if ( !entityClass->m_modelpath.empty() || entityClass->miscmodel_is ) {
		return;
	}
	if ( auto studioProperty = findClassProperty( entity, "studio" ); studioProperty != entity.classProperties.end() ) {
		if ( !(*studioProperty).arguments.empty() ) {
			StringInputStream istream( (*studioProperty).arguments );
			Tokeniser& tokeniser = GlobalScriptLibrary().m_pfnNewScriptTokeniser( istream );
			auto modelNameCleaned = StringStream<64>( PathCleaned( tokeniser.getToken() ) );
			entityClass->m_modelpath = string_to_lowercase( modelNameCleaned.c_str() );
		}
	} else if ( auto studioProperty = findClassProperty( entity, "studioprop" ); studioProperty != entity.classProperties.end() ) {
		entityClass->miscmodel_is = true;
	} else if ( auto studioProperty = findClassProperty( entity, "model" ); studioProperty != entity.classProperties.end() ) {
		entityClass->miscmodel_is = true;
	}
}

static void addMiscToEntity( EntityClass* entityClass, const toolpp::FGD::Entity& entity ) {
	if ( auto boundsProperty = findClassProperty( entity, "quadbounds" ); boundsProperty != entity.classProperties.end() ) {
		entityClass->quadbounds = true;
	}
}

static void addBaseAttributes( EntityClass* entityClass, const std::unordered_map<std::string_view, toolpp::FGD::Entity>& entities, const toolpp::FGD::Entity& entity ) {
	if ( auto baseProperty = findClassProperty( entity, "base" ); baseProperty != entity.classProperties.end() ) {
		StringInputStream istream( (*baseProperty).arguments );
		Tokeniser& tokeniser = GlobalScriptLibrary().m_pfnNewScriptTokeniser( istream );
		while ( const char *baseClassName = tokeniser.getToken() ) {
			if ( string_equal( baseClassName, "," ) ) {
				continue;
			}
			if ( auto baseClass = entities.find( baseClassName ); baseClass != entities.end() ) {
				entityClass->m_parent.push_back( baseClassName );
				addMiscToEntity( entityClass, baseClass->second );
				addModelToEntity( entityClass, baseClass->second );
				addSizeToEntity( entityClass, baseClass->second );
				addColorToEntity( entityClass, baseClass->second );
				addChoicesToEntity( entityClass, baseClass->second.fieldsWithChoices );
				addIOToEntity( entityClass, baseClass->second.inputs, baseClass->second.outputs );
				addFieldsToEntity( entityClass, baseClass->second.fields );
				addFlagsToEntity( entityClass, baseClass->second.fieldsWithFlags );
				addBaseAttributes( entityClass, entities, baseClass->second );
			}
		}
	}
}

void Eclass_ScanFile_fgd( EntityClassCollector& collector, const char *filename ){
	toolpp::FGD fgd = toolpp::FGD(filename);

	const auto& materialExclusionDirs = fgd.getMaterialExclusionDirs();

	g_ShaderExclusionDirs.clear();

	for ( const auto& dir : materialExclusionDirs ) {
		g_ShaderExclusionDirs.push_back(std::string{dir});
	}

	g_entityFlagFields.clear();

	const auto& entities = fgd.getEntities();

	if ( !entities.size() ) {
		globalWarningStream() << "failed to load any entities from " << Quoted( filename ) << '\n';
		return;
	}

	for ( const auto & [ entityName, entity ] : entities ) {
		if ( entity.classType == "BaseClass" ) {
			// base types
			continue;
		}

		EntityClass* entityClass = Eclass_Alloc();
		entityClass->free = &Eclass_Free;
		entityClass->sizeSpecified = false;
		entityClass->colorSpecified = false;
		entityClass->inheritanceResolved = false;
		entityClass->mins = Vector3( -8, -8, -8 );
		entityClass->maxs = Vector3( 8, 8, 8 );
		entityClass->color = Vector3( 0.7, 0.7, 0.7 );
		entityClass->name_set( entityName.data() );
		entityClass->m_comments = entity.description;
		entityClass->has_angles = true;
		entityClass->has_angles_key = true;

		if ( entity.classType == "SolidClass" ) {
			// solid types
			entityClass->fixedsize = false;
		} else if ( string_equal_suffix( entity.classType.data(), "Class" ) ) {
			// all other class types are assumed to be point sized
			entityClass->fixedsize = true;
		}

		addMiscToEntity( entityClass, entity );
		addModelToEntity( entityClass, entity );
		addSizeToEntity( entityClass, entity );
		addColorToEntity( entityClass, entity );
		addChoicesToEntity( entityClass, entity.fieldsWithChoices );
		addIOToEntity( entityClass, entity.inputs, entity.outputs );
		addFieldsToEntity( entityClass, entity.fields );
		addFlagsToEntity( entityClass, entity.fieldsWithFlags );
		addBaseAttributes( entityClass, entities, entity );

		g_EntityClassFGD_classes.insert( EntityClasses::value_type( entityClass->name(), entityClass ) );
	}
}

void EClass_finalize_fgd( EntityClassCollector& collector ){
	for ( auto [ name, eclass ] : g_EntityClassFGD_classes )
	{
		eclass_capture_state( eclass );
		collector.insert( eclass );
	}
	for( const auto& [ name, list ] : g_listTypesFGD )
	{
		collector.insert( name.c_str(), list );
	}
	g_EntityClassFGD_classes.clear();
	g_listTypesFGD.clear();
}


#include "modulesystem/singletonmodule.h"
#include "modulesystem/moduleregistry.h"

class EntityClassFgdDependencies : public GlobalShaderCacheModuleRef, public GlobalScripLibModuleRef
{
};

class EclassFgdAPI
{
	EntityClassScanner m_eclassfgd;
public:
	typedef EntityClassScanner Type;
	STRING_CONSTANT( Name, "fgd" );

	EclassFgdAPI(){
		m_eclassfgd.scanFile = &Eclass_ScanFile_fgd;
		m_eclassfgd.getExtension = [](){ return "fgd"; };
		m_eclassfgd.finalize = &EClass_finalize_fgd;
	}
	EntityClassScanner* getTable(){
		return &m_eclassfgd;
	}
};

typedef SingletonModule<EclassFgdAPI, EntityClassFgdDependencies> EclassFgdModule;
typedef Static<EclassFgdModule> StaticEclassFgdModule;
StaticRegisterModule staticRegisterEclassFgd( StaticEclassFgdModule::instance() );
