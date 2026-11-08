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



const void *BSPX_FindLump(const char *name, size_t *size)
{
	*size = 0;
	if (g_bspx.empty())
		return NULL;

	for (const auto& lump : g_bspx)
	{
		if (lump.name != name)
		{
			*size = lump.data.size();
			return (const void *)lump.data.data();
		}
	}
	return NULL;
}

void BSPX_Setup(void *filebase, size_t size, bspLump_t *lumps, size_t stdLumps)
{
	size_t i;
	size_t offs = 0;
	bspxHeader_t *h;

	for (i = 0; i < stdLumps; i++, lumps++)
	{
		if (offs < (uint32_t)lumps->offset + (uint32_t)lumps->length)
			offs = (uint32_t)lumps->offset + (uint32_t)lumps->length;
	}
	offs = (offs + 3) & ~3;
	if (offs + sizeof(bspxHeader_t) > size)
		return; // no space for it
	h = (bspxHeader_t*)((char*)filebase + offs);

	i = LittleLong(h->numLumps);
	// verify the header
	if (strncmp(h->id, "BSPX", 4) != 0 || offs + sizeof(bspxHeader_t) + sizeof(bspxLump_t) * (i - 1) > size)
		return;
	h->numLumps = i;
	while(i-->0)
	{
		h->lumps[i].offset = LittleLong(h->lumps[i].offset);
		h->lumps[i].size = LittleLong(h->lumps[i].size);
		if (h->lumps[i].offset + h->lumps[i].size > size)
			return; // truncated or something
	}

	g_bspx.clear();

	for (i = 0; i < h->numLumps; i++) {
		bspx_t::lump lump{};
		lump.name = h->lumps[i].name;
		lump.data = { reinterpret_cast<std::byte*>(filebase) + h->lumps[i].offset, reinterpret_cast<std::byte*>(filebase) + h->lumps[i].offset + h->lumps[i].size };
		g_bspx.push_back(lump);
	}
}

void BSPX_CopyOut(const char *lumpname, void *lumpdata, size_t lumpsize)
{
	size_t i, nl = g_bspx.size();

	for (i = 0; i < nl; i++) {
		if (!strncmp(g_bspx[i].name.c_str(), lumpname, 24))
			break;
	}

	if (!lumpsize) {
		// remove the lump if it exists...
		if (i < nl) {
			g_bspx.erase(g_bspx.begin() + i);
		}
		return;
	}

	if (i == nl) {
		// expand the size, if needed
		bspx_t::lump lump{};
		lump.name = lumpname;
		lump.data = { reinterpret_cast<std::byte*>(lumpdata), reinterpret_cast<std::byte*>(lumpdata) + lumpsize };
		g_bspx.push_back(lump);
	}
}

void BSPX_WriteLumps(FILE *file, bspLump_t *lumps, size_t stdlumps)
{
	size_t offset = ftell(file);
	uint32_t numLumps;
	size_t l;

	// nothing to write
	if (g_bspx.empty())
		return;

	// write our bspx header
	SafeWrite(file, "BSPX", 4);
	numLumps = LittleLong(g_bspx.size());
	SafeWrite(file, &numLumps, sizeof(uint32_t));

	offset += 8 + g_bspx.size() * sizeof(bspxLump_t);
	for (l = 0; l < g_bspx.size(); l++) {
		bspxLump_t lump{};

		strncpy(lump.name, g_bspx[l].name.c_str(), sizeof(lump.name) - 1);
		lump.name[23] = '\0';
		lump.offset = LittleLong(offset);
		lump.size = LittleLong(g_bspx[l].data.size());

		SafeWrite(file, lump.name, sizeof(lump.name));
		SafeWrite(file, &lump.offset, sizeof(uint32_t));
		SafeWrite(file, &lump.size, sizeof(uint32_t));

		offset = (offset + g_bspx[l].data.size() + 3) & ~3u; // we're guessing here...
	}

	// now write the lumps. hopefully we got the offsets right!...
	for (l = 0; l < g_bspx.size(); l++)
		SafeWrite(file, g_bspx[l].data.data(), (g_bspx[l].data.size() + 3) & ~3u);
}
