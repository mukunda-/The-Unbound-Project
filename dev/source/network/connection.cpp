//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include <stdafx.h>
#include "network/connection.h"
#include "network/resolver.h"

namespace Network {

 //-------------------------------------------------------------------------------------------------
Connection::Stream::EventLatch::EventLatch( Stream &stream ) : 
		m_lock( stream.m_handler_mutex ),
		m_stream( stream ) {
}

//-------------------------------------------------------------------------------------------------
void Connection::Stream::EventLatch::AcceptedConnection() {
	if( !m_stream.m_parent ) return;
	if( !m_stream.m_parent->m_event_handler ) return;
	m_stream.m_parent->m_event_handler->AcceptedConnection( *m_stream.m_parent );
}

//-------------------------------------------------------------------------------------------------
void Connection::Stream::EventLatch::AcceptError( const boost::system::error_code &error ) {
	if( !m_stream.m_parent ) return;
	if( !m_stream.m_parent->m_event_handler ) return;
	m_stream.m_parent->m_event_handler->AcceptError( *m_stream.m_parent, error );
}

//-------------------------------------------------------------------------------------------------
void Connection::Stream::EventLatch::CantResolve( const boost::system::error_code &error ) {
	if( !m_stream.m_parent ) return;
	if( !m_stream.m_parent->m_event_handler ) return;
	m_stream.m_parent->m_event_handler->CantResolve( *m_stream.m_parent, error );
}

//-------------------------------------------------------------------------------------------------
void Connection::Stream::EventLatch::ConnectError( const boost::system::error_code &error ) {
	if( !m_stream.m_parent ) return;
	if( !m_stream.m_parent->m_event_handler ) return;
	m_stream.m_parent->m_event_handler->ConnectError( *m_stream.m_parent, error );
}

//-------------------------------------------------------------------------------------------------
void Connection::Stream::EventLatch::Connected() {
	if( !m_stream.m_parent ) return;
	if( !m_stream.m_parent->m_event_handler ) return;
	m_stream.m_parent->m_event_handler->Connected( *m_stream.m_parent );
}

//-------------------------------------------------------------------------------------------------
void Connection::Stream::EventLatch::Disconnected( const boost::system::error_code &error ) {
	if( !m_stream.m_parent ) return;
	if( !m_stream.m_parent->m_event_handler ) return;
	m_stream.m_parent->m_event_handler->Disconnected( *m_stream.m_parent, error );
}

//-------------------------------------------------------------------------------------------------
void Connection::Stream::EventLatch::DisconnectedError( const boost::system::error_code &error ) {
	if( !m_stream.m_parent ) return;
	if( !m_stream.m_parent->m_event_handler ) return;
	m_stream.m_parent->m_event_handler->DisconnectedError( *m_stream.m_parent, error );
}

//-------------------------------------------------------------------------------------------------
bool Connection::Stream::EventLatch::Receive( Network::Packet &packet ) {
	if( !m_stream.m_parent ) return false;
	if( !m_stream.m_parent->m_event_handler ) return false;
	return m_stream.m_parent->m_event_handler->Receive( *m_stream.m_parent, packet );
}
 
//-------------------------------------------------------------------------------------------------
int Connection::Stream::ProcessDataRecv( const boost::uint8_t *data, int size ) {
	if( m_recv_write < 2 ) {
		if( m_recv_write==0 ) {
			m_recv_size = data[0];
			m_recv_write++;
			if( size >= 2 ) {
				m_recv_size |= data[1]<<8;
				m_recv_write++;
				if( m_recv_packet != 0 ) Network::DeletePacket( m_recv_packet );
				m_recv_packet = Network::CreatePacket( m_recv_size );
				return 2;
			}
			return 1;
		} else {
			m_recv_size |= data[0] <<8;
			if( m_recv_packet != 0 ) Network::DeletePacket( m_recv_packet );
			m_recv_packet = Network::CreatePacket( m_recv_size );
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
			{
				EventLatch event( *this );
				handled = event.Receive( *m_recv_packet );
			}
			
			if( !handled ) {
				m_recv_fifo.Push( m_recv_packet );
				m_recv_packet = 0;
			} else {
				DeletePacket( m_recv_packet );
				m_recv_packet = 0;
			}
			
			// completed a packet
			m_recv_write = 0;
			m_recv_size = 0;
		}
		return copy_amount;
	}
}

//-------------------------------------------------------------------------------------------------
void Connection::Stream::OnReceive( const boost::system::error_code& error, 
								    size_t bytes_transferred ) {
	
	if( error ) {
		
		m_connected = false;

		{
			EventLatch event( *this );
			event.Disconnected( error );
		}

		boost::lock_guard<boost::mutex> lock(m_recv_lock);
		m_receiving = false;
		m_cv_recv_complete.notify_all();
		return;
	}

	size_t read = 0;
	while( read < bytes_transferred ) {
		int processed = ProcessDataRecv( m_recv_buffer + read, bytes_transferred-read );
		read += processed;
	}
	 
	if( !m_shutdown ) {
		StartReceive( false );
	} else {
		boost::lock_guard<boost::mutex> lock(m_recv_lock);
		m_receiving = false;
		m_cv_recv_complete.notify_all();
	}
}

//-------------------------------------------------------------------------------------------------
void Connection::Stream::StartReceive( bool first ) {
	boost::lock_guard<boost::mutex> lock(m_recv_lock);
	if( first ) assert( m_receiving==false );
	m_receiving = true;
	m_socket.async_receive( 
		boost::asio::buffer( m_recv_buffer ), 
		boost::bind( 
			&Stream::OnReceive, shared_from_this(), 
			boost::asio::placeholders::error, 
			boost::asio::placeholders::bytes_transferred ) );
}

//-------------------------------------------------------------------------------------------------
void Connection::Stream::OnDataSent( const boost::system::error_code& error, size_t bytes_transferred ) {
	
	if( error ) {
		m_connected = false;
		{
			EventLatch event( *this );
			event.DisconnectedError( error );
		}

		boost::lock_guard<boost::mutex> lock(m_send_lock);
		m_sending = false;
		m_cv_send_complete.notify_all();
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

//-------------------------------------------------------------------------------------------------
void Connection::Stream::ContinueSend() {

	do {
		while( m_send_write < BUFFER_SIZE-16 ) {
			if( m_send_packet == 0 ) {
				m_send_packet = m_send_fifo.Pop();
				if( m_send_packet == 0 ) {
					// no packets to send
					break;
				}
				m_send_read = 0;
			} 

			if( m_send_read < 2 ) {
				m_send_buffer[m_send_write++] = m_send_packet->size & 255;
				m_send_buffer[m_send_write++] = m_send_packet->size >> 8;
				m_send_read += 2;
			} else {
				int amount = Util::Min( BUFFER_SIZE - m_send_write, m_send_packet->size - (m_send_read-2) );
				memcpy( m_send_buffer + m_send_write, m_send_packet->data + m_send_read - 2, amount  );
				 

				m_send_read += amount;
				m_send_write += amount;
				if( m_send_read >= m_send_packet->size+2) {
					DeletePacket( m_send_packet );
					m_send_packet = 0;
					m_send_read = 0;
				}
			} 
		}

		if( m_send_write != 0 ) {
			m_socket.async_send( 
				boost::asio::buffer( m_send_buffer, m_send_write ), 
				boost::bind( &Stream::OnDataSent, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );

			return;
		} else {
			boost::lock_guard<boost::mutex> lock(m_send_lock);
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

//-------------------------------------------------------------------------------------------------
Connection::Stream::Stream( Connection * p_parent ) : m_socket( DefaultService()() ) {
//	resolver = 0;
	// todo: can stream start with no parent? or should it be passed by reference?
	m_recv_packet = 0;
	m_send_packet = 0;
	m_recv_write = 0;
	m_receiving = false;
	m_sending = false;
	m_shutdown = false; 
	m_connected = false;

	m_parent = p_parent;

	// this doesn't work:
	// allow lingering for 30 seconds to finish unsent sending data on shutdown
//	boost::system::error_code ec; //ignore error, maybe log message
//	boost::asio::socket_base::linger option(true, 30);
//	socket.set_option(option,ec);
}

//-------------------------------------------------------------------------------------------------
void Connection::Stream::Shutdown() {
	if( !m_shutdown ) {
		{
			// unlink from parent
			boost::lock_guard<boost::mutex> lock( m_handler_mutex );
			m_parent = nullptr;
		}
		InterlockedExchange( &m_shutdown, 1 );
		boost::system::error_code ec;
		if( m_connected ) {
			m_socket.shutdown( boost::asio::ip::tcp::socket::shutdown_receive, ec );  
		}

		DefaultService()().post( boost::bind( &Stream::CloseAfterSend, shared_from_this() ) );
	}
}

//-------------------------------------------------------------------------------------------------
void Connection::Stream::CloseAfterSend() {
	
	boost::unique_lock<boost::mutex> lock(m_send_lock);
	while( m_sending ) {
		m_cv_send_complete.wait( lock );
	}

	m_socket.close();
}

//-------------------------------------------------------------------------------------------------
Connection::Stream::~Stream() {

	// deconstructor should not be called while operations are in progress.
	assert( m_sending == false ); 
	assert( m_receiving == false );
	
	InterlockedExchange( &m_shutdown, 1 );
	boost::system::error_code ec;
	if( m_connected ) {
		m_socket.shutdown( m_socket.shutdown_both, ec );
	}
	m_socket.close(); //   this cancels any async operations
	 
	// free memory
	if( m_recv_packet ) DeletePacket( m_recv_packet );
	if( m_send_packet ) DeletePacket( m_send_packet ); 
}

//-------------------------------------------------------------------------------------------------
Connection::Connection() : m_stream( new Stream(this) ) {
	m_event_handler = nullptr;
	m_userdata = nullptr;
}

//-------------------------------------------------------------------------------------------------
Connection::~Connection() {
	m_stream->Shutdown();
} 
 
//-------------------------------------------------------------------------------------------------
void Connection::Listen( Network::Listener &listener ) {
	listener.AsyncAccept( m_stream->m_socket, boost::bind( 
			&Stream::OnAccept, m_stream->shared_from_this(), 
			boost::asio::placeholders::error ) );
}

//-------------------------------------------------------------------------------------------------
void Connection::Stream::OnAccept( const boost::system::error_code &error ) {
	if( !error ) {
		m_connected = true;
		{
			EventLatch event( *this );
			event.AcceptedConnection();
		}
		
		// TODO: deny connections in event?

		StartReceive(); 
		
	} else { 

		{
			EventLatch event( *this );
			event.AcceptError( error );
		}
	}
}

//-------------------------------------------------------------------------------------------------
bool Connection::Connect( const std::string &host, const std::string &service ) {
	Network::Resolver resolver; 
	m_hostname = host + ":" + service;
	boost::system::error_code ec;
	boost::asio::connect( m_stream->m_socket, resolver.Resolve( host, service ), ec );
	if( ec ) {
		return false;
	}
	m_stream->m_connected = true;

	if( m_event_handler ) {
		m_event_handler->Connected( *this );
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
void Connection::ConnectAsync( const std::string &host, const std::string &service ) {
	m_hostname = host + ":" + service;
	Network::Resolver::CreateThreaded( host, service, 
		boost::bind( 
			&Stream::OnResolve, m_stream->shared_from_this(), _1, _2 ) ); 
}

//-------------------------------------------------------------------------------------------------
void Connection::Stream::OnResolve( const boost::system::error_code &error_code, 
					boost::asio::ip::tcp::resolver::iterator endpoints ) {
	if( error_code ) {
		EventLatch event( *this );
		event.CantResolve( error_code );
	} else {
		boost::asio::async_connect( m_socket, endpoints, 
			boost::bind( &Stream::OnConnect, 
						shared_from_this(), 
						boost::asio::placeholders::error ) );
	}
}
  
//-------------------------------------------------------------------------------------------------
void Connection::Stream::OnConnect( const boost::system::error_code &error ) {
	if( error ) {
		EventLatch event( *this );
		event.ConnectError( error );
		return;
	}
	
	m_connected = true;

	{
		EventLatch event(*this);
		event.Connected();
	}

	StartReceive();
}

//-------------------------------------------------------------------------------------------------
boost::asio::ip::tcp::socket &Connection::Socket() {
	return m_stream->m_socket;
}

//-------------------------------------------------------------------------------------------------
Packet *Connection::Read() {
	return m_stream->m_recv_fifo.Pop();
} 

//-------------------------------------------------------------------------------------------------
void Connection::WaitForData() {
	m_stream->m_recv_fifo.WaitForData();
}

//-------------------------------------------------------------------------------------------------
void Connection::WaitSendComplete() {
	
	boost::unique_lock<boost::mutex> lock(m_stream->m_send_lock);
	while( m_stream->m_sending ) {
		m_stream->m_cv_send_complete.wait( lock );
	}
}

//-------------------------------------------------------------------------------------------------
void Connection::Stream::Write( Packet *p ) {
	m_send_fifo.Push(p);
	{
		boost::lock_guard<boost::mutex> lock(m_send_lock);
		if( m_sending ) return;
		m_send_read = 0;
		m_send_write = 0;
		m_send_packet = 0;
		m_sending = true;
		ContinueSend();
	}
}

//-------------------------------------------------------------------------------------------------
void Connection::Write( Packet *p ) { 

	m_stream->Write( p ); 
}

//-------------------------------------------------------------------------------------------------
void Connection::SetEventHandler( EventHandler *handler ) {
	m_event_handler = handler;
}

//-------------------------------------------------------------------------------------------------
const std::string &Connection::GetHostname() const {
	return m_hostname;
}

}
