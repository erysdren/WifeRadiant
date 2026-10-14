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

#include "iarchive.h"

#include "debugging/debugging.h"
#include "modulesystem/singletonmodule.h"

#include "archive.h"

class ArchiveVpkAPI
{
	_QERArchiveTable m_archivevpk;
public:
	typedef _QERArchiveTable Type;
	STRING_CONSTANT( Name, "vpk" );

	ArchiveVpkAPI(){
		m_archivevpk.m_pfnOpenArchive = &OpenArchive;
	}
	_QERArchiveTable* getTable(){
		return &m_archivevpk;
	}
};

typedef SingletonModule<ArchiveVpkAPI> ArchiveVpkModule;

ArchiveVpkModule g_ArchiveVpkModule;


class ArchiveGmaAPI
{
	_QERArchiveTable m_archivegma;
public:
	typedef _QERArchiveTable Type;
	STRING_CONSTANT( Name, "gma" );

	ArchiveGmaAPI(){
		m_archivegma.m_pfnOpenArchive = &OpenArchive;
	}
	_QERArchiveTable* getTable(){
		return &m_archivegma;
	}
};

typedef SingletonModule<ArchiveGmaAPI> ArchiveGmaModule;

ArchiveGmaModule g_ArchiveGmaModule;


class ArchiveGcfAPI
{
	_QERArchiveTable m_archivegcf;
public:
	typedef _QERArchiveTable Type;
	STRING_CONSTANT( Name, "gcf" );

	ArchiveGcfAPI(){
		m_archivegcf.m_pfnOpenArchive = &OpenArchive;
	}
	_QERArchiveTable* getTable(){
		return &m_archivegcf;
	}
};

typedef SingletonModule<ArchiveGcfAPI> ArchiveGcfModule;

ArchiveGcfModule g_ArchiveGcfModule;


class ArchivePakAPI
{
	_QERArchiveTable m_archivepak;
public:
	typedef _QERArchiveTable Type;
	STRING_CONSTANT( Name, "pak" );

	ArchivePakAPI(){
		m_archivepak.m_pfnOpenArchive = &OpenArchive;
	}
	_QERArchiveTable* getTable(){
		return &m_archivepak;
	}
};

typedef SingletonModule<ArchivePakAPI> ArchivePakModule;

ArchivePakModule g_ArchivePakModule;


class ArchiveZipAPI
{
	_QERArchiveTable m_archivezip;
public:
	typedef _QERArchiveTable Type;
	STRING_CONSTANT( Name, "zip" );

	ArchiveZipAPI(){
		m_archivezip.m_pfnOpenArchive = &OpenArchive;
	}
	_QERArchiveTable* getTable(){
		return &m_archivezip;
	}
};

typedef SingletonModule<ArchiveZipAPI> ArchiveZipModule;

ArchiveZipModule g_ArchiveZipModule;


class ArchivePk3API
{
	_QERArchiveTable m_archivepk3;
public:
	typedef _QERArchiveTable Type;
	STRING_CONSTANT( Name, "pk3" );

	ArchivePk3API(){
		m_archivepk3.m_pfnOpenArchive = &OpenArchive;
	}
	_QERArchiveTable* getTable(){
		return &m_archivepk3;
	}
};

typedef SingletonModule<ArchivePk3API> ArchivePk3Module;

ArchivePk3Module g_ArchivePk3Module;


class ArchivePk4API
{
	_QERArchiveTable m_archivepk4;
public:
	typedef _QERArchiveTable Type;
	STRING_CONSTANT( Name, "pk4" );

	ArchivePk4API(){
		m_archivepk4.m_pfnOpenArchive = &OpenArchive;
	}
	_QERArchiveTable* getTable(){
		return &m_archivepk4;
	}
};

typedef SingletonModule<ArchivePk4API> ArchivePk4Module;

ArchivePk4Module g_ArchivePk4Module;


class ArchiveDpkAPI
{
	_QERArchiveTable m_archivedpk;
public:
	typedef _QERArchiveTable Type;
	STRING_CONSTANT( Name, "dpk" );

	ArchiveDpkAPI(){
		m_archivedpk.m_pfnOpenArchive = &OpenArchive;
	}
	_QERArchiveTable* getTable(){
		return &m_archivedpk;
	}
};

typedef SingletonModule<ArchiveDpkAPI> ArchiveDpkModule;

ArchiveDpkModule g_ArchiveDpkModule;


extern "C" void RADIANT_DLLEXPORT Radiant_RegisterModules( ModuleServer& server ){
	initialiseModule( server );

	g_ArchiveVpkModule.selfRegister();
	g_ArchiveGmaModule.selfRegister();
	g_ArchiveGcfModule.selfRegister();
	g_ArchivePakModule.selfRegister();
	g_ArchiveZipModule.selfRegister();
	g_ArchivePk3Module.selfRegister();
	g_ArchivePk4Module.selfRegister();
	g_ArchiveDpkModule.selfRegister();
}
