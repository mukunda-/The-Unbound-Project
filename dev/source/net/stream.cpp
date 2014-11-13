//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "nwcore.h"
#include "stream.h"
#include "resolver.h"
#include "util/minmax.h"
#include "error.h"

namespace Net {

	namespace {
		const int READ_BUFFER_SIZE = 4096;

		/// -------------------------------------------------------------------
		/// Read a Varint from a stream. Max 28 bits.
		///
		/// @param stream Stream to read from.
		/// @param avail  How many bytes are available in the stream.
		/// @param value  Where to store the value.
		/// @returns Amount of bytes read from the stream. 
		///          0 means there wasn't sufficient data
		///          and the operation was cancelled.
		/// @throws ParseError when the data is errornous (too long).
		///
		int ReadVarint( std::istream &stream, int avail, int &value ) {
			if( avail == 0 ) return 0;
			int result = 0;

			// 28 bits max.
			for( int i = 0; i < 4; i++ ) {
				int data = stream.get();
				if( data == EOF ) throw ParseError();
				
				
				result |= (data & 127) << (i*7);
				if( data & 128 ) {
					if( avail < (i+2) ) {
						// not enough data. rewind and fail.
						stream.seekg( -(i+1), std::ios_base::cur );
						return 0;
					}
				} else {
					value = result;
					return i+1;
				}
			}
			throw ParseError();
		}
	}
	 
/// ---------------------------------------------------------------------------
/// [PRIVATE] Processed data that was received
///
/// @param data Data to process
/// @param size Size of data in bytes
/// @returns    Number of bytes that were used from the data.
///
//int Stream::ProcessDataRecv( const boost::uint8_t *data, int size ) {
	/*
	if( m_recv_write < 2 ) {
		if( m_recv_write==0 ) {
			m_recv_size = data[0];
			m_recv_write++;
			if( size >= 2 ) {
				m_recv_size |= data[1]<<8;
				m_recv_write++;
				if( m_recv_packet != 0 ) Packet::Delete( m_recv_packet );
				m_recv_packet = Packet::Create( m_recv_size );
				return 2;
			}
			return 1;
		} else {
			m_recv_size |= data[0] <<8;
			if( m_recv_packet != 0 ) Packet::Delete( m_recv_packet );
			m_recv_packet = Packet::Create( m_recv_size );
			return 1;
		}
	} else {
		int pwrite = m_recv_write-2;
		int copy_amount = Util::Min( size, m_recv_size-pwrite );
		memcpy( m_recv_packet->data + pwrite, data, copy_amount );
		m_recv_write += copy_amount;
		if( m_recv_write == m_recv_size+2 ) {

			bool handled = false;
			// pass to event
			// only push into fifo if not handled.

			handled = Events::Stream::Dispatcher( shared_from_this() )
					  .Receive( *m_recv_packet );
			
			if( !handled ) {
				m_recv_fifo.Push( m_recv_packet );
				m_recv_packet = 0;
			} else {
				Packet::Delete( m_recv_packet );
				m_recv_packet = 0;
			}
			
			// completed a packet
			m_recv_write = 0;
			m_recv_size = 0;
		}
		return copy_amount;
	}*/
//}
 

bool Stream::ParseMessage( std::istream &is ) {
	if( m_read_avail == 0 ) return false;

	if( m_read_length == 0 ) {
		int avail = (int)m_read_buffer.in_avail();
		if( avail == 0 ) return false;

		int bytesread = ReadVarint( is, m_read_avail, m_read_length );
		if( bytesread ) {
			m_read_avail -= bytesread;
			return true;
		}
		return false;
		
	} else {
		if( m_read_avail < m_read_length ) return false; // need more data.
		int header;
		int bytesread = ReadVarint( is, m_read_avail, header );
		if( bytesread == 0 ) throw ParseError();
		m_read_length -= bytesread;

		Remsg msg( header, is, m_read_length );
		Events::Stream::Dispatcher( shared_from_this() )
			.Receive( msg );
		
		m_read_length = 0;
		return true;
	}
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Callback for receiving data.
///  
void Stream::OnReceive( const boost::system::error_code& error, 
								    size_t bytes_transferred ) {
	
	if( error ) {
		// receive error: the remote has disconnected or an error occurred.
		m_connected = false; 
		
		// send event
		Events::Stream::Dispatcher( shared_from_this() )
			.Disconnected( error );
		
		// shutdown stream.
		StopReceive();
		DoClose();
		return;
	}
	
	m_read_buffer.commit( bytes_transferred );
	m_read_avail += bytes_transferred;

	std::istream is( &m_read_buffer );
	while( ParseMessage( is ) ) {
		// loop and parse messages.
	}

	// TODO catch parse error exception and terminate connection.

	if( !m_shutdown ) {
		// receive next data chunk
		StartReceive();
	} else {
		// shutdown.
		StopReceive();

		// send event
		Events::Stream::Dispatcher( shared_from_this() )
			.Disconnected( boost::system::error_code() );
		return;
	}
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Unset the receiving flag and signal the condition.
///  
void Stream::StopReceive() {
	// the signal isn't used anymore.

	//std::lock_guard<std::mutex> lock(m_recv_lock); 
	m_receiving = false;
	//m_cv_recv_complete.notify_all();
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Set the receiving flag and start listening for data.
///
/// @param first TRUE if this is the first receive call, i.e. right after
///        the connection is established. FALSE if it is a continued 
///        receive call.
///  
void Stream::StartReceive() {

	m_receiving = true;
	
	boost::asio::streambuf::mutable_buffers_type buffers = 
		m_read_buffer.prepare( READ_BUFFER_SIZE );
	
	m_socket.async_receive( 
		buffers, 
		m_strand.wrap( boost::bind( 
			&Stream::OnReceive, shared_from_this(), 
			boost::asio::placeholders::error, 
			boost::asio::placeholders::bytes_transferred )));
			
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Callback for when some data is put out onto the line.
///
void Stream::OnDataSent( const boost::system::error_code& error, 
						 size_t bytes_transferred ) {
	
	if( error ) {
		// error during send, this is an "unexpected" error.

		{
			// disconnect with error status
			Events::Stream::Dispatcher ev( shared_from_this() );
			ev.SendFailed( error );
			//ev.Disconnected( error );  this is called by the receive handler.
		}

		// shutdown.
		{
			std::lock_guard<std::mutex> lock( m_lock );
			m_writing = false;
			m_connected = false;
			m_cv_send_complete.notify_all();
		}
		
		DoClose();
		return;
	}

	m_send_buffers[1-m_send_buffer_index].consume( bytes_transferred );
	ContinueSend(); 
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Put more data on the line or signal that the send has
///           completed.
///
void Stream::ContinueSend() {
	// we own one buffer, but not the other.
	if( m_send_buffers[1-m_send_buffer_index].size() == 0 ) {
		
		// lock for accessing the other buffer and send variables
		std::lock_guard<std::mutex> lock( m_lock );
		if( m_send_buffers[m_send_buffer_index].size() == 0 ) {
			// no more data to send, stop the writing service.
			m_writing = false;
			m_cv_send_complete.notify_all();
			return;
		}

		// swap buffers.
		m_send_buffer_index = 1-m_send_buffer_index;
	}
	
	m_socket.async_write_some( 
		m_send_buffers[1-m_send_buffer_index].data(), m_strand.wrap(
			boost::bind( &Stream::OnDataSent, this, 
						 boost::asio::placeholders::error, 
						 boost::asio::placeholders::bytes_transferred )));
							
}

void Stream::Init() {
//	m_recv_packet = 0;
//	m_send_packet = 0;
//	m_recv_write = 0;
//	m_receiving = false;
//	m_sending = false;
	
	 
//	m_userdata = nullptr; 
//	m_shutdown = false; 
	
	// this doesn't work:
	// allow lingering for 30 seconds to finish unsent sending data on shutdown
//	boost::system::error_code ec; //ignore error, maybe log message
//	boost::asio::socket_base::linger option(true, 30);
//	socket.set_option(option,ec);
}

//-----------------------------------------------------------------------------
Stream::Stream( System::Service &service ) : 
			m_service(service), m_socket( m_service() ), 
			m_strand( m_service() ) {
	 
	Init();
}

//-----------------------------------------------------------------------------
Stream::Stream() : 
			m_service(Net::DefaultService()), m_socket( m_service() ),
			m_strand( m_service() ) {
	
	Init();
}

//-----------------------------------------------------------------------------
void Stream::Close() {


	m_service.Post( m_strand.wrap( 
		boost::bind( &Stream::DoClose, shared_from_this() )));
}

//-----------------------------------------------------------------------------
Stream::~Stream() {

	// these operations keep the stream alive, if the destructor is called
	// when they are active something is wrong.
	assert( m_writing == false ); 
	assert( m_receiving == false );
	 
	// the socket should not be open when the destructor is called.
	//m_socket.close();	
	 
	// free memory TODO smart packet pointers
//	if( m_recv_packet ) Packet::Delete( m_recv_packet );
//	if( m_send_packet ) Packet::Delete( m_send_packet ); 
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Cleanly close the socket.
///
void Stream::DoClose() {
	using namespace boost::asio::ip; 

	std::unique_lock<std::mutex> lock( m_lock ); 
	m_shutdown = true;

	boost::system::error_code ec;
	m_socket.shutdown( tcp::socket::shutdown_receive, ec );  

	if( m_connected ) {
		m_connected = false; 

		// wait until any sending is complete.
		while( m_writing ) {
			m_cv_send_complete.wait( lock );
		}

		// TODO: do we need to linger here for the data to be sent?
	} else {

	}

	m_socket.close();
}

//-----------------------------------------------------------------------------
void Stream::SetConnected() {
	{
		std::lock_guard<std::mutex> lock(m_lock);
		if( m_shutdown ) return;

		m_connected = true; 
		StartReceive();  
		m_writing = true;
	}
	ContinueSend();
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Callback for when a connection is accepted.
///
void Stream::OnAccept( const boost::system::error_code &error ) {

	if( !error ) {

		Events::Stream::Dispatcher( shared_from_this() )
			.Accepted();
		
		SetConnected();
	} else { 

		Events::Stream::Dispatcher( shared_from_this() )
			.AcceptError( error );
		
	}
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Callback for when an address has been resolved during
///           an async connect op. (or an error)
///
void Stream::OnResolve( const boost::system::error_code &error_code, 
					boost::asio::ip::tcp::resolver::iterator endpoints ) {
	
	if( error_code ) {
		Events::Stream::Dispatcher( shared_from_this() )
			.ConnectError( error_code );

	} else {
		// resolve OK, do connect.
		boost::asio::async_connect( m_socket, endpoints, 
			m_strand.wrap( 
				boost::bind( &Stream::OnConnect, 
						shared_from_this(), 
						boost::asio::placeholders::error )));
	}
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Callback for boost::asio::async_connect
///
void Stream::OnConnect( const boost::system::error_code &error ) {
	 
	if( error ) {
		Events::Stream::Dispatcher( shared_from_this() )
			.ConnectError( error );
		return;
	}
	
	Events::Stream::Dispatcher( shared_from_this() ).Connected(); 

	SetConnected();
}

//-----------------------------------------------------------------------------
//Packet *Stream::Read() {
//	return m_recv_fifo.Pop();
//}

//-----------------------------------------------------------------------------
//void Stream::WaitForData() {
//	m_recv_fifo.WaitForData();
//}

//-----------------------------------------------------------------------------
void Stream::WaitSend() {
	
	std::unique_lock<std::mutex> lock( m_lock );
	while( m_writing ) {
		m_cv_send_complete.wait( lock );
	}
}
		
//-----------------------------------------------------------------------------
void Stream::Write( Message &msg ) { 
	std::lock_guard<std::mutex> lock( m_lock );

	std::ostream stream( &m_send_buffers[m_send_buffer_index] );
	msg.Write( stream );
	// todo: catch write error and terminate stream.
	 
	if( !m_connected ) return;
	if( m_writing ) return;
	m_writing = true; 
	m_strand.post( boost::bind(	&Stream::ContinueSend, shared_from_this() ));
}


//-----------------------------------------------------------------------------
const std::string &Stream::GetHostname() const {
	return m_hostname;
}

//-----------------------------------------------------------------------------
void Stream::Connect( const std::string &host, const std::string &service ) {
	assert( !m_shutdown );
	assert( !m_connected );

	Resolver resolver;
	m_hostname = host + ":" + service; 
	boost::asio::connect( m_socket, resolver.Resolve( host, service ) );

	Events::Stream::Dispatcher( shared_from_this() )
		.Connected();

	m_service.Post( m_strand.wrap( 
		boost::bind( &Stream::SetConnected, this )));
}

//-----------------------------------------------------------------------------
void Stream::ConnectAsync( const std::string &host, 
						   const std::string &service ) {
	assert( !m_shutdown );
	assert( !m_connected );

	m_hostname = host + ":" + service;
	Resolver::CreateThreaded( host, service, 
		m_strand.wrap( 
			boost::bind( 
				&Stream::OnResolve, shared_from_this(), _1, _2 )));
}

//-----------------------------------------------------------------------------
void Stream::Listen( BasicListener &listener ) {
	assert( !m_shutdown );
	assert( !m_connected );

	listener.AsyncAccept(
		m_socket, 
		m_strand.wrap( boost::bind(
			&Stream::OnAccept, shared_from_this(),
			boost::asio::placeholders::error )));
}

}
