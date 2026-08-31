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
#include "pugixml.hpp"



/*
   ==============================================================================

   LEAK FILE GENERATION

   Save out name.line for qe3 to read
   ==============================================================================
 */


/*
   =============
   LeakFile

   Finds the shortest possible chain of portals
   that leads from the outside leaf to a specifically
   occupied leaf

   TTimo: builds a polyline xml node
   =============
 */
static pugi::xml_node* LeakFile( const tree_t& tree ){
	Vector3 mid;
	FILE    *linefile;
	const node_t  *node;
	int count;

	if ( !tree.outside_node.occupied ) {
		return nullptr;
	}

	Sys_FPrintf( SYS_VRB, "--- LeakFile ---\n" );

	//
	// write the points to the file
	//
	const auto filename = StringStream( source, ".lin" );
	linefile = SafeOpenWrite( filename, "wt" );

	auto* xmlnode = new pugi::xml_node();
	xmlnode->set_name( "polyline" );

	count = 0;
	node = &tree.outside_node;
	while ( node->occupied > 1 )
	{
		int next = node->occupied;
		const portal_t    *nextportal = nullptr;
		const node_t      *nextnode = nullptr;

		// find the best portal exit
		ESide side;
		for ( const portal_t *p = node->portals; p; p = p->next[!side] )
		{
			side = ( p->nodes[eFront] == node );
			if ( p->nodes[side]->occupied
			  && p->nodes[side]->occupied < next ) {
				nextportal = p;
				nextnode = p->nodes[side];
				next = nextnode->occupied;
			}
		}
		node = nextnode;
		mid = WindingCenter( nextportal->winding );
		fprintf( linefile, "%f %f %f\n", mid[0], mid[1], mid[2] );
		auto point = xml_NodeForVec( mid );
		xmlnode->append_copy( point );
		count++;
	}
	// add the occupant center
	mid = node->occupant->vectorForKey( "origin" );

	fprintf( linefile, "%f %f %f\n", mid[0], mid[1], mid[2] );
	auto point = xml_NodeForVec( mid );
	xmlnode->append_copy( point );
	Sys_FPrintf( SYS_VRB, "%9d point linefile\n", count + 1 );

	fclose( linefile );

	xml_Select( "Entity leaked", node->occupant->mapEntityNum, 0, false );

	return xmlnode;
}

void Leak_feedback( const tree_t& tree ){
	Sys_FPrintf( SYS_NOXMLflag | SYS_ERR, "**********************\n" );
	Sys_FPrintf( SYS_NOXMLflag | SYS_ERR, "******* leaked *******\n" );
	Sys_FPrintf( SYS_NOXMLflag | SYS_ERR, "**********************\n" );
	pugi::xml_node* polyline = LeakFile( tree );
	auto leaknode = polyline->append_child( "message" );
	leaknode.set_value( "MAP LEAKED\n" );
	char level[ 2 ];
	level[0] = (int) '0' + SYS_ERR;
	level[1] = 0;
	leaknode.append_attribute( "level" ) = level;

	xml_SendNode( leaknode );
}
