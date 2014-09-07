//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>

#include "network.h"
#include "client.h"

#include "planet.h"

#include "worldrender.h"

#define MAX_STRING_LENGTH 500

#define PACKETFAIL { printf("poopy\n"); return; }

namespace network {

client my_client;

//--------------------------------------------------------------------------------------------------
//void packet_alloc( datapacket *p ) {
//--------------------------------------------------------------------------------------------------
//	p->data = new unsigned char[ p->data_size ];
//}

//--------------------------------------------------------------------------------------------------
datapacket * create_packet( unsigned char hh, unsigned char hl, int parambytes ) {
//--------------------------------------------------------------------------------------------------
	datapacket *p = my_client.alloc_packet();
	//datapacket *p = new datapacket;
	p->data_size = NETWORK_PACKET_HEADER_SIZE + parambytes;
	//packet_alloc(p);
	p->data[0] = hl;
	p->data[1] = hh;
	return p;
}

bool verify_packet( datapacket *p, const char *params ) {
	// returns true if arguments dont fill parameters properly (errornous)

	int bytes_left = p->data_size;
	unsigned char *data_read = p->data+NETWORK_PACKET_HEADER_SIZE;
	bytes_left -= NETWORK_PACKET_HEADER_SIZE;
	if( bytes_left < 0 ) return true; // error: packet doesnt have header

	int iterator = 0;
	for( int i = 0; params[i]; i++ ) {
		switch(i) {
		case 'b':
			bytes_left -= 1;
			if( bytes_left < 0 ) return true; // error: missing byte argument
			data_read++;
			break;
		case 'w':
			bytes_left -= 2;
			if( bytes_left < 0 ) return true; // error: missing word argument
			data_read += 2;
			break;
		case 'p':
			bytes_left -= 3;
			if( bytes_left < 0 ) return true; // error: missing pword argument
			data_read += 3;
			break;
		case 'd':
			bytes_left -= 4;
			if( bytes_left < 0 ) return true; // error: missing dword argument
			data_read += 4;
			break;
		case 's':
			// search for terminator
			int string_size=0;
			if( bytes_left == 0 ) return true; // error: missing string argument
			while( (bytes_left > 0) ) {
				if( (*data_read++) == 0 ) {
					bytes_left--;
					continue; // string terminator found
				} else {
					bytes_left--;
				}
				string_size++;
				if( string_size > MAX_STRING_LENGTH ) return true; // string too long
			}
			return true; // missing string terminator
			break;
		}
	}
	return false; // packet verification OK
}

bool verify_planet_index( int index ) {
	return ( index >= (planet::get_width()/16)*(planet::get_depth()/16)*(planet::get_length()/16) );
}

//--------------------------------------------------------------------------------------------------
unsigned int get_dword( const datapacket *p, int index ) {
//--------------------------------------------------------------------------------------------------
	return *(unsigned int*)(p->data+NETWORK_PACKET_HEADER_SIZE+index);
}

//--------------------------------------------------------------------------------------------------
unsigned int get_pword( const datapacket *p, int index ) {
//--------------------------------------------------------------------------------------------------
	return (*(unsigned int*)(p->data+NETWORK_PACKET_HEADER_SIZE+index)) & 0xFFFFFF;
}

//--------------------------------------------------------------------------------------------------
unsigned short get_word( const datapacket *p, int index ) {
//--------------------------------------------------------------------------------------------------
	return (*(unsigned short*)(p->data+NETWORK_PACKET_HEADER_SIZE+index));
}

//--------------------------------------------------------------------------------------------------
unsigned char get_byte( const datapacket *p, int index ) {
//--------------------------------------------------------------------------------------------------
	return p->data[NETWORK_PACKET_HEADER_SIZE+index];
}

//--------------------------------------------------------------------------------------------------
void set_dword( datapacket *p, int index, unsigned int value ) {
//--------------------------------------------------------------------------------------------------
	*(unsigned int*)(p->data+NETWORK_PACKET_HEADER_SIZE+index) = value;
}

//--------------------------------------------------------------------------------------------------
void set_pword( datapacket *p, int index, unsigned int value ) {
//--------------------------------------------------------------------------------------------------
	*(unsigned short*)(p->data+NETWORK_PACKET_HEADER_SIZE+index) = value & 0xFFFF;
	*(unsigned char*)(p->data+NETWORK_PACKET_HEADER_SIZE+index+2) = value >> 16;
}

//--------------------------------------------------------------------------------------------------
void set_word( datapacket *p, int index, unsigned short value ) {
//--------------------------------------------------------------------------------------------------
	*(unsigned short*)(p->data+NETWORK_PACKET_HEADER_SIZE+index) = value;
}

//--------------------------------------------------------------------------------------------------
void set_byte( datapacket *p, int index, unsigned char value ) {
//--------------------------------------------------------------------------------------------------
	p->data[NETWORK_PACKET_HEADER_SIZE+index] = value;
}

//=================================================================================================
// SERVER<-CLIENT
//=================================================================================================

void csLogin( std::string name, std::string password ) {
	datapacket *p = my_client.alloc_packet();
	//datapacket *p = new datapacket;
	
	p->data_size = NETWORK_PACKET_HEADER_SIZE + name.size()+1 + password.size() + 1;
	//packet_alloc(p);

	p->data[0] = PHL_CS_SETUP_LOGIN;
	p->data[1] = PHH_CS_SETUP;

	strcpy( (char*)p->data+NETWORK_PACKET_HEADER_SIZE, name.c_str() );
	p->data[NETWORK_PACKET_HEADER_SIZE+name.size()] = 0;
	strcpy( (char*)p->data+NETWORK_PACKET_HEADER_SIZE+name.size()+1, password.c_str() );
	p->data[NETWORK_PACKET_HEADER_SIZE+name.size()+1+password.size()] = 0;
	my_client.queue_packet( p );
}

void csWorldChunkRequest( int index ) {
	datapacket *p = create_packet( PHH_CS_WORLD, PHL_CS_WORLD_CHUNK_REQUEST, 4 );
	set_dword( p, 0, (unsigned int)index );
	my_client.queue_packet( p );
}

//=================================================================================================
// SERVER->CLIENT
//=================================================================================================

void scHello( datapacket *p ) {
	
}

void scDenied( datapacket *p ) {
}

void scChatGlobal( datapacket *p ) {
	if( verify_packet( p, "ss" ) ) PACKETFAIL

}

void scChatLocal( datapacket *p ) {
	if( verify_packet( p, "ss" ) ) PACKETFAIL

}

void scChatParty( datapacket *p ) {
	if( verify_packet( p, "ss" ) )PACKETFAIL
}

void scChatPrivate( datapacket *p ) {
	if( verify_packet( p, "ss" ) ) PACKETFAIL
}

void scWorldSeed( datapacket *p ) {
	if( verify_packet( p, "d" ) ) PACKETFAIL

	//planet::create_network(get_dword(p,0));
}

void scWorldDataShort( datapacket *p ) {
	if( verify_packet( p, "db" ) ) PACKETFAIL
	int index = get_dword( p, 0 );
	if( verify_planet_index(index) ) PACKETFAIL
	int mode = get_byte( p, 4 );
	if( mode > 1 ) return; // errornous packet
	
	//planet::set_chunk( index, (planet::chunk*)mode );
}

void scWorldDataTransfer( datapacket *p ) {
	if( verify_packet( p, "dbb" ) ) PACKETFAIL
	
	int index = get_dword( p, 0 );


	if( verify_planet_index(index) ) PACKETFAIL // errornous data
	
	unsigned char *cdata = p->data+7;
	int cdata_size = p->data_size-7;
	int read = 0;
	if( cdata_size <= 0 ) PACKETFAIL // errornous packet

	//planet::chunk *c = new planet::chunk;
	//planet::chunk *c = planet::get_chunk_direct_indexed( index );

	///if( !planet::valid_chunk_addr(c) ) {
	//	c = planet::new_chunk();//new planet::chunk;
	//	c->instance=0;
	//}

	//c->dirty = 1;

	//c->original = 0;
	//c->solid = get_byte( p, 4 );


	for( int i = 0; i < 16*16*16; i++ ) {
		//c->sunlight[i] = 0;
		//c->light_cache[i] = 0;
	}

	int write = 0;
	while( write < 16*16*16 ) {
		if( read == cdata_size ) {
			//if( c->instance ) worldrender::reset_instance(c->instance-1);
			//planet::free_chunk(c);//delete c;
			return; // errornous data
		}
		int a = cdata[read++];
		
		if( a > 0xc0 ) {
			int count = a - 0xc0;
			if( read == cdata_size ) {
				//if( c->instance ) worldrender::reset_instance(c->instance-1);
				//planet::free_chunk(c);//delete c;
				PACKETFAIL // errornous data
			}
			a = cdata[read++];
			for( ; count > 0; count-- ) {
				if( write == 16*16*16 ) {
					if( read == cdata_size ) {
						//if( c->instance ) worldrender::reset_instance(c->instance-1);
						//planet::free_chunk(c);//delete c;
						PACKETFAIL // errornous data
					}
				}
				//c->data[write++] = a;
			}
		} else {
			//c->data[write++] = a;
		}
	}

	
	//planet::set_chunk( index, c );
}

//void delete_packet( datapacket *p ) {
//	if( p->data ) delete[] p->data;
//	delete p;
//}

void check_input() {
	if( my_client.connected() ) {
		fd_set pood;
		FD_ZERO( &pood );
		FD_SET( my_client.get_socket(), &pood );
		timeval t =  {0,0};
		select( 0, &pood, NULL, NULL, &t );

		if( FD_ISSET( my_client.get_socket(), &pood ) ) {
			my_client.on_data();
		}
	}
}

void process_packet( datapacket *p ) {
	if( p->data_size < NETWORK_PACKET_HEADER_SIZE ) PACKETFAIL // invalid packet

	switch( p->data[1] ) {
	case PHH_SC_WORLD:
		switch( p->data[0] ) {
		case PHL_SC_WORLD_SEED:
			scWorldSeed(p);
			break;
		case PHL_SC_WORLD_DATA_SHORT:
			scWorldDataShort(p);
			break;
		case PHL_SC_WORLD_DATA_XFER:
			printf( "world data transfer: %i\n", (int)p );
			scWorldDataTransfer(p);
		}
		break;
	}
}

void process_input() {
	datapacket *p;
	while( p = my_client.read() ) {
		process_packet(p);
		//delete_packet(p);
		my_client.release_packet(p);
	}
}

void dispatch_packets() {
	if( my_client.connected() ) {

		fd_set pood;
		FD_ZERO( &pood );
		FD_SET( my_client.get_socket(), &pood );
		timeval t =  {0,0};
		select( 0, NULL, &pood, NULL, &t );

		if( FD_ISSET( my_client.get_socket(), &pood ) ) {
			
			my_client.dispatch_packets();
		}



	}
} 

void process() {
	check_input();
	process_input();
	dispatch_packets();
}

int open( std::string hostname ) {
	int error = my_client.create_via_connect( hostname, "32798" );

	if( error ) {
		// couldnt connect
		return 1;
	}

	csLogin( "mister", "poodles" );

	return 0;
}

void close() {
	my_client.close();
}

}

