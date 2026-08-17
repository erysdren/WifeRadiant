/*
   Copyright (C) 1999-2006 Id Software, Inc. and contributors.
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
 */

//-----------------------------------------------------------------------------
//
//
// DESCRIPTION:
// deal with in/out tasks, for either stdin/stdout or network/XML stream
//

#include "cmdlib.h"
#include "mathlib.h"
#include "polylib.h"
#include "inout.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>

#ifdef WIN32
#include <direct.h>
#include <windows.h>
#endif

// network broadcasting
#include "l_net/l_net.h"
#include "pugixml.hpp"

#ifdef WIN32
HWND hwndOut = NULL;
qboolean lookedForServer = false;
UINT wm_BroadcastCommand = -1;
#endif

socket_t *brdcst_socket;
netmessage_t msg;

qboolean verbose = false;

// our main document
// is streamed through the network to Radiant
// possibly written to disk at the end of the run
//++timo FIXME: need to be global, required when creating nodes?
pugi::xml_document* doc;
pugi::xml_node tree;

// some useful stuff
pugi::xml_node* xml_NodeForVec( vec3_t v ){
	auto* ret = new pugi::xml_node();
	char buf[1024];

	sprintf( buf, "%f %f %f", v[0], v[1], v[2] );
	ret->set_name( buf );
	ret->set_value( buf );
	return ret;
}

// send a node down the stream, add it to the document
void xml_SendNode( pugi::xml_node node ){
	// xmlBufferPtr xml_buf;
	auto xml_stream = std::stringstream();
	char xmlbuf[MAX_NETMESSAGE]; // we have to copy content from the xmlBufferPtr into an aux buffer .. that sucks ..
	// this index loops through the node buffer
	int pos = 0;

	// xmlAddChild( doc->children, node );
	doc->append_copy(node);

	if ( brdcst_socket ) {
		// xml_buf = xmlBufferCreate();
		// xmlNodeDump( xml_buf, doc, node, 0, 0 );
		doc->save( xml_stream );

		// the XML node might be too big to fit in a single network message
		// l_net library defines an upper limit of MAX_NETMESSAGE
		// there are some size check errors, so we use MAX_NETMESSAGE-10 to be safe
		// if the size of the buffer exceeds MAX_NETMESSAGE-10 we'll send in several network message
		xml_stream.seekg(0, std::ios::end);
		while ( xml_stream.tellg() > 0 )
		{
			xml_stream.seekg(pos, std::ios::beg);
			auto size = ( xml_stream.tellg() > MAX_NETMESSAGE - 10 ) ? static_cast<int>(xml_stream.tellg()) : MAX_NETMESSAGE - 10;
			xml_stream.readsome(xmlbuf, size);

			NMSG_Clear( &msg );
			NMSG_WriteString( &msg, xmlbuf );
			Net_Send( brdcst_socket, &msg );

			pos += size;
			xml_stream.seekg(0, std::ios::end);
		}
	}
}

void xml_Select( char *msg, int entitynum, int brushnum, qboolean bError ){
	char buf[1024];
	char level[2];
	level[0] = (int)'0' + ( bError ? SYS_ERR : SYS_WRN )  ;
	level[1] = 0;

	// now build a proper "select" XML node
	sprintf( buf, "Entity %i, Brush %i: %s", entitynum, brushnum, msg );
	auto* node = new pugi::xml_node();
	node->set_name( "select" );
	node->set_value( buf );
	node->append_attribute( "level" ) = level;

	sprintf( buf, "%i %i", entitynum, brushnum );
	pugi::xml_node select = node->append_child( "brush" );
	select.set_name( "brush" );
	select.set_value( buf );

	xml_SendNode( *node );

	sprintf( buf, "Entity %i, Brush %i: %s", entitynum, brushnum, msg );
	if ( bError ) {
		Error( buf );
	}
	else{
		Sys_FPrintf( SYS_NOXML, "%s\n", buf );
	}

}

void xml_Point( char *msg, vec3_t pt ){
	char buf[1024];
	char level[2];
	level[0] = (int)'0' + SYS_ERR;
	level[1] = 0;

	auto* node = new pugi::xml_node();
	node->set_name( "pointmsg" );
	node->set_value( msg );
	node->append_attribute( "level" ) = level;

	sprintf( buf, "%g %g %g", pt[0], pt[1], pt[2] );
	pugi::xml_node point = node->append_child( "point" );
	point.set_name( "point" );
	point.set_value( buf );

	xml_SendNode( *node );

	sprintf( buf, "%s (%g %g %g)", msg, pt[0], pt[1], pt[2] );
	Error( buf );
}

#define WINDING_BUFSIZE 2048
void xml_Winding( char *msg, vec3_t p[], int numpoints, qboolean die ){
	char buf[WINDING_BUFSIZE];
	char smlbuf[128];
	char level[2];
	level[0] = (int)'0' + SYS_ERR;
	level[1] = 0;
	int i;

	auto* node = new pugi::xml_node();
	node->set_name( "windingmsg" );
	node->set_value( msg );
	node->append_attribute( "level" ) = level;

	sprintf( buf, "%i ", numpoints );
	for ( i = 0; i < numpoints; i++ )
	{
		sprintf( smlbuf, "(%g %g %g)", p[i][0], p[i][1], p[i][2] );
		// don't overflow
		if ( strlen( buf ) + strlen( smlbuf ) > WINDING_BUFSIZE ) {
			break;
		}
		strcat( buf, smlbuf );
	}

	pugi::xml_node winding = node->append_child( "winding" );
	winding.set_value( buf );

	xml_SendNode( *node );

	if ( die ) {
		Error( msg );
	}
	else
	{
		Sys_Printf( msg );
		Sys_Printf( "\n" );
	}
}

// in include
#include "stream_version.h"

void Broadcast_Setup( const char *dest ){
	address_t address;
	char sMsg[1024];

	Net_Setup();
	Net_StringToAddress( dest, &address );
	brdcst_socket = Net_Connect( &address, 0 );
	if ( brdcst_socket ) {
		// send in a header
		sprintf( sMsg, "<?xml version=\"1.0\"?><q3map_feedback version=\"" Q3MAP_STREAM_VERSION "\">" );
		NMSG_Clear( &msg );
		NMSG_WriteString( &msg, sMsg );
		Net_Send( brdcst_socket, &msg );
	}
}

void Broadcast_Shutdown(){
	if ( brdcst_socket ) {
		Sys_Printf( "Disconnecting\n" );
		Net_Disconnect( brdcst_socket );
		brdcst_socket = NULL;
	}
}

// all output ends up through here
void FPrintf( int flag, char *buf ){
	static qboolean bGotXML = false;
	char level[2];
	level[0] = (int)'0' + flag;
	level[1] = 0;

	printf( "%s", buf );

	// the following part is XML stuff only.. but maybe we don't want that message to go down the XML pipe?
	if ( flag == SYS_NOXML ) {
		return;
	}

	// output an XML file of the run
	// use the DOM interface to build a tree
	/*
	   <message level='flag'>
	   message string
	   .. various nodes to describe corresponding geometry ..
	   </message>
	 */
	if ( !bGotXML ) {
		// initialize
		doc = new pugi::xml_document();
		doc->append_child( "q3map_feedback" );
	}

	auto* node = new pugi::xml_node();
	node->set_name( "message" );
	node->set_value( buf );
	node->append_attribute( "level" ) = level;

	xml_SendNode( *node );
}

#ifdef DBG_XML
void DumpXML(){
	doc->save_file( "XMLDump.xml" );
}
#endif

void Sys_FPrintf( int flag, const char *format, ... ){
	char out_buffer[4096];
	va_list argptr;

	if ( ( flag == SYS_VRB ) && ( verbose == false ) ) {
		return;
	}

	va_start( argptr, format );
	vsprintf( out_buffer, format, argptr );
	va_end( argptr );

	FPrintf( flag, out_buffer );
}

void Sys_Printf( const char *format, ... ){
	char out_buffer[4096];
	va_list argptr;

	va_start( argptr, format );
	vsprintf( out_buffer, format, argptr );
	va_end( argptr );

	FPrintf( SYS_STD, out_buffer );
}

/*
   =================
   Error

   For abnormal program terminations
   =================
 */
void Error( const char *error, ... ){
	char out_buffer[4096];
	char tmp[4096];
	va_list argptr;

	va_start( argptr,error );
	vsprintf( tmp, error, argptr );
	va_end( argptr );

	sprintf( out_buffer, "************ ERROR ************\n%s\n", tmp );

	FPrintf( SYS_ERR, out_buffer );

#ifdef DBG_XML
	DumpXML();
#endif

	//++timo HACK ALERT .. if we shut down too fast the xml stream won't reach the listener.
	// a clean solution is to send a sync request node in the stream and wait for an answer before exiting
	Sys_Sleep( 1000 );

	Broadcast_Shutdown();

	exit( 1 );
}
