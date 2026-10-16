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

#include "image.h"

#include "ifilesystem.h"
#include "iimage.h"

#include "tga.h"
#include "bmp.h"
#include "pcx.h"
#include "stb.h"
#include "vtf.h"
#include "pvr.h"

#include "modulesystem/singletonmodule.h"

class ImageDependencies : public GlobalFileSystemModuleRef
{
};

class ImageTGAAPI
{
	_QERPlugImageTable m_imagetga;
public:
	typedef _QERPlugImageTable Type;
	STRING_CONSTANT( Name, "tga" );

	ImageTGAAPI(){
		m_imagetga.loadImage = LoadTGA;
	}
	_QERPlugImageTable* getTable(){
		return &m_imagetga;
	}
};

typedef SingletonModule<ImageTGAAPI> ImageTGAModule;

ImageTGAModule g_ImageTGAModule;


class ImageJPGAPI
{
	_QERPlugImageTable m_imagejpg;
public:
	typedef _QERPlugImageTable Type;
	STRING_CONSTANT( Name, "jpg" );

	ImageJPGAPI(){
		m_imagejpg.loadImage = LoadJPG;
	}
	_QERPlugImageTable* getTable(){
		return &m_imagejpg;
	}
};

typedef SingletonModule<ImageJPGAPI, ImageDependencies> ImageJPGModule;

ImageJPGModule g_ImageJPGModule;


class ImagePNGAPI
{
	_QERPlugImageTable m_imagepng;
public:
	typedef _QERPlugImageTable Type;
	STRING_CONSTANT( Name, "png" );

	ImagePNGAPI(){
		m_imagepng.loadImage = LoadPNG;
	}
	_QERPlugImageTable* getTable(){
		return &m_imagepng;
	}
};

typedef SingletonModule<ImagePNGAPI, ImageDependencies> ImagePNGModule;

ImagePNGModule g_ImagePNGModule;


class ImageBMPAPI
{
	_QERPlugImageTable m_imagebmp;
public:
	typedef _QERPlugImageTable Type;
	STRING_CONSTANT( Name, "bmp" );

	ImageBMPAPI(){
		m_imagebmp.loadImage = LoadBMP;
	}
	_QERPlugImageTable* getTable(){
		return &m_imagebmp;
	}
};

typedef SingletonModule<ImageBMPAPI, ImageDependencies> ImageBMPModule;

ImageBMPModule g_ImageBMPModule;


class ImagePCXAPI
{
	_QERPlugImageTable m_imagepcx;
public:
	typedef _QERPlugImageTable Type;
	STRING_CONSTANT( Name, "pcx" );

	ImagePCXAPI(){
		m_imagepcx.loadImage = LoadPCX32;
	}
	_QERPlugImageTable* getTable(){
		return &m_imagepcx;
	}
};

typedef SingletonModule<ImagePCXAPI, ImageDependencies> ImagePCXModule;

ImagePCXModule g_ImagePCXModule;


#ifndef NO_DDS
#include "dds.h"
class ImageDDSAPI
{
	_QERPlugImageTable m_imagedds;
public:
	typedef _QERPlugImageTable Type;
	STRING_CONSTANT( Name, "dds" );

	ImageDDSAPI(){
		m_imagedds.loadImage = LoadDDS;
	}
	_QERPlugImageTable* getTable(){
		return &m_imagedds;
	}
};

typedef SingletonModule<ImageDDSAPI, ImageDependencies> ImageDDSModule;

ImageDDSModule g_ImageDDSModule;
#endif

#ifndef NO_KTX
#include "ktx.h"
class ImageKTXAPI
{
	_QERPlugImageTable m_imagektx;
public:
	typedef _QERPlugImageTable Type;
	STRING_CONSTANT( Name, "ktx" );

	ImageKTXAPI(){
		m_imagektx.loadImage = LoadKTX;
	}
	_QERPlugImageTable* getTable(){
		return &m_imagektx;
	}
};

typedef SingletonModule<ImageKTXAPI, ImageDependencies> ImageKTXModule;

ImageKTXModule g_ImageKTXModule;
#endif


#ifndef NO_CRN
#include "crn.h"
class ImageCRNAPI
{
	_QERPlugImageTable m_imagecrn;
public:
	typedef _QERPlugImageTable Type;
	STRING_CONSTANT( Name, "crn" );

	ImageCRNAPI(){
		m_imagecrn.loadImage = LoadCRN;
	}
	_QERPlugImageTable* getTable(){
		return &m_imagecrn;
	}
};

typedef SingletonModule<ImageCRNAPI, ImageDependencies> ImageCRNModule;

ImageCRNModule g_ImageCRNModule;
#endif

#ifndef NO_WEBP
#include "webp.h"
class ImageWebpAPI
{
	_QERPlugImageTable m_imagewebp;
public:
	typedef _QERPlugImageTable Type;
	STRING_CONSTANT( Name, "webp" );

	ImageWebpAPI(){
		m_imagewebp.loadImage = LoadWebp;
	}
	_QERPlugImageTable* getTable(){
		return &m_imagewebp;
	}
};

typedef SingletonModule<ImageWebpAPI, ImageDependencies> ImageWebpModule;

ImageWebpModule g_ImageWebpModule;
#endif

class ImageVtfAPI
{
	_QERPlugImageTable m_imagevtf;
public:
	typedef _QERPlugImageTable Type;
	STRING_CONSTANT( Name, "vtf" );

	ImageVtfAPI(){
		m_imagevtf.loadImage = LoadVtf;
	}
	_QERPlugImageTable* getTable(){
		return &m_imagevtf;
	}
};

typedef SingletonModule<ImageVtfAPI, ImageDependencies> ImageVtfModule;

ImageVtfModule g_ImageVtfModule;

class ImageTthAPI
{
	_QERPlugImageTable m_imagetth;
public:
	typedef _QERPlugImageTable Type;
	STRING_CONSTANT( Name, "tth" );

	ImageTthAPI(){
		m_imagetth.loadImage = LoadTth;
	}
	_QERPlugImageTable* getTable(){
		return &m_imagetth;
	}
};

typedef SingletonModule<ImageTthAPI, ImageDependencies> ImageTthModule;

ImageTthModule g_ImageTthModule;


class ImagePvrAPI
{
	_QERPlugImageTable m_imagepvr;
public:
	typedef _QERPlugImageTable Type;
	STRING_CONSTANT( Name, "pvr" );

	ImagePvrAPI(){
		m_imagepvr.loadImage = LoadPvr;
	}
	_QERPlugImageTable* getTable(){
		return &m_imagepvr;
	}
};

typedef SingletonModule<ImagePvrAPI, ImageDependencies> ImagePvrModule;

ImagePvrModule g_ImagePvrModule;


extern "C" void RADIANT_DLLEXPORT Radiant_RegisterModules( ModuleServer& server ){
	initialiseModule( server );

	g_ImageTGAModule.selfRegister();
	g_ImageJPGModule.selfRegister();
	g_ImagePNGModule.selfRegister();
	g_ImageBMPModule.selfRegister();
	g_ImagePCXModule.selfRegister();
	g_ImageVtfModule.selfRegister();
	g_ImageTthModule.selfRegister();
	g_ImagePvrModule.selfRegister();
#ifndef NO_DDS
	g_ImageDDSModule.selfRegister();
#endif
#ifndef NO_KTX
	g_ImageKTXModule.selfRegister();
#endif
#ifndef NO_CRN
	g_ImageCRNModule.selfRegister();
#endif
#ifndef NO_WEBP
	g_ImageWebpModule.selfRegister();
#endif
}
