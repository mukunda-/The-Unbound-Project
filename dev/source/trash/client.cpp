//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

// winsock client

#include "client.h"

void client::increase_packet_count() {
	datapacket *packet_chunk = new datapacket[32];
	for( int i = 0; i < 32; i++ ) {
		packets.push_back(&packet_chunk[i]);
	}
	
	
}

datapacket *client::alloc_packet() {
	if( packets.empty() ) {
		increase_packet_count();
	}
	datapacket *p = packets.back();
	packets.pop_back();
	
	return p;
}

void client::release_packet( datapacket *p ) {
	packets.push_back(p);
	
}

int client::create_via_connect( std::string hostname, std::string port ) {
	
	if( my_socket )
		closesocket( my_socket );

	struct addrinfo hints, *res;

	// first, load up address structs with getaddrinfo():

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int error = getaddrinfo( hostname.c_str(), port.c_str(), &hints, &res);

	// make a socket:

	if( error ) {
		wprintf( L"network error: %s\n", gai_strerror( error ) );
		return error;
	}

	my_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	// connect!

	error = connect(my_socket, res->ai_addr, res->ai_addrlen);
	return error;
}

void client::create_via_accept( int listen_fd ) {
	
	// create via connection accept
	
	int addr_size = sizeof address;
    my_socket = accept(listen_fd, (struct sockaddr *)&address, &addr_size);

	
	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];
	int port;

	len = sizeof addr;
	getpeername(my_socket, (struct sockaddr*)&addr, &len);

	// deal with both IPv4 and IPv6:
	if (addr.ss_family == AF_INET) {
		struct sockaddr_in *s = (struct sockaddr_in *)&addr;
		port = ntohs(s->sin_port);
		inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
	} else { // AF_INET6
		struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
		port = ntohs(s->sin6_port);
		inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
	}

	name = ipstr;

	
}


const std::string *client::get_name() const {
	return &name;
}

int client::get_socket() {
	return my_socket;
}


datapacket *client::read() {
	if( fifo ) {
		
		datapacket *p = fifo;
		fifo = fifo->link;
		if( !fifo )
			fifo_end =0;
		return p;
	}
	return 0;
}

void client::on_data() {
	unsigned char data[16384];

	int size = recv( my_socket, (char*)data, 16384, 0 );

	if( size == 0 ) {
		// REMOTE SIDE DISCONNECTED
	}

	unsigned char *read = data;

	while( size > 0 ) {

		if( buffer_write_pos == 0 ) {
			packet_buffer = alloc_packet();
			packet_buffer->data_size = *read++;
			size--;
			buffer_write_pos++;
		} else if( buffer_write_pos == 1 ) {
			packet_buffer->data_size |= (*read++) << 8;
			size--;
			buffer_write_pos++;

			// TODO: VERIFY DATA SIZE
			
			//packet_buffer->data = new unsigned char[packet_buffer->data_size];
			remaining_bytes = packet_buffer->data_size;
		} else {

			while( (size > 0) && (remaining_bytes > 0) ) {
				packet_buffer->data[buffer_write_pos-2] = *read++;
				size--;
				remaining_bytes--;
				buffer_write_pos++;
			}

			if( remaining_bytes == 0 ) {
				packet_buffer->link = 0;
				if( !fifo ) {
					fifo = packet_buffer;
					fifo_end = packet_buffer;
				} else {
					fifo_end->link = packet_buffer;
					fifo_end = packet_buffer;
				}

				buffer_write_pos = 0;
			}

		}
	}
}

void client::queue_packet( datapacket *p ) { 
	
	p->link = 0;
	if( fifo_out ) { 
		fifo_out_end->link = p;
		fifo_out_end = p;
	} else {
		fifo_out_end=fifo_out=p;
	}
}

void client::refill_fifo_out() {
	for( datapacket *p = fifo_out; p; p = fifo_out) {
		if( fifo_out_buffer_size+p->data_size+2 < CLIENT_MAX_FIFO_BUFFER ) {
			int write = fifo_out_buffer_read+fifo_out_buffer_size;
			fifo_out_buffer_size += p->data_size+2;
			fifo_out_buffer[write&CLIENT_MAX_FIFO_BUFFER_MASK] = p->data_size & 0xFF;
			fifo_out_buffer[(write+1)&CLIENT_MAX_FIFO_BUFFER_MASK] = p->data_size >> 8;
			write += 2;
			for( int i = 0; i < p->data_size; i++ ) {
				fifo_out_buffer[write&CLIENT_MAX_FIFO_BUFFER_MASK] = p->data[i];
				write++;
			}

			
			//if( p->data ) {
			//	delete[] p->data;
			//}

			datapacket *del = fifo_out;
			fifo_out = fifo_out->link;

			if( fifo_out == 0 ) {
				fifo_out_end=0;
			}

			release_packet(del);
			//delete del;
		}
	}
}

void client::dispatch_packets() {

	if( !fifo_out && fifo_out_buffer_size == 0 ) { return; } // no buffered data

	refill_fifo_out();

	// COPY DATA TO END
	int overflow = fifo_out_buffer_read+fifo_out_buffer_size - CLIENT_MAX_FIFO_BUFFER;
	if( overflow > 0 ) {
		
		for( int i = 0; i < overflow; i++ ) {
			fifo_out_buffer[CLIENT_MAX_FIFO_BUFFER+i] = fifo_out_buffer[i];
		}
	}
	
	int sent = send( my_socket, (char*)fifo_out_buffer+fifo_out_buffer_read, fifo_out_buffer_size, 0 );

	fifo_out_buffer_size -= sent;
	fifo_out_buffer_read += sent;
	fifo_out_buffer_read &= CLIENT_MAX_FIFO_BUFFER_MASK;
}

void client::close() {
	if( my_socket ) {
		closesocket( my_socket );
		my_socket=0;
	}
}

bool client::connected() {
	return my_socket != 0;
}


client::client() {
	prev=next=0;
	my_socket=0;
	fifo=0;
	fifo_end=0;

	packet_buffer=0;
	buffer_write_pos=0;
	remaining_bytes=0;

	fifo_out = 0;
	fifo_out_end = 0;

	fifo_out_buffer_read=0;
	fifo_out_buffer_size=0;
}
