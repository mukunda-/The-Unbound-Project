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
#include "netevents.h"
#include "system/system.h"

namespace Net {

/// -----------------------------------------------------------------------
/// A Stream is a network connection. 
///
class Stream : 
		public std::enable_shared_from_this<Stream>, 
		public Asev::Source {
private:
	void *m_userdata; 

	// where this connection is coming from/going to
	std::string m_hostname;

	System::Service &m_service;
		
	// tcp socket
	boost::asio::ip::tcp::socket m_socket;

	// internal buffer size for storing data to be sent
	static const int BUFFER_SIZE = 8*1024;
		
	// complete packets that have been received
	PacketFIFO m_recv_fifo;

	// partial packet buffer
	Packet *m_recv_packet; 

	uint8_t m_recv_buffer[BUFFER_SIZE]; 
	int m_recv_size;
	int m_recv_write;
	bool m_receiving;
	std::mutex m_recv_lock;
	std::condition_variable m_cv_recv_complete;
		
	PacketFIFO m_send_fifo; // packets waiting to be sent
	Packet *m_send_packet; // packet currently being transmitted
	uint8_t m_send_buffer[BUFFER_SIZE]; 
	int m_send_read; // position in packet; >=2 is data, 0,1 is header (size)
	int m_send_write; // position in send buffer
	bool m_sending;
	std::mutex m_send_lock;
	std::condition_variable m_cv_send_complete;

	// connected is FALSE upon construction
	// TRUE upon connection
	// and FALSE after the connection ends
	//
	// that is the lifetime of the object,
	// and it should not be revived.
	std::atomic_bool m_connected; 

	// shutdown is FALSE upon construction
	// and TRUE after Close is called.
	std::atomic_bool m_shutdown;
	 
		
	int ProcessDataRecv( const uint8_t *data, int size );
	void OnReceive( const boost::system::error_code& error, 
					size_t bytes_transferred );
	void OnDataSent( const boost::system::error_code& error,
					size_t bytes_transferred );
	void ContinueSend();

	void OnAccept( const boost::system::error_code &error );
	void OnConnect( const boost::system::error_code &error );
	void OnResolve( const boost::system::error_code& ec, 
			boost::asio::ip::tcp::resolver::iterator it );
		 
	void StartReceive( bool first ); 
	void StopReceive(); 

	void CloseAfterSend();

public:
	Stream( System::Service &service );
	Stream();
	~Stream();
	void Init();
		  
	/// -----------------------------------------------------------------------
	/// Close the stream.
	///
	/// This function initiates a clean shutdown process. After this function
	/// is called, receive events will be discarded and the stream will 
	/// delete itself after all pending send data is put out on the line.
	///
	void Close();

	/// -----------------------------------------------------------------------
	/// Get the underlying boost asio socket object
	///
	/// @return TCP socket.
	///
	boost::asio::ip::tcp::socket &Socket() { return m_socket; }
		
	/// -----------------------------------------------------------------------
	/// Block until data is received.
	///
	/// This will not return if the data is handled within a receive event.
	///
	/// @throws RemoteDisconnected if the socket is disconnected while it is
	///         waiting.
	///
	void WaitForData();

	/// -----------------------------------------------------------------------
	/// Wait until all data in the output queue is put out on the line.
	///
	void WaitSend();

	/// -----------------------------------------------------------------------
	/// Read a buffered packet, and remove it from the queue
	///
	/// @returns A packet, or nullptr if the receive queue was empty. The
	///          packet must be deleted with Packet::Delete()
	///
	Packet *Read();

	/// -----------------------------------------------------------------------
	/// Place a packet in the output queue.
	/// 
	/// @param p Packet to queue.
	///
	void Write( Packet *p );
	
	/// -----------------------------------------------------------------------
	/// Remove/disable the event handler for this stream.
	///
	void RemoveEventHandler();

	/// -----------------------------------------------------------------------
	/// Get hostname of last connect operation
	///
	/// Returns in format "address:service"
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

	typedef std::shared_ptr<Stream> ptr;
};

}