//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include "network/listener.h"
#include "network/packetfifo.h"
#include "network/nwcore.h"
#include "util/minmax.h"

namespace Network {
  
//-------------------------------------------------------------------------------------------------
class Connection {  

public:
	
	enum EventType {
		
		EVENT_NULL, 
		// unused

		EVENT_ACCEPTEDCONNECTION,
		// when listening, an incoming connection was accepted
		//

		EVENT_ACCEPTERROR,
		// when listening, an error occurrred

		EVENT_CANTRESOLVE,
		// error while resolving remote address

		EVENT_CONNECTERROR,
		// could not connect

		EVENT_CONNECTED,
		// connected to endpoint successfully

		EVENT_DISCONNECT, 
		EVENT_DISCONNECT2, 
		// when the connection is closed or interrupted
		// DISCONNECT2 is when the connection is interrupted
		// during an outgoing operation

		EVENT_RECEIVE	 
		// when a packet is received
		// data is a pointer to the received Network::Packet
		// return 0 to have the data buffered in the receive queue (unhandled)
		// return nonzero to delete the data (handled)
	};

private:

	class Stream : public std::enable_shared_from_this<Stream> {
	private:

		void CloseAfterSend();
	public:
		static const int BUFFER_SIZE = 8*1024;
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

		Connection *m_parent;
		boost::mutex m_handler_mutex;

		int ProcessDataRecv( const boost::uint8_t *data, int size );
		void OnReceive( const boost::system::error_code& error, size_t bytes_transferred );
		void OnDataSent( const boost::system::error_code& error, size_t bytes_transferred );
		void ContinueSend();

		void OnAccept( const boost::system::error_code &error );
		void OnConnect( const boost::system::error_code &error );
		void OnResolve( const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator it );

		int FireEvent( EventType type, void *data=0 );
 
		Stream( Connection *p_parent );
		~Stream();
		void StartReceive( bool first=true );
		void Shutdown();

		void Write( Packet *p );
	};

	// the stream stays alive if this class is destroyed to 
	// cleanly close the connection and finish sending data.
	std::shared_ptr<Stream> m_stream;

	std::string m_hostname;
	 
	// source: reference to this connection
	// type: see EventType
	// data: variable depending on event type
	// 
	typedef boost::function< int( Connection  &source, EventType type, void *data ) > event_handler_t;
	event_handler_t m_event_handler; 
	  
	int FireEvent( EventType type, void *data=0 );
	
	void *m_userdata; 

	//------------------------------------------------------------------------------------------------- 
protected:

public:  
	//-------------------------------------------------------------------------------------------------
	Connection(); 
	~Connection();


	void Listen( Network::Listener &listener );
	void Connect( const std::string &host, const std::string &service );
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
	void SetEventHandler( event_handler_t cb );

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
