//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

// Network Connection Class

#pragma once

#include "network/listener.h"
#include "network/packetfifo.h"
#include "network/nwcore.h"
#include "util/minmax.h"

namespace Network {
  
//-----------------------------------------------------------------------------
class Connection {  

public:

	class EventHandler {

	public:
		/// -------------------------------------------------------------------
		/// Called when an incoming connection is successful.
		///
		virtual void AcceptedConnection() {}

		/// -------------------------------------------------------------------
		/// Called from a listening connection when an error occurs when
		/// trying to accept an incoming connection.
		///
		/// \param error The error code.
		///
		virtual void AcceptError( 
				const boost::system::error_code &error ) {}

		/// -------------------------------------------------------------------
		/// Called during an asynchronous connection when the resolver
		/// fails to resolve a host name.
		///
		/// \param error The error code.
		///
		virtual void CantResolve( 
				const boost::system::error_code &error ) {}

		/// -------------------------------------------------------------------
		/// Called during an asynchronous connection when the connection
		/// fails.
		///
		/// \param error The error code.
		///
		virtual void ConnectError( 
				const boost::system::error_code &error ) {}

		/// -------------------------------------------------------------------
		/// Called when an outgoing connection is successful.
		///
		virtual void Connected() {}

		/// -------------------------------------------------------------------
		/// Called when the connection is closed.
		///
		/// \param error The error code.
		///
		virtual void Disconnected( 
				const boost::system::error_code &error ) {}

		/// -------------------------------------------------------------------
		/// Called when the connection fails during an outgoing operation.
		///
		/// \param error The error code.
		///
		virtual void DisconnectedError( 
				const boost::system::error_code &error ) {}

		/// -------------------------------------------------------------------
		/// Called when the connection receives a packet.
		///
		/// \param packet The packet that was received.
		/// \return       Return true to not buffer the packet (handled)
		///               Return false to buffer the packet (not handled)
		///               Buffered packets are obtained from Connection::Read()
		///
		virtual bool Receive( Network::Packet &packet ) { return false; }

	};

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
		class EventLatch {

			boost::lock_guard<boost::mutex> m_lock;
			Stream &m_stream;

			static EventHandler dummy_handler;

		public:
			EventLatch( Stream &stream );

			/// ---------------------------------------------------------------
			/// Get the event handler.
			///
			/// \return The parent's event handler, or an empty handler if no
			///         handler is set or the parent is detached.
			///
			EventHandler *Handler();
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
		
		Network::PacketFIFO m_recv_fifo; // complete packets that have been received
		Network::Packet *m_recv_packet; // partial packet buffer

		boost::uint8_t m_recv_buffer[BUFFER_SIZE]; 
		int m_recv_size;
		int m_recv_write;
		bool m_receiving;
		boost::mutex m_recv_lock;
		boost::condition_variable m_cv_recv_complete;
		
		Network::PacketFIFO m_send_fifo; // packets waiting to be sent
		Network::Packet *m_send_packet; // packet currently being transmitted
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
		void OnReceive( const boost::system::error_code& error, size_t bytes_transferred );
		void OnDataSent( const boost::system::error_code& error, size_t bytes_transferred );
		void ContinueSend();

		void OnAccept( const boost::system::error_code &error );
		void OnConnect( const boost::system::error_code &error );
		void OnResolve( const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator it );

		//int FireEvent( EventType type, void *data=0 );
 
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
	 
	// source: reference to this connection
	// type: see EventType
	// data: variable depending on event type
	// 
	//typedef boost::function< int( Connection  &source, EventType type, void *data ) > event_handler_t;
	EventHandler *m_event_handler; 
	  
	//int FireEvent( EventType type, void *data=0 );
	
	void *m_userdata; 

	//------------------------------------------------------------------------------------------------- 
protected:

public:  
	//-------------------------------------------------------------------------------------------------
	Connection(); 
	~Connection();

	/// -----------------------------------------------------------------------
	/// Enter listening mode.
	///
	/// This starts an asynchronous task that waits for incoming connections.
	/// When something connects, either EVENT_ACCEPTEDCONNECTION or
	/// EVENT_ACCEPTERROR will be triggered, and this listening connection
	/// will become an active connection or a closed connection
	/// respectively.
	///
	/// \param listener Network listener which describes what port to listen
	///                 on.
	///
	void Listen( Network::Listener &listener );

	/// -----------------------------------------------------------------------
	/// Make a remote connection.
	///
	/// This is a blocking function that connects to the specified address.
	///
	/// \param host Address to connect to
	/// \param service Service/port number to connect to.
	/// \return false if the connection failed.
	///
	bool Connect( const std::string &host, const std::string &service );

	/// -----------------------------------------------------------------------
	/// Make a remote connection asynchronously.
	///
	/// This starts a connection task that completes in the background.
	///
	/// Fires one of these events:
	///  EVENT_CANTRESOLVE
	///    Triggered if the host address can't be resolved.
	///  EVENT_CONNECTERROR
	///  
	void ConnectAsync( const std::string &host, const std::string &service );

	void Close();
	
	// get socket base
	boost::asio::ip::tcp::socket &Socket();

	// block until data is received
	// note: this will not break if the data is handled within the receive event
	//
	void WaitForData();

	// wait until all data in the output stack is put out on the line
	//
	void WaitSendComplete();
	
	// read a buffered packet
	// returns 0 if received stack is empty
	//
	Packet *Read();

	// place a packet on the output queue
	// 
	void Write( Packet *p );

	// set the function to receive events from this connection
	//
	void SetEventHandler( EventHandler cb );

	// get hostname of last connect operation
	//
	// returns in format "address:service"
	const std::string &GetHostname() const;

	template <class T> void SetUserData( T *data ) {
		m_userdata = data;
	}
	template <class T> T *GetUserData() {
		return static_cast<T*>(m_userdata);
	}
};


}
