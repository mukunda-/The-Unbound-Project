//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "core.h"
#include "stream.h"
#include "resolver.h"
#include "util/minmax.h"
#include "error.h"

namespace Net {

//-----------------------------------------------------------------------------
namespace {
	const int READ_BUFFER_SIZE = 4096;

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

	if( m_read_avail > 0 ) {
		std::istream is( &m_read_buffer );
		while( m_read_avail ) {

			// loop until processor returns 0 or there is no more data.
			int amount = ProcessInput( is, m_read_avail );
			if( amount == 0 ) break;
			m_read_avail -= amount;
		}
	}

	// TODO catch parse error exception and terminate connection.

	if( !m_shutdown ) {
		// receive next data chunk
		ReceiveNext();
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
void Stream::ReceiveNext() {

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
void Stream::OnSend( const boost::system::error_code& error, 
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
			m_sending   = false;
			m_cv_send_complete.notify_all();
			if( !m_connected ) { 
				// Close was called already.
				m_socket.close();
				return;
			} else {
				m_connected = false;
			}
		}
		
		DoClose();
		return;
	}

	m_send_buffers[1-m_send_buffer_index].consume( bytes_transferred );
	SendNext(); 
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Stop the sending thread and signal waiters.
///
void Stream::StopSend() {
	m_sending = false;
	m_cv_send_complete.notify_all();

	if( m_close_after_send ) {
		m_socket.close();
	}
	return;
}

/// ---------------------------------------------------------------------------
/// [PRIVATE] Put data on the line or signal that the send has
///           completed.
///
void Stream::SendNext() {
	// we own one buffer, but not the other.
	// 1 - index for ours.

	if( m_send_buffers[1-m_send_buffer_index].size() == 0 ) {

		// lock for accessing the other buffer and send variables
		std::lock_guard<std::mutex> lock( m_lock );

		if( m_send_buffer_locked ) {
			// a thread is busy writing to the send buffer
			// this process will resume after they release it.

			StopSend();
			return;
		}

		if( m_send_buffers[m_send_buffer_index].size() == 0 ) {
			// no more data to send, stop the writing service.

			StopSend();
			return;
		}

		// swap the buffers.
		m_send_buffer_index = 1-m_send_buffer_index;
	}
	
	m_socket.async_write_some( 
		m_send_buffers[1-m_send_buffer_index].data(), m_strand.wrap(
			boost::bind( &Stream::OnSend, this, 
						 boost::asio::placeholders::error, 
						 boost::asio::placeholders::bytes_transferred )));
							
}

void Stream::Init() {

	// this doesn't work:
	// allow lingering for 30 seconds to finish unsent sending data on shutdown
//	boost::system::error_code ec; //ignore error, maybe log message
//	boost::asio::socket_base::linger option(true, 30);
//	socket.set_option(option,ec);
}

// todo merge constructors with c++11 power
// delete above init function.
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
	assert( m_sending   == false );
	assert( m_receiving == false );
	 
	// the socket should not be open when the destructor is called.
	//m_socket.close();	 
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

		if( m_sending ) {
			m_close_after_send = true;
			return; // the sending `thread` will close the socket.
		}
		
		// TODO: do we need to linger here for the data to be sent? 
	}

	m_socket.close();
}

//-----------------------------------------------------------------------------
void Stream::SetConnected() {
	{
		std::lock_guard<std::mutex> lock(m_lock);
		if( m_shutdown ) return;

		m_connected = true; 
		ReceiveNext();  
		m_sending = true;
	}
	SendNext();
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
void Stream::WaitSend() {
	
	std::unique_lock<std::mutex> lock( m_lock );
	while( m_sending ) {
		m_cv_send_complete.wait( lock );
	}
}

//-----------------------------------------------------------------------------
auto Stream::AcquireSendBuffer() -> SendLock {
	{
		std::unique_lock<std::mutex> lock( m_lock );
		while( m_send_buffer_locked ) {
			// someone else is writing to the send buffer
			// wait for them.
			m_cond_sendbuffer_unlocked.wait( lock );
		}
		m_send_buffer_locked = true;
	}
	
	return SendLock( *this, m_send_buffers[m_send_buffer_index] );
}

//-----------------------------------------------------------------------------
void Stream::ReleaseSendBuffer( bool start ) {
	{
		std::lock_guard<std::mutex> lock( m_lock );

		if( start && m_connected && !m_sending ) {
			m_sending = true;
			m_strand.post( boost::bind( &Stream::SendNext, 
										 shared_from_this() ));
		}

		m_send_buffer_locked = false;
	}
	m_cond_sendbuffer_unlocked.notify_one();
}
/*
//-----------------------------------------------------------------------------
void Stream::Write( Message &msg ) { 
	std::lock_guard<std::mutex> lock( m_lock );

	std::ostream stream( &m_send_buffers[m_send_buffer_index] );
	msg.Write( stream );
	// todo: catch write error and terminate stream.
	
	if( !m_connected ) return;
	if( m_sending ) return;
	m_sending = true; 
	m_strand.post( boost::bind(	&Stream::SendNext, shared_from_this() ));
}
*/

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
