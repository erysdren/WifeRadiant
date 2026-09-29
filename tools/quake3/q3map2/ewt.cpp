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

   -------------------------------------------------------------------------------

   This code has been altered significantly from its original form, to support
   several games based on the Quake III Arena engine, in the form of "Q3Map2."

   ------------------------------------------------------------------------------- */



/* dependencies */
#include "q3map2.h"

enum {
	EWT_QBSP,
	EWT_VIS,
	EWT_LIGHT
};

int qbsp_main(int argc, const char **argv);
int vis_main(int argc, const char **argv);
int light_main(int argc, const char **argv);

int EwtMain( Args& args ){

	int which;

	if ( args.takeFront( "qbsp" ) ) {
		which = EWT_QBSP;
	}

	else if ( args.takeFront( "vis" ) ) {
		which = EWT_VIS;
	}

	else if ( args.takeFront( "light" ) ) {
		which = EWT_LIGHT;
	}

	else {
		Error( "invalid ericw-tools submodule specified" );
	}

	try {
		switch (which) {
			case EWT_QBSP: return qbsp_main(0, NULL);
			case EWT_VIS: return vis_main(0, NULL);
			case EWT_LIGHT: return light_main(0, NULL);
		}
	} catch (const std::exception &e) {
		Error(e.what());
	}

	return 0;
}
