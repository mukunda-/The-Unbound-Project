//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "nwcore.h"
#include "stream.h"
#include "util/minmax.h"

namespace Net {
	 
/// ---------------------------------------------------------------------------
/// [PRIVATE] Processed data that was received
///
/// @param data Data to process
/// @param size Size of data in bytes
/// @returns    Number of bytes that were used from the data.
///
int Stream::ProcessDataRecv( const boost::uint8_t *data, int size ) {
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
		Close();
		return;
	}

	// parse packets from data stream
	size_t read = 0;
	while( read < bytes_transferred ) {
		int processed = ProcessDataRecv( m_recv_buffer + read, bytes_transferred-read );
		read += processed;
	}
	 
	if( !m_shutdown ) {
		// receive next data chunk
		StartReceive( false );
	} else {
		// shutdown.
		StopReceive();
	}
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Unset the receiving flag and signal the condition.
///  
void Stream::StopReceive() {
	
	std::lock_guard<std::mutex> lock(m_recv_lock); 
	m_receiving = false;
	m_cv_recv_complete.notify_all();
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Set the receiving flag and start listening for data.
///
/// @param first TRUE if this is the first receive call, i.e. right after
///        the connection is established. FALSE if it is a continued 
///        receive call.
///  
void Stream::StartReceive( bool first ) {
	std::lock_guard<std::mutex> lock(m_recv_lock);

	if( first ) assert( m_receiving==false );
	m_receiving = true;
	m_socket.async_receive( 
		boost::asio::buffer( m_recv_buffer ), 
		boost::bind( 
			&Stream::OnReceive, shared_from_this(), 
			boost::asio::placeholders::error, 
			boost::asio::placeholders::bytes_transferred ) );
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Callback for when some data is put out onto the line.
///
void Stream::OnDataSent( const boost::system::error_code& error, 
						 size_t bytes_transferred ) {
	
	if( error ) {
		// error during send, this is an "unexpected" error.

		m_connected = false;
		{
			// disconnect with error status
			Events::Stream::Dispatcher ev( shared_from_this() );
			ev.SendFailed( error );
			ev.Disconnected( error ); 
		}

		// shutdown.
		std::lock_guard<std::mutex> lock(m_send_lock);
		m_sending = false;
		m_cv_send_complete.notify_all();
		Close();
		return;
	}

	if( (int)bytes_transferred < m_send_write ) {
		memcpy( m_send_buffer, m_send_buffer+bytes_transferred, m_send_write-bytes_transferred );
		m_send_write -= bytes_transferred;
	} else {
		m_send_write = 0;
	}
	ContinueSend();
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Put more data on the line or signal that the send has
///           completed.
///
void Stream::ContinueSend() {

	do {
		// put packets into the send buffer
		while( m_send_write < BUFFER_SIZE-16 ) {
			if( !m_send_packet ) {
				m_send_packet = m_send_fifo.Pop();
				if( m_send_packet == 0 ) {
					// no packets to send
					break;
				}
				m_send_read = 0;
			} 

			if( m_send_read < 2 ) {
				// first two bytes are the size.
				m_send_buffer[m_send_write++] = m_send_packet->size & 255;
				m_send_buffer[m_send_write++] = m_send_packet->size >> 8;
				m_send_read += 2;
			} else {
				// copy the payload
				int amount = Util::Min( 
						BUFFER_SIZE - m_send_write, 
						m_send_packet->size - (m_send_read-2));

				memcpy( m_send_buffer + m_send_write, 
						m_send_packet->data + m_send_read - 2, 
						amount  ); 

				m_send_read += amount;
				m_send_write += amount;

				if( m_send_read >= m_send_packet->size+2) {
					// delete finished packets
					Packet::Delete( m_send_packet );
					m_send_packet = 0;
					m_send_read = 0;
				}
			} 
		}

		if( m_send_write != 0 ) {

			// send some data
			m_socket.async_send( 
				boost::asio::buffer( m_send_buffer, m_send_write ), 
				boost::bind( &Stream::OnDataSent, this, 
							 boost::asio::placeholders::error, 
							 boost::asio::placeholders::bytes_transferred ));

			return;
		} else {
			std::lock_guard<std::mutex> lock(m_send_lock);
			if( m_send_fifo.Count() > 0 ) {
				continue;
			} else {
				m_sending = false;
				break;
			}
		}
	} while(true);
	m_cv_send_complete.notify_all();
}

void Stream::Init() {
	m_recv_packet = 0;
	m_send_packet = 0;
	m_recv_write = 0;
	m_receiving = false;
	m_sending = false;
	m_connected = false;
	 
	m_userdata = nullptr;

	m_shutdown = false; 
	
	// this doesn't work:
	// allow lingering for 30 seconds to finish unsent sending data on shutdown
//	boost::system::error_code ec; //ignore error, maybe log message
//	boost::asio::socket_base::linger option(true, 30);
//	socket.set_option(option,ec);
}

//-------------------------------------------------------------------------------------------------
Stream::Stream( System::Service &service ) : 
			m_service(service), m_socket( m_service() ) {
	 
	Init();
}

//-------------------------------------------------------------------------------------------------
Stream::Stream() : m_service(Net::DefaultService()), m_socket( m_service() ) {
	
	Init();
}

//-----------------------------------------------------------------------------
void Stream::Close() {
	using namespace boost::asio::ip;

	// TODO: how do async events handle this shit?
//	Event::Lock lock( m_event_handler );
	
	int shutdown = m_shutdown.exchange( true );
	if( shutdown ) return; // we have already shut down. 

	boost::system::error_code ec;

	if( m_connected ) {
		m_socket.shutdown( tcp::socket::shutdown_receive, ec );  
	}

	m_service.Post(
		boost::bind( &Stream::CloseAfterSend, 
					 shared_from_this() ) ); 
}

//-----------------------------------------------------------------------------
Stream::~Stream() {

	// destructor should not be called while operations are in progress.
	assert( m_sending == false ); 
	assert( m_receiving == false );
	
	boost::system::error_code ec; 
	m_socket.shutdown( m_socket.shutdown_both, ec );
	m_socket.close(); //   this cancels any async operations
	 
	// free memory TODO smart packet pointers
	if( m_recv_packet ) Packet::Delete( m_recv_packet );
	if( m_send_packet ) Packet::Delete( m_send_packet ); 
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Wait until the send buffer is empty and then close the socket.
///
void Stream::CloseAfterSend() {
	
	std::unique_lock<std::mutex> lock(m_send_lock);
	while( m_sending ) {
		m_cv_send_complete.wait( lock );
	}

	m_socket.close();
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Callback for when a connection is accepted.
///
void Stream::OnAccept( const boost::system::error_code &error ) {
	if( !error ) {
		m_connected = true;

		Events::Stream::Dispatcher( shared_from_this() )
			.Accepted();

		StartReceive( true ); 
		
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
			boost::bind( &Stream::OnConnect, 
						shared_from_this(), 
						boost::asio::placeholders::error ) );
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
	
	m_connected = true;

	Events::Stream::Dispatcher( shared_from_this() ).Connected();

	StartReceive( true );
}

//-----------------------------------------------------------------------------
Packet *Stream::Read() {
	return m_recv_fifo.Pop();
} 

//-----------------------------------------------------------------------------
void Stream::WaitForData() {
	m_recv_fifo.WaitForData();
}

//-----------------------------------------------------------------------------
void Stream::WaitSend() {
	
	std::unique_lock<std::mutex> lock(m_send_lock);
	while( m_sending ) {
		m_cv_send_complete.wait( lock );
	}
}

//-------------------------------------------------------------------------------------------------
void Stream::Write( Packet *p ) {
	m_send_fifo.Push(p);
	{
		std::lock_guard<std::mutex> lock(m_send_lock);
		if( m_sending ) return;
		m_send_read = 0;
		m_send_write = 0;
		m_send_packet = 0;
		m_sending = true;
		ContinueSend();
	}
}

//-------------------------------------------------------------------------------------------------
const std::string &Stream::GetHostname() const {
	return m_hostname;
}

}
