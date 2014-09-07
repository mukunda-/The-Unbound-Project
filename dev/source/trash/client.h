//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <string>
#include <vector>

#ifndef CLIENT_H
#define CLIENT_H

#define CLIENT_MAX_FIFO_BUFFER 32768
#define CLIENT_MAX_FIFO_BUFFER_MASK 32767

#define NETWORK_PACKET_HEADER_SIZE 2

typedef struct t_datapacket {
	short data_size;
	unsigned char data[4200];
	struct t_datapacket *link;
} datapacket;

class client {

	std::vector<datapacket*> packets;

	int my_socket;
	struct sockaddr_storage address;

	std::string name;

	datapacket *fifo;
	datapacket *fifo_end;

	datapacket *packet_buffer;
	int buffer_write_pos;
	int remaining_bytes;

	datapacket *fifo_out;
	datapacket *fifo_out_end;

	unsigned char fifo_out_buffer[CLIENT_MAX_FIFO_BUFFER+CLIENT_MAX_FIFO_BUFFER];
	int fifo_out_buffer_read;
	int fifo_out_buffer_size;

	void refill_fifo_out();

	void increase_packet_count() ;
	
public:
	client *prev;	// linked list hooks
	client *next;

	client();
	void create_via_accept( int listen_fd );
	int create_via_connect( std::string hostname, std::string port );
	int get_socket();
	void on_data();
	void queue_packet( datapacket *p );
	void dispatch_packets();
	void close();
	bool connected();

	const std::string *get_name() const;

	datapacket *read();

	
	datapacket *alloc_packet();
	void release_packet( datapacket *p );

};


#endif
