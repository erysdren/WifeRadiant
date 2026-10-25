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

#include "dds.h"

#include <cstdlib>

#include "ifilesystem.h"
#include "iarchive.h"
#include "idatastream.h"

#include "ddslib.h"
#include "imagelib.h"

#include <vtfpp/vtfpp.h>

Image* LoadDDSBuff( const byte* buffer, size_t length ){
	int width, height;
	ddsPF_t pixelFormat;
	const ddsBuffer_t* dds = reinterpret_cast<const ddsBuffer_t*>( buffer );
	if ( DDSGetInfo( dds, &width, &height, &pixelFormat ) == -1 ) {
		return nullptr;
	}

	vtfpp::ImageFormat format;

	switch (pixelFormat) {
		case DDS_PF_ARGB8888:
			format = vtfpp::ImageFormat::ABGR8888;
			break;
		case DDS_PF_DXT1:
			format = vtfpp::ImageFormat::DXT1;
			break;
		case DDS_PF_DXT2:
		case DDS_PF_DXT3:
			format = vtfpp::ImageFormat::DXT3;
			break;
		case DDS_PF_DXT4:
		case DDS_PF_DXT5:
			format = vtfpp::ImageFormat::DXT5;
			break;
		case DDS_PF_UNKNOWN:
			globalErrorStream() << __func__ << ": unknown pixel format" << '\n';
			return nullptr;
	}

	auto *image = new RGBAImage( width, height );

	const std::byte* data = reinterpret_cast<const std::byte*>(dds->data);
	size_t dataLength = length - offsetof(ddsBuffer_t, data);

	auto convertedData = vtfpp::ImageConversion::convertImageDataToFormat({data, data + dataLength}, format, vtfpp::ImageFormat::RGBA8888, width, height);
	memcpy(image->getRGBAPixels(), convertedData.data(), width * height * 4);

	return image;
}

Image* LoadDDS( ArchiveFile& file ){
	ScopedArchiveBuffer buffer( file );
	return LoadDDSBuff( buffer.buffer, buffer.length );
}
