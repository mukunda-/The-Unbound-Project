//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once
 
#if !defined(_MUTEX_) || !defined(_CONDITION_VARIABLE_) || !defined(_MEMORY_)
#  error "Requires: <mutex> <condition_variable> <memory>"
#endif

#if !defined(_ATOMIC_)
#  error "Requires: <atomic>"
#endif

#include "packetfifo.h"
#include "events.h"
#include "system/system.h"
#include "basiclistener.h"
#include "message.h"
#include "types.h"

namespace Net {

/// ---------------------------------------------------------------------------
/// A Stream is a network connection. You need to implement this class
/// to handle your base protocol format.
///
class Stream : 
		public std::enable_shared_from_this<Stream>, 
		public Asev::Source {

public:

	enum class StreamState {
		NEW,		// a new stream object
		CONNECTING,	// connect is in progress
		LISTENING,	// socket is listening for connections
		CONNECTED,	// a connection is live
		CLOSING,	// the connection should close when 
					// all data is finished sending
		CLOSED,		// the connection is closed
		FAILURE		// the connection failed with an error
	};

	Stream( System::Service &service );
	Stream();
	~Stream();

	Stream( Stream&  ) = delete;
	Stream( Stream&& ) = delete;
	Stream& operator=( Stream&  ) = delete;
	Stream& operator=( Stream&& ) = delete;

		  
	/// -----------------------------------------------------------------------
	/// Close the stream.
	///
	/// This function initiates a clean shutdown process. After this function
	/// is called, receive events will be discarded and the stream will 
	/// delete itself after all pending send data is put out on the line.
	///
	void Close();
	  
	/// -----------------------------------------------------------------------
	/// Listen and accept an incoming connection.
	///
	/// This does not block, and will send an Accepted or AcceptError
	/// event.
	///
	/// @param listener Active listener to wait on.
	///
	void Listen( BasicListener &listener );

	/// -----------------------------------------------------------------------
	/// Try to connect to a remote point. Emits a Connected event and
	/// returns or throws an exception on failure.
	///
	/// @param host    Remote host address.
	/// @param service Service/port number.
	///
	/// @throws boost::system::system_error on failure.
	///
	void Connect( const std::string &host, const std::string &service );
	
	/// -----------------------------------------------------------------------
	/// Make a remote connection asynchronously.
	///
	/// Will emit a Connected or ConnectError event.
	///
	/// @param host    Remote address.
	/// @param service Service/port number to connect to. 
	///
	void ConnectAsync( const std::string &host, const std::string &service );

	/// -----------------------------------------------------------------------
	/// Secure mode.
	///
	/// Calling this will switch the stream into a secure communication
	/// mode.
	///
	/// @param context SSL context to use.
	///
	void Secure( SSLContextPtr &context );

	/// -----------------------------------------------------------------------
	/// Get the underlying boost asio socket object
	///
	/// @return TCP socket.
	///
	boost::asio::ip::tcp::socket &Socket() { return m_socket; }
	
	/// -----------------------------------------------------------------------
	/// @returns The service object for this stream.
	/// 
	System::Service &GetService() { return m_service; }
	
	/// -----------------------------------------------------------------------
	/// Wait until all data in the output queue is put out on the line.
	///
	void WaitSend();
	 
	/// -----------------------------------------------------------------------
	/// Get hostname of last connect operation.
	///
	/// @returns in format "address:service"
	///
	const std::string &GetHostname() const;
		 
	/// -----------------------------------------------------------------------
	/// Set arbitrary user-defined data.
	///
	/// @param data Pointer to user-defined data.
	///
	template <class T> void SetUserData( T *data ) { 
		m_userdata = data;
	}
	
	/// -----------------------------------------------------------------------
	/// Get arbitrary user-defined data.
	///
	/// @returns Pointer to user-defined data.
	///
	template <class T> T *GetUserData() {
		return static_cast<T*>(m_userdata);
	}
	
	/// -----------------------------------------------------------------------
	/// Obtain a casted reference.
	///
	template <class T> T& Cast() {
		return static_cast<T&>(*this);
	}

	typedef std::shared_ptr<Stream> ptr;

	//-------------------------------------------------------------------------
protected:
	class SendLock;
	friend class WriterBase;
	 
	/// -----------------------------------------------------------------------
	/// Process input will be called repeatedly until you return zero or 
	/// use all of the available bytes.
	///
	/// @param stream          Stream to read from.
	/// @param bytes_available Number of bytes available in the stream.
	///
	/// @return Number of bytes read from stream.
	///         (function will be repeated if this is zero
	///          or less than bytes_available.)
	///
	/// @throws ParseError if the input is invalid.
	///
	virtual int ProcessInput( 
		//std::istream &stream,
		int bytes_available ) = 0;
	
	/// -----------------------------------------------------------------------
	/// Create/release send locks.
	///
	SendLock AcquireSendBuffer();
	void ReleaseSendBuffer( bool start );

	std::istream &GetInputStream() { return m_input_stream; }
	
	/// -----------------------------------------------------------------------
	/// These are called during certain events in addition to the 
	/// ASEV interface.
	///
	virtual void Accepted() {}
	virtual void AcceptError( const boost::system::error_code & ) {}
	virtual void Connected() {}
	virtual void ConnectError( const boost::system::error_code & ) {}
	virtual void SendFailed( const boost::system::error_code & ) {}
	virtual void Disconnected( const boost::system::error_code & ) {}
	virtual void Receive( Net::Message &nmsg ) {}

private:

	using ssl_socket_t = 
		boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>;

	class ReadError : public std::runtime_error {};

	std::atomic<void*> m_userdata = nullptr; 

	// where this connection is coming from/going to
	std::string m_hostname;

	System::Service &m_service;
	
	// strand used to synchronize operations
	boost::asio::strand m_strand;
	
	// tcp socket
	boost::asio::ip::tcp::socket m_socket;
	std::unique_ptr<ssl_socket_t> m_ssl_socket;

	SSLContextPtr m_ssl_context;
	bool m_secure = false;
	                                                
	// saved connection error
	boost::system::error_code m_conerr;
	 
	// stream for receiving data
	boost::asio::streambuf m_read_buffer;
	std::istream m_input_stream;
	// how many bytes are waiting in the read buffer
	int m_read_avail = 0; 

	// if the receive thread is active (mainly a safety check)
	bool m_receiving = false;
	
	//std::mutex m_send_lock;
	boost::asio::streambuf m_send_buffers[2];
	int m_send_buffer_index = 0; // which buffer the outside writes to

	// triggered when the connect operation finishes
	std::condition_variable m_connection_completed;

	// when a thread is writing to the backbuffer
	bool m_send_buffer_locked = false; 
	std::condition_variable m_cond_sendbuffer_unlocked; 

	// if the sending thread is active
	bool m_sending = false;
	std::condition_variable m_cv_send_complete;

	// note that "thread" doesn't mean a physical thread here, 
	// it means an active service task

	bool m_close_after_send = false;
	 
	// for safe outside access.
	std::mutex m_lock;

	// connected is FALSE upon construction
	// TRUE upon connection
	// and FALSE after the connection ends
	//
	// that is the lifetime of the object,
	// and it cannot be revived.
	//bool m_connected = false; 
	// (m_connected was replaced by StreamState)

	// shutdown is FALSE upon construction
	// and TRUE after Close is called.
	bool m_shutdown = false;

	// true if this stream was created from accepting a connection.
	bool m_accepted = false;

	StreamState m_state = StreamState::NEW;
	
	virtual void OnReceive( const boost::system::error_code& error, 
					size_t bytes_transferred );
	virtual void OnSend( const boost::system::error_code& error,
					size_t bytes_transferred );
		 
	void ReceiveNext(); 
	void StopReceive();
	void SendNext();
	void StopSend();
	
	void OnAccept( const boost::system::error_code &error );
	void OnConnect( const boost::system::error_code &error );
	void OnResolve( const boost::system::error_code &error, 
			boost::asio::ip::tcp::resolver::iterator it );
	void OnHandshake( const boost::system::error_code &error );
	void OnShutdown( const boost::system::error_code &error );

	void SetConnectedState();
	void SetConnectedFailed( const boost::system::error_code &error );
	void SignalConnectionCompleted();

	//bool ParseMessage( std::istream &is );
	
	//void SetConnected();

	void DoClose();
	void TryClose( bool close_socket );

protected:
	
};

/// ---------------------------------------------------------------------------
/// A send lock holds a buffer from a stream to write to.
///
class Stream::SendLock {

	bool m_locked = false;
	Stream &m_parent;
	boost::asio::streambuf &m_buffer;

public:
	// create new
	SendLock( Stream &parent, boost::asio::streambuf &buffer );

	// or move
	SendLock( SendLock &&lock );
		
	SendLock( SendLock & ) = delete;
	SendLock& operator=( SendLock & ) = delete;
	SendLock& operator=( SendLock && ) = delete;


	~SendLock();

	Stream &Stream() {
		return m_parent;
	}

	boost::asio::streambuf &Buffer() {
		return m_buffer;
	}
}; 


}