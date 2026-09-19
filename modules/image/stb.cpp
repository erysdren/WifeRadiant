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

#include "stb.h"

#include <cstdlib>

#include "stb_image.hpp"

#include "ifilesystem.h"
#include "iarchive.h"
#include "idatastream.h"

#include "imagelib.h"

Image* LoadSTB( const byte* buffer, const int length ){

	int w = 0, h = 0;
	stbi_uc *pixels = stbi_load_from_memory(buffer, length, &w, &h, NULL, 4);

	if (!pixels)
	{
		globalWarningStream() << "LoadSTB: failed to load image" << '\n';
		return NULL;
	}

	auto *image = new RGBAImage( w, h );
	byte *dest = image->getRGBAPixels();

	memcpy(dest, pixels, w * h * 4);

	stbi_image_free(pixels);

	return image;
}

Image* LoadPNG( ArchiveFile& file ){
	ScopedArchiveBuffer buffer( file );
	return LoadSTB( buffer.buffer, buffer.length );
}

Image* LoadJPG( ArchiveFile& file ){
	ScopedArchiveBuffer buffer( file );
	return LoadSTB( buffer.buffer, buffer.length );
}
