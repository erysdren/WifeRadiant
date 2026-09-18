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
#include "inout.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <algorithm>
#include "generic/vector.h"
#include "timer.h"
#include <thread>
#include <mutex>
#include <cstring>
#include "stdarg.h"

#ifdef WIN32
#include <direct.h>
#include <windows.h>
#endif

// network broadcasting
#include "l_net/l_net.h"
#include "pugixml.hpp"

static socket_t *brdcst_socket;

// locks xml doc and mesege_*
// messages may come from various threads, 'force send' signal comes from dedicated thread
static std::recursive_mutex mesege_mutex;

bool verbose = false;

// our main document
// is streamed through the network to Radiant
// possibly written to disk at the end of the run
//++timo FIXME: need to be global, required when creating nodes?
static pugi::xml_document* doc;

// some useful stuff
pugi::xml_node* xml_NodeForVec( const Vector3& v ){
	auto* ret = new pugi::xml_node();
	char buf[1024];

	sprintf( buf, "%f %f %f", v[0], v[1], v[2] );
	ret->set_name( buf );
	ret->set_value( buf );
	return ret;
}


static void xml_message_flush();

// send a node down the stream, add it to the document
void xml_SendNode( pugi::xml_node node ){
	auto xml_stream = std::stringstream();
	char xmlbuf[MAX_NETMESSAGE];
	int pos = 0;
	std::lock_guard lock( mesege_mutex );

	xml_message_flush(); /* flush regular print messages buffer, so that special ones will appear at correct spot */

	doc->append_copy( node );

	if ( brdcst_socket ) {
		doc->save( xml_stream );

		// the XML node might be too big to fit in a single network message
		// l_net library defines an upper limit of MAX_NETMESSAGE
		// there are some size check errors, so we use MAX_NETMESSAGE-10 to be safe
		// if the size of the buffer exceeds MAX_NETMESSAGE-10 we'll send in several network messages
		xml_stream.seekg( 0, std::ios::end );
		while ( xml_stream.tellg() > 0 )
		{
			xml_stream.seekg( pos, std::ios::beg );
			auto size = ( xml_stream.tellg() > MAX_NETMESSAGE - 10 ) ? static_cast<int>(xml_stream.tellg()) : MAX_NETMESSAGE - 10;
			xml_stream.readsome( xmlbuf, size );

			netmessage_t msg;
			NMSG_Clear( &msg );
			NMSG_WriteString_n( &msg, xmlbuf, size );
			Net_Send( brdcst_socket, &msg );
			// now that the thing is sent prepare to loop again
			pos += size;
			xml_stream.seekg( 0, std::ios::end );
		}
	}
}

void xml_Select( const char *msg, int entitynum, int brushnum, bool bError ){
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
	select.set_value( buf );

	xml_SendNode( *node );

	sprintf( buf, "Entity %i, Brush %i: %s", entitynum, brushnum, msg );
	if ( bError ) {
		Error( buf );
	}
	else{
		Sys_FPrintf( SYS_NOXMLflag, "%s\n", buf );
	}
}

void xml_Point( char *msg, const Vector3& pt ){
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

	snprintf( buf, sizeof(buf), "%s (%g %g %g)", msg, pt[0], pt[1], pt[2] );
	Error( buf );
}

#define WINDING_BUFSIZE 2048
void xml_Winding( const char *msg, const Vector3 p[], int numpoints, bool die ){
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

static void set_console_colour_for_flag( int flag ){
#ifdef WIN32
	static int curFlag = SYS_STD;
	static bool ok = true;
	static bool initialized = false;
	static HANDLE hConsole;
	static WORD colour_saved;
	if( !ok )
		return;
	if( !initialized ){
		hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		if( hConsole == INVALID_HANDLE_VALUE || !GetConsoleScreenBufferInfo( hConsole, &consoleInfo ) ){
			ok = false;
			return;
		}
		colour_saved = consoleInfo.wAttributes;
		initialized = true;
	}
	if( curFlag != flag ){
		curFlag = flag;
		SetConsoleTextAttribute( hConsole, flag == SYS_WRN ? FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
		                                 : flag == SYS_ERR ? FOREGROUND_RED | FOREGROUND_INTENSITY
		                                 : colour_saved );
	}
#endif
}


#define MAX_MESEGE      MAX_NETMESSAGE / 2
static char mesege[MAX_MESEGE];
static size_t mesege_len = 0;
static int mesege_flag = SYS_STD;
static Timer mesege_send_timer;


// in include
#include "stream_version.h"

void Broadcast_Setup( const char *dest ){
	address_t address;

	Net_Setup();
	Net_StringToAddress( dest, &address );
	brdcst_socket = Net_Connect( &address, 0 );
	if ( brdcst_socket ) {
		// send in a header
		const char string[] = "<?xml version=\"1.0\"?><q3map_feedback version=\"" Q3MAP_STREAM_VERSION "\">";
		netmessage_t msg;
		NMSG_Clear( &msg );
		NMSG_WriteString( &msg, string );
		Net_Send( brdcst_socket, &msg );

		std::thread ( [](){
			while( true ){
				std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
				std::lock_guard lock( mesege_mutex );
				if( mesege_send_timer.elapsed_msec() + 1000 * mesege_len / MAX_MESEGE > 2000 ){ // force send if >1-2 seconds has passed
					xml_message_flush();
				}
			}
		} ).detach();
	}
}

void Broadcast_Shutdown(){
	if ( brdcst_socket ) {
		Sys_Printf( "Disconnecting\n" );
		xml_message_flush();
		Net_Disconnect( brdcst_socket );
		brdcst_socket = nullptr;
	}
	set_console_colour_for_flag( SYS_STD ); //restore default on exit
}

static void xml_message_flush(){
	std::lock_guard lock( mesege_mutex );

	if( mesege_len == 0 )
		return;

	pugi::xml_node* node = new pugi::xml_node();
	node->set_name( "message" );
	{
		mesege[mesege_len] = '\0';
		mesege_len = 0;
#if 0 // FIXME: replace this - erysdren
		gchar* utf8 = g_locale_to_utf8( mesege, -1, nullptr, nullptr, nullptr );
		node->set_value( (const char*)utf8 );
		g_free( utf8 );
#else
		node->set_value( mesege );
#endif
	}
	char level[2];
	level[0] = (int)'0' + mesege_flag;
	level[1] = 0;
	node->append_attribute( "level" ) = level;

	xml_SendNode( *node );

	mesege_send_timer.start();
}

static void xml_message_push( int flag, const char* characters, size_t length ){
	std::lock_guard lock( mesege_mutex );

	if( flag != mesege_flag ){
		xml_message_flush();
		mesege_flag = flag;
	}

	const char* end = characters + length;
	while ( characters != end )
	{
		const size_t space = MAX_MESEGE - 1 - mesege_len;
		if ( space == 0 ) {
			xml_message_flush();
		}
		else
		{
			const size_t size = std::min( space, static_cast<size_t>( end - characters ) );
			memcpy( mesege + mesege_len, characters, size );
			mesege_len += size;
			characters += size;
		}
	}
}

// all output ends up through here
static void FPrintf( int flag, char *buf ){
	static bool bGotXML = false;

	set_console_colour_for_flag( flag & ~( SYS_NOXMLflag | SYS_VRBflag ) );
	printf( "%s", buf );

	// the following part is XML stuff only.. but maybe we don't want that message to go down the XML pipe?
	if ( flag & SYS_NOXMLflag ) {
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
	xml_message_push( flag & ~( SYS_NOXMLflag | SYS_VRBflag ), buf, strlen( buf ) );
}

#ifdef DBG_XML
void DumpXML(){
	xmlSaveFile( "XMLDump.xml", doc );
}
#endif

void Sys_FPrintf( int flag, const char *format, ... ){
	char out_buffer[4096];
	va_list argptr;

	if ( ( flag & SYS_VRBflag ) && !verbose ) {
		return;
	}

	va_start( argptr, format );
	vsnprintf( out_buffer, sizeof(out_buffer), format, argptr );
	va_end( argptr );

	FPrintf( flag, out_buffer );
}

void Sys_Printf( const char *format, ... ){
	char out_buffer[4096];
	va_list argptr;

	va_start( argptr, format );
	vsnprintf( out_buffer, sizeof(out_buffer), format, argptr );
	va_end( argptr );

	FPrintf( SYS_STD, out_buffer );
}

void Sys_Warning( const char *format, ... ){
	char out_buffer[4096];
	va_list argptr;

	va_start( argptr, format );
	snprintf( out_buffer, sizeof(out_buffer), "WARNING: " );
	vsnprintf( out_buffer + strlen( "WARNING: " ), sizeof(out_buffer) - strlen( "WARNING: " ), format, argptr );
	va_end( argptr );

	FPrintf( SYS_WRN, out_buffer );
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

	va_start( argptr, error );
	vsnprintf( tmp, sizeof(tmp), error, argptr );
	va_end( argptr );

	snprintf( out_buffer, sizeof(out_buffer), "************ ERROR ************\n%s\n", tmp );

	FPrintf( SYS_ERR, out_buffer );
	xml_message_flush();

#ifdef DBG_XML
	DumpXML();
#endif

	//++timo HACK ALERT .. if we shut down too fast the xml stream won't reach the listener.
	// a clean solution is to send a sync request node in the stream and wait for an answer before exiting
	std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

	exit( 1 );
}
