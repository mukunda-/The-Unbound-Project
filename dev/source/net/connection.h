//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2014, Mukunda Johnson, All rights reserved. =========//

// Network Connection Class

#pragma once

#include "listener.h"
#include "packetfifo.h"
#include "nwcore.h"
#include "util/minmax.h"

namespace Net {
  
/// ---------------------------------------------------------------------------
/// A Connection manages a TCP socket with boost::asio.
///
class Connection {  

public:

	

private:

	/// -----------------------------------------------------------------------
	/// A Stream handles the actual network connection.
	///
	/// When the Connection is destructed, the stream is detached and keeps
	/// running until the connection is ready to close.
	///
	class Stream : public std::enable_shared_from_this<Stream> {
	private:

		/// -------------------------------------------------------------------
		/// Class to provide safe access to the event handler.
		///
		/// When instantiated, it places a lock on the parent connection,
		/// or silently ignores requests if the connection is detached.
		///
		class EventLatch {

			// this class makes a lock on m_handler_mutex
			boost::lock_guard<boost::mutex> m_lock;

			// owner of this latch
			Stream &m_stream;

			static EventHandler dummy_handler;

		public:
			EventLatch( Stream &stream );

			// Event wrappers
			void Accepted();
			void AcceptError( const boost::system::error_code &error ); 
			void ConnectError( const boost::system::error_code &error );
			void Connected();
			void Disconnected( const boost::system::error_code &error );
			void DisconnectedError( const boost::system::error_code &error );
			bool Receive( Packet &packet );

		};

		/// -------------------------------------------------------------------
		/// Wait until any pending data is put out on the line and 
		/// then close the socket.
		///
		void CloseAfterSend();
	public:

		// internal buffer size for storing data to be sent
		static const int BUFFER_SIZE = 8*1024;

		// tcp socket
		boost::asio::ip::tcp::socket m_socket;
		
		// complete packets that have been received
		PacketFIFO m_recv_fifo;

		// partial packet buffer
		Packet *m_recv_packet; 

		boost::uint8_t m_recv_buffer[BUFFER_SIZE]; 
		int m_recv_size;
		int m_recv_write;
		bool m_receiving;
		boost::mutex m_recv_lock;
		boost::condition_variable m_cv_recv_complete;
		
		PacketFIFO m_send_fifo; // packets waiting to be sent
		Packet *m_send_packet; // packet currently being transmitted
		boost::uint8_t m_send_buffer[BUFFER_SIZE]; 
		int m_send_read; // position in packet 2+ is data, 0,1 is header (size)
		int m_send_write; // position in send buffer
		bool m_sending;
		boost::mutex m_send_lock;
		boost::condition_variable m_cv_send_complete;

		volatile bool m_connected;

		volatile LONG m_shutdown;

		// this becomes null when the connection is destructed (detached):
		Connection *m_parent; 
		boost::mutex m_handler_mutex;

		int ProcessDataRecv( const boost::uint8_t *data, int size );
		void OnReceive( const boost::system::error_code& error, 
						size_t bytes_transferred );
		void OnDataSent( const boost::system::error_code& error,
						size_t bytes_transferred );
		void ContinueSend();

		void OnAccept( const boost::system::error_code &error );
		void OnConnect( const boost::system::error_code &error );
		void OnResolve( const boost::system::error_code& ec, 
				boost::asio::ip::tcp::resolver::iterator it );
		 
		Stream( Connection *p_parent );
		~Stream();
		void StartReceive( bool first=true );
		void Shutdown();

		void Write( Packet *p );
	};

	// the stream stays alive if this class is destroyed to 
	// cleanly close the connection and finish sending data.
	std::shared_ptr<Stream> m_stream;

	// where this connection is coming from/going to
	std::string m_hostname;
	
	EventHandler *m_event_handler; 
	  
	//int FireEvent( EventType type, void *data=0 );
	
	void *m_userdata; 

	//-------------------------------------------------------------------------
protected:

public:  
	//-------------------------------------------------------------------------
	Connection(); 
	~Connection();
	 
	/// -----------------------------------------------------------------------
	/// Make a remote connection.
	///
	/// This is a blocking function that connects to the specified address.
	///
	/// @param host Address to connect to
	/// @param service Service/port number to connect to.
	/// @throws boost::system::system_error on failure.
	///
	void Connect( const std::string &host, const std::string &service );

	/// -----------------------------------------------------------------------
	/// Make a remote connection asynchronously.
	///
	/// This starts a connection task that completes in the background.
	/// 
	/// Fires a connection error event on failure.
	///  
	void ConnectAsync( const std::string &host, const std::string &service );
	
	// To close a connection, destruct the Connection object.
	//void Close();
	
	/// -----------------------------------------------------------------------
	/// Get boost asio socket object
	///
	/// @return TCP socket.
	///
	boost::asio::ip::tcp::socket &Socket();

	/// -----------------------------------------------------------------------
	/// Block until data is received
	///
	/// This will not return if the data is handled within a receive event.
	///
	void WaitForData();

	/// -----------------------------------------------------------------------
	/// wait until all data in the output queue is put out on the line
	///
	void WaitSendComplete();
	
	/// -----------------------------------------------------------------------
	/// Read a buffered packet, and remove it from the queue
	///
	/// @returns A packet, or nullptr if the receive queue was empty.
	///
	Packet *Read();

	/// -----------------------------------------------------------------------
	/// Place a packet in the output queue
	/// 
	/// @param p Packet to queue.
	///
	void Write( Packet *p );

	/// -----------------------------------------------------------------------
	/// Set the event handler for this connection.
	///
	/// @param handler Event handler to use. Pass nullptr to remove the event
	///                handler.
	///
	void SetEventHandler( EventHandler *handler );

	/// -----------------------------------------------------------------------
	/// Get hostname of last connect operation
	///
	/// Returns in format "address:service"
	///
	const std::string &GetHostname() const;

	/// -----------------------------------------------------------------------
	template <class T> void SetUserData( T *data ) {
		
		m_userdata = data;
	}
	template <class T> T *GetUserData() {
		return static_cast<T*>(m_userdata);
	}
};


}