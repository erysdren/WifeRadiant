/* -------------------------------------------------------------------------------

   Copyright (C) 1999-2007 id Software, Inc. and contributors.
   For a list of contributors, see the accompanying CONTRIBUTORS file.

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

   ----------------------------------------------------------------------------------

   This code has been altered significantly from its original form, to support
   several games based on the Quake III Arena engine, in the form of "Q3Map2."

   ------------------------------------------------------------------------------- */



/* dependencies */
#include "q3map2.h"
#include "ddslib.h"
#include "crnlib/crnlib.h"
#include "webplib/webplib.h"

#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/* -------------------------------------------------------------------------------

   this file contains image pool management with reference counting. note: it isn't
   reentrant, so only call it from init/shutdown code or wrap calls in a mutex

   ------------------------------------------------------------------------------- */

/*
   LoadDDSBuffer()
   loads a dxtc (1, 3, 5) dds buffer into a valid rgba image
 */

static void LoadDDSBuffer( byte *buffer, int size, byte **pixels, int *width, int *height ){
	int w, h;
	ddsPF_t pf;


	/* dummy check */
	if ( buffer == nullptr || size <= 0 || pixels == nullptr || width == nullptr || height == nullptr ) {
		return;
	}

	/* null out */
	*pixels = 0;
	*width = 0;
	*height = 0;

	/* get dds info */
	if ( DDSGetInfo( (ddsBuffer_t*) buffer, &w, &h, &pf ) ) {
		Sys_Warning( "Invalid DDS texture\n" );
		return;
	}

	/* only certain types of dds textures are supported */
	if ( pf != DDS_PF_ARGB8888 && pf != DDS_PF_DXT1 && pf != DDS_PF_DXT3 && pf != DDS_PF_DXT5 ) {
		Sys_Warning( "Only DDS texture formats ARGB8888, DXT1, DXT3, and DXT5 are supported (%d)\n", pf );
		return;
	}

	/* create image pixel buffer */
	*width = w;
	*height = h;
	*pixels = safe_malloc( w * h * 4 );

	/* decompress the dds texture */
	DDSDecompress( (ddsBuffer_t*) buffer, *pixels );
}



#ifndef NO_CRN
/*
    LoadCRNBuffer
    loads a crn image into a valid rgba image
*/
void LoadCRNBuffer( byte *buffer, int size, byte **pixels, int *width, int *height ) {
	/* dummy check */
	if ( buffer == nullptr || size <= 0 || pixels == nullptr || width == nullptr || height == nullptr ) {
		return;
	}
	if ( !GetCRNImageSize( buffer, size, width, height ) ) {
		Sys_Warning( "Error getting crn image dimensions.\n" );
		return;
	}
	const unsigned int outBufSize = *width * *height * 4;
	*pixels = safe_malloc( outBufSize );
	if ( !ConvertCRNtoRGBA( buffer, size, outBufSize, *pixels ) ) {
		Sys_Warning( "Error decoding crn image.\n" );
	}
}
#endif



/*
    LoadSTBBuffer
    loads a png or jpeg image into a valid rgba image
*/
int LoadSTBBuffer( byte *buffer, int size, byte **pixels, int *width, int *height ) {
	/* dummy check */
	if ( buffer == nullptr || size <= 0 || pixels == nullptr || width == nullptr || height == nullptr ) {
		return -1;
	}

	stbi_uc *p = stbi_load_from_memory(buffer, size, width, height, NULL, 4);

	if (!p)
		return -1;

	*pixels = safe_malloc(*width * *height * 4);
	memcpy(*pixels, p, *width * *height * 4);

	stbi_image_free(p);

	return 0;
}



/*
   LoadJPGBuff()
   loads a jpeg file buffer into a valid rgba image
 */

static int LoadJPGBuff( byte *buffer, int size, byte **pixels, int *width, int *height ){
	return LoadSTBBuffer(buffer, size, pixels, width, height);
}



/*
   LoadPNGBuffer()
   loads a png file buffer into a valid rgba image
 */

static int LoadPNGBuffer( byte *buffer, int size, byte **pixels, int *width, int *height ){
	return LoadSTBBuffer(buffer, size, pixels, width, height);
}



static std::forward_list<image_t> images;

static struct construct_default_image
{
	construct_default_image(){
		images.emplace_front( DEFAULT_IMAGE, DEFAULT_IMAGE, 64, 64, void_ptr( memset( safe_malloc( 64 * 64 * 4 ), 255, 64 * 64 * 4 ) ) );
	}
} s_construct_default_image;

/*
   ImageFind()
   finds an existing rgba image and returns a pointer to the image_t struct or NULL if not found
   name is name without extension, as in images[ i ].name
 */

static const image_t *ImageFind( const char *name ){
	/* dummy check */
	if ( strEmptyOrNull( name ) ) {
		return nullptr;
	}

	/* search list */
	for ( const auto& img : images )
	{
		if ( striEqual( name, img.name.c_str() ) ) {
			return &img;
		}
	}

	/* no matching image found */
	return nullptr;
}



/*
   ImageLoad()
   loads an rgba image and returns a pointer to the image_t struct or NULL if not found
   expects extensionless path as input
 */

const image_t *ImageLoad( const char *name ){
	/* dummy check */
	if ( strEmptyOrNull( name ) ) {
		return nullptr;
	}

	/* try to find existing image */
	if ( const auto *img = ImageFind( name ) ) {
		return img;
	}

	/* none found, so let's create a new one */
	byte *pixels = nullptr;
	int width, height;
	char filename[ 1024 ];
	MemBuffer buffer;
	bool alphaHack = false;

	/* attempt to load various formats */
	if ( snprintf( filename, sizeof(filename), "%s.tga", name ); ( buffer = vfsLoadFile( filename ) ) ) // StripExtension( name ); already
	{
		LoadTGABuffer( buffer.data(), buffer.size(), &pixels, &width, &height );
	}
	else if( path_set_extension( filename, ".png" ); ( buffer = vfsLoadFile( filename ) ) )
	{
		LoadPNGBuffer( buffer.data(), buffer.size(), &pixels, &width, &height );
	}
	else if( path_set_extension( filename, ".jpg" ); ( buffer = vfsLoadFile( filename ) ) )
	{
		if ( LoadJPGBuff( (byte *)buffer.data(), buffer.size(), &pixels, &width, &height ) == -1 && pixels != nullptr ) {
			// On error, LoadJPGBuff might store a pointer to the error message in pixels
			Sys_Warning( "LoadJPGBuff %s %s\n", filename, (unsigned char*) pixels );
			pixels = nullptr;
		}
		alphaHack = true;
	}
	else if( path_set_extension( filename, ".dds" ); ( buffer = vfsLoadFile( filename ) ) )
	{
		LoadDDSBuffer( buffer.data(), buffer.size(), &pixels, &width, &height );
		/* debug code */
		#if 0
		{
			ddsPF_t pf;
			DDSGetInfo( (ddsBuffer_t*) buffer, nullptr, nullptr, &pf );
			Sys_Printf( "pf = %d\n", pf );
			if ( width > 0 ) {
				path_set_extension( filename, "_converted.tga" );
				WriteTGA( "C:\\games\\quake3\\baseq3\\textures\\rad\\dds_converted.tga", pixels, width, height );
			}
		}
		#endif
	}
	else if( path_set_extension( filename, ".ktx" ); ( buffer = vfsLoadFile( filename ) ) )
	{
		LoadKTXBufferFirstImage( buffer.data(), buffer.size(), &pixels, &width, &height );
	}
#ifndef NO_CRN
	else if( path_set_extension( filename, ".crn" ); ( buffer = vfsLoadFile( filename ) ) )
	{
		LoadCRNBuffer( buffer.data(), buffer.size(), &pixels, &width, &height );
	}
#endif
#ifndef NO_WEBP
	else if( path_set_extension( filename, ".webp" ); ( buffer = vfsLoadFile( filename ) ) )
	{
		pixels = ConvertWebptoRGBA( buffer.data(), buffer.size(), width, height );
	}
#endif

	/* make sure everything's kosher */
	if ( !buffer || width <= 0 || height <= 0 || pixels == nullptr ) {
		//%	Sys_Printf( "size = %zu  width = %d  height = %d  pixels = 0x%08x (%s)\n",
		//%		buffer.size(), width, height, pixels, filename );
		return nullptr;
	}

	/* everybody's in the place, create new image */
	image_t& image = *images.emplace_after( images.cbegin(), name, filename, width, height, pixels );

	if ( alphaHack ) {
		if ( path_set_extension( filename, "_alpha.jpg" ); ( buffer = vfsLoadFile( filename ) ) ) {
			if ( LoadJPGBuff( (byte *)buffer.data(), buffer.size(), &pixels, &width, &height ) == -1 ) {
				if ( pixels ) {
					// On error, LoadJPGBuff might store a pointer to the error message in pixels
					Sys_Warning( "LoadJPGBuff %s %s\n", filename, (unsigned char*) pixels );
				}
			} else {
				if ( width == image.width && height == image.height ) {
					for ( int i = 0; i < width * height; ++i )
						image.pixels[4 * i + 3] = pixels[4 * i + 2];  // copy alpha from blue channel
				}
				free( pixels );
			}
		}
	}

	/* return the image */
	return &image;
}
