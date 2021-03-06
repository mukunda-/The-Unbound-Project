//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright � 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include "mem/memorylib.h"

#pragma warning(disable:4996)

namespace Net {
	
//-------------------------------------------------------------------------------------------------
struct Packet {
	unsigned short size;
	char *data;
	Packet *next;

	static inline Packet *Create( int size ) {
		Packet *p = (Packet*)Memory::AllocMem( sizeof Packet + size ); 

		p->size = size;
		p->data = ((char*)p) + sizeof Packet;
		p->next = 0;
		return p;
	}

	static Packet *Build( const char *fmt, ... ) {
		char buffer[4096];
		va_list args;
		va_start( args, fmt );
		int size = vsnprintf( buffer, sizeof buffer, fmt, args );
		va_end( args );
		Packet *p = Create( size +1 );
	
		memcpy( p->data, buffer, size+1 );
		return p;
	}

	static void Delete( Packet *p ) {
		Memory::FreeMem( p );
	}
};

}
