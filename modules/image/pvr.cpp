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

#include "pvr.h"

#include "ifilesystem.h"

#include "os/path.h"
#include "stream/stringstream.h"
#include "bytestreamutils.h"
#include "imagelib.h"

typedef struct gbix {
	uint32_t magic;
	uint32_t len;
} gbix_t;

typedef struct pvr {
	uint32_t magic;
	uint32_t len_file;
	uint32_t type;
	uint16_t width;
	uint16_t height;
} pvr_t;

typedef uint32_t (*pvr_pixel_func_t)(uint16_t color);
typedef void (*pvr_image_func_t)(Image* image, pvr_t *pvr, int offset, bool detwiddle, pvr_pixel_func_t pixel_func);

enum {
	PVR_PIXEL_TYPE_ARGB1555 = 0,
	PVR_PIXEL_TYPE_RGB565 = 1,
	PVR_PIXEL_TYPE_ARGB4444 = 2
};

enum {
	PVR_IMAGE_TYPE_TWIDDLED = 1,
	PVR_IMAGE_TYPE_TWIDDLED_MM = 2,
	PVR_IMAGE_TYPE_VQ = 3,
	PVR_IMAGE_TYPE_VQ_MM = 4,
	PVR_IMAGE_TYPE_RECTANGULAR = 9,
	PVR_IMAGE_TYPE_RECTANGULAR_MM = 10
};

static int pvr_log2(int x)
{
	switch (x)
	{
		case 8: return 3;
		case 16: return 4;
		case 32: return 5;
		case 64: return 6;
		case 128: return 7;
		case 256: return 8;
		case 512: return 9;
		case 1024: return 10;
		default: return -1;
	}
}

static int pvr_detwiddle(int x, int y, int w, int h)
{
	int wmax, hmax;
	int i, idx = 0;

	wmax = pvr_log2(w);
	hmax = pvr_log2(h);

	if (wmax < 0 || hmax < 0)
		return -1;

	for (i = 0; i < 10; i++)
	{
		if (i < wmax && i < hmax)
		{
			idx |= ((y >> i) & 1) << (i * 2 + 0);
			idx |= ((x >> i) & 1) << (i * 2 + 1);
		}
		else if (i < wmax)
		{
			idx |= ((x >> i) & 1) << (i + hmax);
		}
		else if (i < hmax)
		{
			idx |= ((y >> i) & 1) << (i + wmax);
		}
		else
		{
			break;
		}
	}

	return idx;
}

static uint32_t argb1555_to_rgba8888(uint16_t color)
{
	uint8_t r, g, b, a;
	r = ((color >> 10) & 31) << 4;
	g = ((color >> 5) & 31) << 4;
	b = ((color >> 0) & 31) << 4;
	a = ((color >> 15) & 1) * 255;
	return (a << 24) | (b << 16) | (g << 8) | r;
}

static uint32_t rgb565_to_rgba8888(uint16_t color)
{
	uint8_t r, g, b, a;
	r = ((color >> 11) & 31) << 3;
	g = ((color >> 5) & 63) << 2;
	b = ((color >> 0) & 31) << 3;
	a = 255;
	return (a << 24) | (b << 16) | (g << 8) | r;
}

static uint32_t argb4444_to_rgba8888(uint16_t color)
{
	uint8_t r, g, b, a;
	r = ((color >> 8) & 15) << 4;
	g = ((color >> 4) & 15) << 4;
	b = ((color >> 0) & 15) << 4;
	a = ((color >> 12) & 15) | 0xF;
	return (a << 24) | (b << 16) | (g << 8) | r;
}

static int pvr_mm_offset(int w)
{
	switch (w)
	{
		case 1: return 0x00006;
		case 2: return 0x00008;
		case 4: return 0x00010;
		case 8: return 0x00030;
		case 16: return 0x000B0;
		case 32: return 0x002B0;
		case 64: return 0x00AB0;
		case 128: return 0x02AB0;
		case 256: return 0x0AAB0;
		case 512: return 0x2AAB0;
		case 1024: return 0xAAAB0;
		default: return -1;
	}
}

static int pvr_mm_offset_vq(int w)
{
	switch (w)
	{
		case 1: return 0x00000;
		case 2: return 0x00001;
		case 4: return 0x00002;
		case 8: return 0x00006;
		case 16: return 0x00016;
		case 32: return 0x00056;
		case 64: return 0x00156;
		case 128: return 0x00556;
		case 256: return 0x01556;
		case 512: return 0x05556;
		case 1024: return 0x15556;
		default: return -1;
	}
}

static void decode_mm(Image* image, pvr_t *pvr, pvr_image_func_t image_func, pvr_pixel_func_t pixel_func, bool vq, bool detwiddle)
{
	int offset = vq ? pvr_mm_offset_vq(pvr->width) : pvr_mm_offset(pvr->width);
	image_func(image, pvr, offset, detwiddle, pixel_func);
}

static void decode(Image* image, pvr_t *pvr, int offset, bool detwiddle, pvr_pixel_func_t pixel_func)
{
	int x, y;
	uint32_t *rgba32;

	rgba32 = (uint32_t *)image->getRGBAPixels();

	for (y = 0; y < pvr->height; y++)
	{
		for (x = 0; x < pvr->width; x++)
		{
			uint16_t color;
			int ofs;
			if (detwiddle)
				ofs = offset + pvr_detwiddle(x, y, pvr->width, pvr->height) * 2;
			else
				ofs = offset + (y * pvr->width + x) * 2;

			if (ofs < 0)
			{
				globalErrorStream() << "LoadPvr: decode ofs < 0" << '\n';
				return;
			}

			color = *(uint16_t *)(((uint8_t *)(pvr + 1)) + ofs);

			rgba32[y * pvr->width + x] = pixel_func(color);
		}
	}
}

static void decode_vq(Image* image, pvr_t *pvr, int offset, bool detwiddle, pvr_pixel_func_t pixel_func)
{
	int x, y;
	uint32_t *rgba32;
	uint16_t *codebook;
	uint8_t *indices;

	rgba32 = (uint32_t *)image->getRGBAPixels();

	codebook = (uint16_t *)(pvr + 1);
	indices = ((uint8_t *)(codebook + 1024)) + offset;

	for (y = 0; y < pvr->height / 2; y++)
	{
		for (x = 0; x < pvr->width / 2; x++)
		{
			uint16_t *colors;
			int a, b, c, d;
			int idx;
			if (detwiddle)
				idx = pvr_detwiddle(x, y, pvr->width, pvr->height);
			else
				idx = y * (pvr->width / 2) + x;

			if (idx < 0)
			{
				globalErrorStream() << "LoadPvr: decode_vq idx < 0" << '\n';
				return;
			}

			colors = &codebook[indices[idx] * 4];

			a = ((y * 2) + 0) * pvr->width + ((x * 2) + 0);
			b = ((y * 2) + 1) * pvr->width + ((x * 2) + 0);
			c = ((y * 2) + 0) * pvr->width + ((x * 2) + 1);
			d = ((y * 2) + 1) * pvr->width + ((x * 2) + 1);

			rgba32[a] = pixel_func(colors[0]);
			rgba32[b] = pixel_func(colors[1]);
			rgba32[c] = pixel_func(colors[2]);
			rgba32[d] = pixel_func(colors[3]);
		}
	}
}

// FIXME: endian
#define LittleLong(x) x
#define LittleShort(x) x

Image* LoadPvr( ArchiveFile& file )
{
	uint8_t *ptr;
	pvr_t *pvr;
	int pixel_type, image_type;
	pvr_pixel_func_t pixel_func;

	ScopedArchiveBuffer buffer( file );

	ptr = (uint8_t *)(buffer.buffer);

	// skip global index
	if (memcmp(ptr, "GBIX", 4) == 0)
	{
		gbix_t *gbix = (gbix_t *)ptr;
		ptr += sizeof(gbix_t) + LittleLong(gbix->len);
	}

	// check magic identifier
	if (memcmp(ptr, "PVRT", 4) != 0)
	{
		globalErrorStream() << "LoadPvr: magic identifier does not match expected" << '\n';
		return nullptr;
	}

	// fix up header
	pvr = (pvr_t *)ptr;
	pvr->len_file = LittleLong(pvr->len_file);
	pvr->type = LittleLong(pvr->type);
	pvr->width = LittleShort(pvr->width);
	pvr->height = LittleShort(pvr->height);

	// check width value is valid
	if (pvr->width < 8 || pvr->width > 1024 || (pvr->width & (pvr->width - 1)) != 0)
	{
		globalErrorStream() << "LoadPvr: image width value is not valid" << '\n';
		return nullptr;
	}

	// check height value is valid
	if (pvr->height < 8 || pvr->height > 1024 || (pvr->height & (pvr->height - 1)) != 0)
	{
		globalErrorStream() << "LoadPvr: image height value is not valid" << '\n';
		return nullptr;
	}

	// break out type values
	pixel_type = pvr->type & 0xFF;
	image_type = (pvr->type & 0xFF00) >> 8;

	// get pixel function
	switch (pixel_type)
	{
		case PVR_PIXEL_TYPE_ARGB1555:
		{
			pixel_func = argb1555_to_rgba8888;
			break;
		}
		case PVR_PIXEL_TYPE_RGB565:
		{
			pixel_func = rgb565_to_rgba8888;
			break;
		}
		case PVR_PIXEL_TYPE_ARGB4444:
		{
			pixel_func = argb4444_to_rgba8888;
			break;
		}
		default:
		{
			globalErrorStream() << "LoadPvr: unsupported pixel type " << pixel_type << '\n';
			return nullptr;
		}
	}

	auto* image = new RGBAImageFlags( pvr->width, pvr->height, 0, 0, 0 );

	// decompress image
	switch (image_type)
	{
		case PVR_IMAGE_TYPE_TWIDDLED:
		{
			decode(image, pvr, 0, true, pixel_func);
			break;
		}
		case PVR_IMAGE_TYPE_TWIDDLED_MM:
		{
			decode_mm(image, pvr, decode, pixel_func, false, true);
			break;
		}
		case PVR_IMAGE_TYPE_VQ:
		{
			decode_vq(image, pvr, 0, true, pixel_func);
			break;
		}
		case PVR_IMAGE_TYPE_VQ_MM:
		{
			decode_mm(image, pvr, decode_vq, pixel_func, true, true);
			break;
		}
		case PVR_IMAGE_TYPE_RECTANGULAR:
		{
			decode(image, pvr, 0, false, pixel_func);
			break;
		}
		case PVR_IMAGE_TYPE_RECTANGULAR_MM:
		{
			decode_mm(image, pvr, decode, pixel_func, false, false);
			break;
		}
		default:
		{
			globalErrorStream() << "LoadPvr: unsupported image type " << image_type << '\n';
			image->release();
			return nullptr;
		}
	}

	return image;
}
