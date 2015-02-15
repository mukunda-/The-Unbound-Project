//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "core.h"
#include "stream.h"
#include "resolver.h"
#include "util/minmax.h"
#include "error.h"
#include "sslcontext.h"

namespace Net {

extern Instance *g_instance;

//-----------------------------------------------------------------------------
namespace {
	const int READ_BUFFER_SIZE = 4096;

}
	 
/// ---------------------------------------------------------------------------
/// Callback for receiving data.
///  
void Stream::OnReceive( const boost::system::error_code& error, 
								    size_t bytes_transferred ) {
	
	if( error ) {
		// receive error: the remote has disconnected or an error occurred.
		m_receiving = false;

		if( m_shutdown ) return; // socket is already closed.

		if( error == boost::asio::error::eof ) {
			// clean shutdown
			DoClose( false, boost::system::error_code() );
		} else {
			DoClose( true, error );
		}
		
		return;
		/*
		// TODO closed/failure state depending on error.
		std::lock_guard<std::mutex> lock( m_lock );
		if( !m_shutdown || error == boost::asio::error::eof ) {
		
			TryClose( true );
		} else {
			TryClose( false );
		}
		*/

		// send event
//		Disconnected( error );
	//	Events::Stream::Dispatcher( shared_from_this() )
//			.Disconnected( error );
		
		// shutdown stream.
		//StopReceive();
		//DoClose();
		return;
	}

	if( m_shutdown ) {
		m_receiving = false;
		return; // socket is closed. ignore any further data.
	}
	
	m_read_buffer.commit( bytes_transferred );
	m_read_avail += (int)bytes_transferred;

	if( m_read_avail > 0 ) {
		//std::istream is( &m_read_buffer );
		while( m_read_avail ) {

			// loop until processor returns 0 or there is no more data.
			int amount = ProcessInput(  m_read_avail );
			if( amount == 0 ) break;
			m_read_avail -= amount;
		}
	}

	// TODO catch parse error exception and terminate connection.

	ReceiveNext();
	/*
	if( m_state == StreamState::CONNECTED ) {
		// receive next data chunk
		ReceiveNext();
	} else {
		// shutdown.
		m_receiving = false;

		std::lock_guard<std::mutex> lock( m_lock );
		TryClose( false );

		// send event
//		Disconnected( boost::system::error_code() );
//		Events::Stream::Dispatcher( shared_from_this() )
//			.Disconnected( boost::system::error_code() );
		return;
	}*/
}

/// ---------------------------------------------------------------------------
/// Unset the receiving flag and signal the condition.
///  
void Stream::StopReceive() {
	// the signal isn't used anymore.

	//std::lock_guard<std::mutex> lock(m_recv_lock); 
	m_receiving = false;
	//m_cv_recv_complete.notify_all();
}

/// ---------------------------------------------------------------------------
/// Set the receiving flag and start listening for data.
///
/// @param first TRUE if this is the first receive call, i.e. right after
///        the connection is established. FALSE if it is a continued 
///        receive call.
///  
void Stream::ReceiveNext() {

	m_receiving = true;
	
	boost::asio::streambuf::mutable_buffers_type buffers = 
		m_read_buffer.prepare( READ_BUFFER_SIZE );
	
	if( !m_secure ) {
		m_socket.async_read_some( 
			buffers, 
			m_strand->wrap( boost::bind( 
				&Stream::OnReceive, shared_from_this(), 
				boost::asio::placeholders::error, 
				boost::asio::placeholders::bytes_transferred )));
	} else {
		m_ssl_socket->async_read_some( 
			buffers, 
			m_strand->wrap( boost::bind( 
				&Stream::OnReceive, shared_from_this(), 
				boost::asio::placeholders::error, 
				boost::asio::placeholders::bytes_transferred )));
	}
			
}

/// ---------------------------------------------------------------------------
/// Callback for when some data is put out onto the line.
///
void Stream::OnSend( const boost::system::error_code& error, 
						 size_t bytes_transferred ) {
	
	if( error ) {
		// error during send, this is an "unexpected" error.
		std::lock_guard<std::mutex> lock( m_lock );
		m_sending = false;

		DoClose( true, error );
		m_cv_send_complete.notify_all();
		return;
	}

	m_send_buffers[1-m_send_buffer_index].consume( bytes_transferred );
	SendNext(); 
}

/// ---------------------------------------------------------------------------
/// Stop the sending thread and signal waiters.
///
void Stream::StopSend() {
	// *** m_lock has to be locked outside.

	using namespace boost::asio::ip; 
	m_sending = false;
	m_cv_send_complete.notify_all();

	if( m_shutdown ) {
		m_state = StreamState::CLOSED;

		if( m_secure ) {
			m_ssl_socket->async_shutdown( 
				boost::bind( &Stream::OnShutdown, shared_from_this(), 
							 boost::asio::placeholders::error ));
		} else {
			boost::system::error_code ec;
			m_socket.shutdown( tcp::socket::shutdown_both, ec );  
			
		}

	}
	return;
}

//-----------------------------------------------------------------------------
void Stream::OnShutdown( const boost::system::error_code &error ) {
	
}

/// ---------------------------------------------------------------------------
/// Put data on the line or signal that the send has
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
	
	if( !m_secure ) {
		m_socket.async_write_some( 
			m_send_buffers[1-m_send_buffer_index].data(), m_strand->wrap(
				boost::bind( &Stream::OnSend, shared_from_this(), 
							 boost::asio::placeholders::error, 
							 boost::asio::placeholders::bytes_transferred )));
	} else {
		m_ssl_socket->async_write_some(
			m_send_buffers[1-m_send_buffer_index].data(), m_strand->wrap(
				boost::bind( &Stream::OnSend, shared_from_this(), 
							 boost::asio::placeholders::error, 
							 boost::asio::placeholders::bytes_transferred )));
	}
} 
 
//-----------------------------------------------------------------------------
Stream::Stream( System::Service &service ) : 
			m_service(service), m_socket( m_service() ), 
			m_my_strand( m_service() ), m_input_stream( &m_read_buffer ) {

	m_strand = &m_my_strand;
	  
	// this doesn't work:
	// allow lingering for 30 seconds to finish sending data on shutdown
//	boost::system::error_code ec; //ignore error, maybe log message
//	boost::asio::socket_base::linger option(true, 30);
//	socket.set_option(option,ec);
}

//-----------------------------------------------------------------------------
Stream::Stream() : Stream( Net::DefaultService() ) {}

//-----------------------------------------------------------------------------
void Stream::Close() {

	m_service.Post( m_strand->wrap( 
		boost::bind( &Stream::DoClose, shared_from_this(), 
				     false, boost::system::error_code(), false )));
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
/*
///----------------------------------------------------------------------------
void Stream::TryClose( bool failure ) {
	// Close the socket if both receive and send threads have ended.
	// if "failure" is set, then something went wrong and 
	// we force close the socket.

	using namespace boost::asio::ip; 
	// assert mutex is owned?

	if( failure ) {
		m_socket.close();
		m_state = StreamState::FAILURE; 
	} 

	if( !m_receiving && !m_sending ) {
		m_state = StreamState::CLOSED;

		boost::system::error_code ec;
		m_socket.shutdown( tcp::socket::shutdown_both, ec ); 

		Disconnected( boost::system::error_code() );
		Events::Stream::Dispatcher( shared_from_this() )
			.Disconnected( boost::system::error_code() );
	}
}*/

/// ---------------------------------------------------------------------------
/// Cleanly close the socket.
///
/// @param failure true if there was an error and the socket should be
///                forcefully closed.
/// @param error   Error code for failure state.
/// @param locked  true if m_lock is already locked outside of this function.
///
void Stream::DoClose( bool failure, const boost::system::error_code &error, 
	                  bool locked ) {
	using namespace boost::asio::ip; 
	
	std::unique_lock<std::mutex> lock( m_lock, std::defer_lock );
	if( !locked ) lock.lock();

	if( m_shutdown ) return;
	m_shutdown = true;
	
	StreamState oldstate = m_state;
	
	m_shutdown = true;

	if( oldstate == StreamState::CONNECTED ) {

		Disconnected( error );
		Events::Stream::Dispatcher( shared_from_this() )
			.Disconnected( error );
			
		if( failure ) {
			m_socket.close();
			return;
		}
		
		if( m_sending ) {
			// the send thread will shutdown the socket when
			// it's done.

			boost::system::error_code ec;
			m_state = StreamState::CLOSING; 

		} else {
			
			m_state = StreamState::CLOSED;
			if( m_secure ) {
				m_ssl_socket->async_shutdown( 
					boost::bind( &Stream::OnShutdown, shared_from_this(), 
								 boost::asio::placeholders::error ));
			} else {
				boost::system::error_code ec;
				m_socket.shutdown( tcp::socket::shutdown_both, ec );  
			}
			
		} 

	} else if( oldstate == StreamState::CONNECTING || 
			   oldstate == StreamState::LISTENING ) {
		// this isn't a live connection, so simply close it.
		m_socket.close();
		m_state = StreamState::CLOSED;

	} else if( oldstate == StreamState::NEW ) {

		// for whatever reason, close was called on a brand new socket.
		m_state = StreamState::CLOSED;
	}
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

		if( start && (m_state == StreamState::CONNECTED 
			|| m_state == StreamState::CLOSING) && !m_sending ) {

			m_sending = true;
			m_strand->post( boost::bind( &Stream::SendNext, 
										 shared_from_this() ));
		}

		m_send_buffer_locked = false;
	}
	m_cond_sendbuffer_unlocked.notify_one();
}

//-----------------------------------------------------------------------------
const std::string &Stream::GetHostname() const {
	return m_hostname;
} 

//-----------------------------------------------------------------------------
void Stream::Connect( const std::string &host, const std::string &service ) {
	assert( !m_shutdown );
	//assert( !m_connected );
	assert( m_state == StreamState::NEW );

	std::unique_lock<std::mutex> lock( m_lock );
	ConnectAsync( host, service );
	m_connection_completed.wait( lock );
	
	if( m_conerr ) throw m_conerr;
}

//-----------------------------------------------------------------------------
void Stream::ConnectAsync( const std::string &host, 
						   const std::string &service ) {
	assert( !m_shutdown );
	//assert( !m_connected );
	assert( m_state == StreamState::NEW );

	m_state = StreamState::CONNECTING;
	m_hostname = host + ":" + service;
	Resolver::CreateThreaded( host, service, 
		m_strand->wrap( boost::bind( 
				&Stream::OnResolve, shared_from_this(), _1, _2 )));
}

/// ---------------------------------------------------------------------------
/// Callback for when an address has been resolved during
/// an async connect op. (or an error)
///
void Stream::OnResolve( const boost::system::error_code &error_code, 
				        boost::asio::ip::tcp::resolver::iterator endpoints ) {
	
	if( error_code ) {
		m_state = StreamState::FAILURE;
		m_conerr = error_code;
		ConnectError( error_code );
		Events::Stream::Dispatcher( shared_from_this() )
			.ConnectError( error_code );

		std::lock_guard<std::mutex> lock( m_lock );
		m_connection_completed.notify_all();

	} else {

		// resolve OK, do connect.
		boost::asio::async_connect( m_socket, endpoints, 
			m_strand->wrap( boost::bind( &Stream::OnConnect, 
					shared_from_this(), 
					boost::asio::placeholders::error )));
	}
}

/// ---------------------------------------------------------------------------
/// Signal a thread that is waiting if they used Connect()
///
void Stream::SignalConnectionCompleted() {
	
	std::lock_guard<std::mutex> lock( m_lock );
	m_connection_completed.notify_all();
}

/// ---------------------------------------------------------------------------
/// Called after a connection is successful to change the state and
/// start the read task.
///
void Stream::SetConnectedState() {
	
	// change state, trigger event and start receive loop
	m_state = StreamState::CONNECTED;

	if( m_accepted ) {
		
		Accepted();
		Events::Stream::Dispatcher( shared_from_this() )
			.Accepted();  

	} else {
		
		Connected();
		Events::Stream::Dispatcher( shared_from_this() )
			.Connected();  
		
	}

	ReceiveNext();
}

/// ---------------------------------------------------------------------------
/// Called after a connection attempt fails to change the state and
/// send an error event.
///
void Stream::SetConnectedFailed( const boost::system::error_code &error ) {
	
	m_state = StreamState::FAILURE;
	m_conerr = error;

	if( m_accepted ) {

		AcceptError( error );
		Events::Stream::Dispatcher( shared_from_this() )
			.AcceptError( error );

	} else {

		ConnectError( error );
		Events::Stream::Dispatcher( shared_from_this() )
			.ConnectError( error );
	}
	
		
}

/// ---------------------------------------------------------------------------
/// Callback for boost::asio::async_connect
///
void Stream::OnConnect( const boost::system::error_code &error ) {
	 
	if( !error ) {

		if( m_secure ) {
			
			// wrap with ssl socket.
			m_ssl_socket.reset( new ssl_socket_t( m_socket, (*m_ssl_context)() ));
			
			// need to perform handshake first
			m_ssl_socket->async_handshake( 
				boost::asio::ssl::stream_base::client, m_strand->wrap(
					boost::bind( &Stream::OnHandshake, shared_from_this(), 
								 boost::asio::placeholders::error )));

			return;
		}
	
		SetConnectedState();
		
	} else {
	
		SetConnectedFailed( error );
	}
	
	SignalConnectionCompleted();
}

/// ---------------------------------------------------------------------------
/// Callback for when async_handshake completes
///
void Stream::OnHandshake( const boost::system::error_code &error ) {
	if( !error ) {
		
		SetConnectedState();
		
	} else {
		
		SetConnectedFailed( error );
	}

	SignalConnectionCompleted();
}

/// ---------------------------------------------------------------------------
/// Callback for when a connection is accepted.
///
void Stream::OnAccept( const boost::system::error_code &error ) {
	if( !error ) {
		m_hostname = m_socket.remote_endpoint().address().to_string();

		if( m_secure ) {
			
			// wrap with ssl socket.
			m_ssl_socket.reset( new ssl_socket_t( m_socket, (*m_ssl_context)() ));

			m_ssl_socket->async_handshake( 
				boost::asio::ssl::stream_base::server, m_strand->wrap(
					boost::bind( &Stream::OnHandshake, shared_from_this(),
							     boost::asio::placeholders::error )));

			return;
		}

		SetConnectedState();
	} else {
		
		SetConnectedFailed( error );
	}
	
	SignalConnectionCompleted();
}

//-----------------------------------------------------------------------------
void Stream::Listen( BasicListener &listener ) {
	assert( !m_shutdown ); 
	assert( m_state == StreamState::NEW );
	m_accepted = true;

	m_state = StreamState::LISTENING;
	listener.AsyncAccept(
		m_socket, 
		m_strand->wrap( boost::bind(
			&Stream::OnAccept, shared_from_this(),
			boost::asio::placeholders::error )));
}
 
//-----------------------------------------------------------------------------
void Stream::Secure( SSLContextPtr &context ) {

	assert( m_state == StreamState::NEW );
	
	// add ownership to shared ptr
	m_ssl_context = context;

	// wrap our existing socket with ssl
	// low-level socket operations still use m_socket

//	m_ssl_socket.reset( new ssl_socket_t( m_socket, (*m_ssl_context)() ));
	// (it seems we need to create the socket while within the ssl strand)

	m_secure = true;

	m_strand = &g_instance->GetSSLStrand();
}

}
