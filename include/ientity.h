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

#include <format>

#include "generic/constant.h"

#include "string/string.h"
#include "scenelib.h"
#include "qerplugin.h"

class EntityClass;
class Entity;

class EntityOutput
{
	Entity& m_entity;
	std::string m_name;
	std::string m_target;
	std::string m_input;
	std::string m_data;
	float m_delay;
	int m_numUses;
	char m_separator;
public:
	EntityOutput( Entity& entity, const char* name, const char* target, const char* input, const char* data = "", float delay = 0, int numUses = -1 ) : m_entity( entity ), m_name( name ), m_target( target ), m_input( input ), m_data( data ), m_delay( delay ), m_numUses( numUses ) {
		if ( string_equal( GlobalRadiant().getGameDescriptionKeyValue( "use_new_output_separator" ), "1" ) ) {
			m_separator = '\x1b';
		} else {
			m_separator = ',';
		}
	}
	EntityOutput( Entity& entity, const char* key, const char* value ) : m_entity( entity ), m_name( key ) {
		if ( string_equal( GlobalRadiant().getGameDescriptionKeyValue( "use_new_output_separator" ), "1" ) ) {
			m_separator = '\x1b';
		} else {
			m_separator = ',';
		}
		// FIXME: why am i doing this C-style?
		std::string delay, numUses;
		while ( *value && *value != m_separator ) m_target.append( 1, *(value++) );
		value++;
		while ( *value && *value != m_separator ) m_input.append( 1, *(value++) );
		value++;
		while ( *value && *value != m_separator ) m_data.append( 1, *(value++) );
		value++;
		while ( *value && *value != m_separator ) delay.append( 1, *(value++) );
		value++;
		while ( *value && *value != m_separator ) numUses.append( 1, *(value++) );
		if ( !delay.empty() ) {
			try {
				m_delay = std::stof( delay );
			} catch(std::invalid_argument& e) {
				m_delay = 0;
			}
		} else {
			m_delay = 0;
		}
		if ( !numUses.empty() ) {
			try {
				m_numUses = std::stoi( numUses );
			} catch(std::invalid_argument& e) {
				m_numUses = -1;
			}
		} else {
			m_numUses = -1;
		}
	}
	~EntityOutput() = default;
	std::string key() const {
		return m_name;
	}
	std::string value() const {
		return std::format( "{}{}{}{}{}{}{}{}{}", m_target, m_separator, m_input, m_separator, m_data, m_separator, m_delay, m_separator, m_numUses );
	}
	char separator() const {
		return m_separator;
	}
	void set( const char* name, const char* target, const char* input, const char* data = "", float delay = 0, int numUses = -1 ) {
		m_name = name;
		m_target = target;
		m_input = input;
		m_data = data;
		m_delay = delay;
		m_numUses = numUses;
	}
	void setName( const char* s ) {
		m_name = s;
	}
	void setTarget( const char* s ) {
		m_target = s;
	}
	void setInput( const char* s ) {
		m_input = s;
	}
	void setData( const char* s ) {
		m_data = s;
	}
	void setDelay( float f ) {
		m_delay = f;
	}
	void setNumUses( int i ) {
		m_numUses = i;
	}
	Entity& parent() {
		return m_entity;
	}
	const Entity& parent() const {
		return m_entity;
	}
};

typedef Callback<void(const char*)> KeyObserver;

class EntityKeyValue
{
public:
	virtual const char* c_str() const = 0;
	virtual void assign( const char* other ) = 0;
	virtual void attach( const KeyObserver& observer ) = 0;
	virtual void detach( const KeyObserver& observer ) = 0;
};

class Entity
{
public:
	STRING_CONSTANT( Name, "Entity" );

	class Observer
	{
	public:
		virtual void insert( const char* key, EntityKeyValue& value ) = 0;
		virtual void erase( const char* key, EntityKeyValue& value ) = 0;
		virtual void clear() { };
	};

	class Visitor
	{
	public:
		virtual void visit( const char* key, const char* value ) = 0;
	};

	class OutputVisitor
	{
	public:
		virtual void visit( EntityOutput* output ) = 0;
	};

	virtual const EntityClass& getEntityClass() const = 0;
	virtual const char* getClassName() const = 0;
	virtual void forEachKeyValue( Visitor& visitor ) const = 0;
	virtual void setKeyValue( const char* key, const char* value ) = 0;
	virtual const char* getKeyValue( const char* key ) const = 0;
	virtual bool hasKeyValue( const char* key ) const = 0;
	virtual bool isContainer() const = 0;
	virtual void attach( Observer& observer ) = 0;
	virtual void detach( Observer& observer ) = 0;
	virtual EntityOutput* addOutput( const char* name, const char* target, const char* input, const char* data = "", float delay = 0, int numUses = -1 ) = 0;
	virtual EntityOutput* addOutput( const char* key, const char* value ) = 0;
	virtual void removeOutput( EntityOutput* output ) = 0;
	virtual void forEachOutput( OutputVisitor& visitor ) = 0;
};

class EntityCopyingVisitor : public Entity::Visitor
{
	Entity& m_entity;
public:
	EntityCopyingVisitor( Entity& entity )
		: m_entity( entity ){
	}

	void visit( const char* key, const char* value ) override {
		if ( !string_equal( key, "classname" ) ) {
			m_entity.setKeyValue( key, value );
		}
	}
};

inline Entity* Node_getEntity( scene::Node& node ){
	return NodeTypeCast<Entity>::cast( node );
}


template<typename value_type>
class Stack;
template<typename Contained>
class Reference;

namespace scene
{
class Node;
}

typedef Reference<scene::Node> NodeReference;

namespace scene
{
typedef Stack<NodeReference> Path;
}

class Counter;

class EntityCreator
{
public:
	INTEGER_CONSTANT( Version, 2 );
	STRING_CONSTANT( Name, "entity" );

	virtual scene::Node& createEntity( EntityClass* eclass ) = 0;

	typedef void ( *KeyValueChangedFunc )();
	virtual void setKeyValueChangedFunc( KeyValueChangedFunc func ) = 0;

	virtual void setCounter( Counter* counter ) = 0;

	virtual void connectEntities( const scene::Path& e1, const scene::Path& e2, int index ) = 0;

	virtual void setLightColorize( bool lightColorize ) = 0;
	virtual void setLightRadii( bool lightRadii ) = 0;
	virtual bool getLightRadii() = 0;
	virtual void setShowNames( bool showNames ) = 0;
	virtual bool getShowNames() = 0;
	virtual void setShowBboxes( bool showBboxes ) = 0;
	virtual bool getShowBboxes() = 0;
	virtual void setShowConnections( bool showConnections ) = 0;
	virtual bool getShowConnections() = 0;
	virtual void setShowNamesDist( int dist ) = 0;
	virtual int getShowNamesDist() = 0;
	virtual void setShowNamesRatio( int ratio ) = 0;
	virtual int getShowNamesRatio() = 0;
	virtual void setShowTargetNames( bool showNames ) = 0;
	virtual bool getShowTargetNames() = 0;
	virtual void setShowAngles( bool showAngles ) = 0;
	virtual bool getShowAngles() = 0;

	virtual void printStatistics() const = 0;
};

#include "modulesystem.h"

template<typename Type>
class GlobalModule;
typedef GlobalModule<EntityCreator> GlobalEntityModule;

template<typename Type>
class GlobalModuleRef;
typedef GlobalModuleRef<EntityCreator> GlobalEntityModuleRef;

inline EntityCreator& GlobalEntityCreator(){
	return GlobalEntityModule::getTable();
}
