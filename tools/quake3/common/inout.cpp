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
#include <cstdarg>

#ifdef WIN32
#define NOMINMAX 1
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

bool terminalColor = true;
bool verbose = false;

// our main document
// is streamed through the network to Radiant
// possibly written to disk at the end of the run
//++timo FIXME: need to be global, required when creating nodes?
static pugi::xml_document doc;

// some useful stuff
pugi::xml_node xml_NodeForVec( const Vector3& v ){
	pugi::xml_node ret{};
	ret.set_name("point");
	ret.set_value(std::format("{} {} {}", v[0], v[1], v[2]));
	return ret;
}

static void xml_message_flush();

// send a node down the stream, add it to the document
void xml_SendNode( pugi::xml_node& node ){
	std::lock_guard lock( mesege_mutex );

	xml_message_flush(); /* flush regular print messages buffer, so that special ones will appear at correct spot */

	doc.append_copy(node);

	struct xml_string_writer : public pugi::xml_writer {
		std::string m_string;
		virtual void write(const void* data, size_t size)
		{
			m_string.append(static_cast<const char*>(data), size);
		}
	};

	if ( brdcst_socket ) {
		xml_string_writer writer{};
		node.print(writer);

		// the XML node might be too big to fit in a single network message
		// l_net library defines an upper limit of MAX_NETMESSAGE
		// there are some size check errors, so we use MAX_NETMESSAGE-10 to be safe
		// if the size of the buffer exceeds MAX_NETMESSAGE-10 we'll send in several network messages
		int length = (int)writer.m_string.length();
		for ( int pos = 0; pos < length; )
		{
			// what size are we gonna send now?
			const int size = std::min( length - pos, MAX_NETMESSAGE - 10 );
			netmessage_t msg;
			NMSG_Clear( &msg );
			NMSG_WriteString_n( &msg, reinterpret_cast<const char*>( writer.m_string.c_str() + pos ), size );
			Net_Send( brdcst_socket, &msg );
			// now that the thing is sent prepare to loop again
			pos += size;
		}
	}
}

void xml_Select( const char *msg, int entitynum, int brushnum, bool bError ){
	// now build a proper "select" XML node
	pugi::xml_node node{};
	node.set_name("select");
	node.set_value(std::format("Entity {}, Brush {}: {}", entitynum, brushnum, msg));
	node.append_attribute( "level" ) = std::format("{}", bError ? SYS_ERR : SYS_WRN);
	// a 'select' information
	pugi::xml_node select = node.append_child("brush");
	select.set_value(std::format("{} {}", entitynum, brushnum));
	xml_SendNode(node);

	if (bError) {
		Error(node.text().as_string());
	}
	else{
		Sys_FPrintf(SYS_NOXMLflag | SYS_WRN, "%s\n", node.text().as_string());
	}
}

void xml_Point( const char *msg, const Vector3& pt ){
	pugi::xml_node node{};
	node.set_name("pointmsg");
	node.set_value(msg);
	node.append_attribute("level") = std::format("{}", SYS_ERR);
	// a 'point' node
	pugi::xml_node point = node.append_child("point");
	point.set_value(std::format("{} {} {}", pt[0], pt[1], pt[2]));
	xml_SendNode(node);

	Error( std::format("{} ({} {} {})", msg, pt[0], pt[1], pt[2]).c_str() );
}

void xml_Winding( const char *msg, const Vector3 p[], int numpoints, bool die ){
	std::string buf;

	pugi::xml_node node{};
	node.set_name("windingmsg");
	node.set_value(msg);
	node.append_attribute("level") = std::format("{}", SYS_ERR);
	// a 'winding' node
	buf += std::format("{} ", numpoints);
	for ( int i = 0; i < numpoints; ++i )
	{
		buf += std::format("({} {} {})", p[i][0], p[i][1], p[i][2]);
	}

	pugi::xml_node winding = node.append_child("winding");
	winding.set_value(buf);

	xml_SendNode( node );

	if ( die ) {
		Error( msg );
	}
	else
	{
		Sys_Printf( "%s\n", msg );
	}
}

static void set_console_colour_for_flag( int flag ){
	if (!terminalColor) {
		return;
	}
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
#else
	static int curFlag = SYS_STD;
	if (curFlag == flag) {
		return;
	}
	curFlag = flag;
	switch (flag) {
		case SYS_STD: {
			fputs("\033[00m", stdout); // reset
			break;
		}

		case SYS_WRN: {
			fputs("\033[1m", stdout); // bold
			fputs("\033[33m", stdout); // yellow
			break;
		}

		case SYS_ERR: {
			fputs("\033[1m", stdout); // bold
			fputs("\033[31m", stdout); // red
			break;
		}
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

	mesege[mesege_len] = 0;
	mesege_len = 0;

	pugi::xml_node node{};
	node.set_name("message");
	node.set_value(mesege);
	node.append_attribute( "level" ) = std::format("{}", mesege_flag);

	xml_SendNode( node );

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
static void FPrintf( int flag, const char *buf ){
	static bool bGotXML = false;

	set_console_colour_for_flag( flag & ~( SYS_NOXMLflag | SYS_VRBflag ) );
	fputs( buf, stdout );

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
		doc.append_child("q3map_feedback");
		bGotXML = true;
	}
	xml_message_push( flag & ~( SYS_NOXMLflag | SYS_VRBflag ), buf, strlen( buf ) );
}

#ifdef DBG_XML
void DumpXML(){
	doc.save_file("XMLDump.xml");
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
	char tmp[4096];
	va_list argptr;

	va_start( argptr, error );
	vsnprintf( tmp, sizeof(tmp), error, argptr );
	va_end( argptr );

	auto out = std::format("************ ERROR ************\n{}\n", tmp);

	FPrintf( SYS_ERR, out.c_str() );
	xml_message_flush();

#ifdef DBG_XML
	DumpXML();
#endif

	//++timo HACK ALERT .. if we shut down too fast the xml stream won't reach the listener.
	// a clean solution is to send a sync request node in the stream and wait for an answer before exiting
	std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

	exit( 1 );
}
