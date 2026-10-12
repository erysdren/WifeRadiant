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

#include <cstdint>

#define PAK_FILENAME_LEN (56)

struct pakheader_t
{
	char magic[4];       // Name of the new WAD format ("PACK")
	uint32_t diroffset; // Position of WAD directory from start of file
	uint32_t dirsize; // Number of entries * 0x40 (64 char)
};

static_assert(sizeof(pakheader_t) == 12);

struct pakentry_t
{
	char filename[PAK_FILENAME_LEN]; // Name of the file, Unix style, with extension, 56 chars, padded with '\0'.
	uint32_t offset; // Position of the entry in PACK file
	uint32_t size;   // Size of the entry in PACK file
};

static_assert(sizeof(pakentry_t) == 64);

struct dkpakentry_t
{
	char filename[PAK_FILENAME_LEN]; // name of the file
	uint32_t offset; // position of file data
	uint32_t size;   // uncompressed size of file data
	uint32_t compressedsize; // compressed size of file data
	uint32_t compressiontype; // 0 = uncompressed 1 = compressed
};

static_assert(sizeof(dkpakentry_t) == 72);
