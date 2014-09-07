//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>

namespace network {

enum {
	PHL_CS_SETUP_NULL	=0,
	PHL_CS_SETUP_LOGIN	=1,

	PHL_CS_CHAT_GLOBAL	=1,
	PHL_CS_CHAT_LOCAL	=2,
	PHL_CS_CHAT_PARTY	=3,
	PHL_CS_CHAT_PRIVATE	=4,
	
	PHL_CS_WORLD_SEED_PLEASE	=0,
	PHL_CS_WORLD_CHUNK_REQUEST	=1,

	PHL_CS_PLAYER_POSITION	=0
};

enum {
	PHH_CS_SETUP	=0,
	PHH_CS_CHAT		=1,
	PHH_CS_WORLD	=2,
	PHH_CS_PLAYER	=3
};

enum {

	PHL_SC_SETUP_HELLO	=1,
	PHL_SC_SETUP_DENIED	=2,

	PHL_SC_CHAT_GLOBAL	=1,
	PHL_SC_CHAT_LOCAL	=2,
	PHL_SC_CHAT_PARTY	=3,
	PHL_SC_CHAT_PRIVATE	=4,

	PHL_SC_WORLD_SEED		=0,
	PHL_SC_WORLD_DATA_SHORT	=1,
	PHL_SC_WORLD_DATA_XFER	=2
};

enum {
	PHH_SC_SETUP	=0,
	PHH_SC_CHAT		=1,
	PHH_SC_WORLD	=2,
	PHH_SC_PLAYER	=3
};

void process();
int open( std::string hostname );
void close();

void csWorldChunkRequest( int index );


}