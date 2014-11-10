//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "stream.h"
#include "netevents.h"

namespace Net { namespace Events {

namespace Stream {

	 //-------------------------------------------------------------------------
	Event::Event( StreamPtr &stream, Classes event_class ) : 
			m_stream(stream), m_class( event_class ) {
	}

	//-------------------------------------------------------------------------
	ErrorEvent::ErrorEvent( 
				StreamPtr &stream, 
				const boost::system::error_code &error, 
				Classes event_class ) :
			m_error(error), Event(stream,event_class) {
	}

	//-------------------------------------------------------------------------
	Event::Accepted::Accepted( StreamPtr &stream ) : 
			Event( stream, ACCEPTED ) {
	}
	 
	//-------------------------------------------------------------------------
	Event::AcceptError::AcceptError( 
				StreamPtr &stream, 
				const boost::system::error_code &error ) :
			ErrorEvent( stream, error, ACCEPTERROR ) {
	}

	//-------------------------------------------------------------------------
	Event::ConnectError::ConnectError( 
				StreamPtr &stream, 
				const boost::system::error_code &error ) :
			ErrorEvent( stream, error, CONNECTERROR ) {
	}

	//-------------------------------------------------------------------------
	Event::Connected::Connected( StreamPtr &stream ) :
			Event( stream, CONNECTED ) {
	}
	
	//-------------------------------------------------------------------------
	Event::Disconnected::Disconnected( 
				StreamPtr &stream, 
				const boost::system::error_code &error ) :
			ErrorEvent( stream, error, DISCONNECTED ) {
	}

	//-------------------------------------------------------------------------
	Event::SendFailed::SendFailed( 
				StreamPtr &stream, 
				const boost::system::error_code &error ) :
			ErrorEvent( stream, error, SENDFAILED ) {
	}

	//-------------------------------------------------------------------------
	Event::Receive::Receive( StreamPtr &stream, Packet &packet ) :
			Event( stream, RECEIVE ), m_packet(packet) {
		
	}

	//-------------------------------------------------------------------------
	int Handler::Handle( Asev::Event &e ) {
		if( e.Info() != typeid( Event ) ) return 0;

#define GETERROR static_cast<ErrorEvent&>(e).GetError()
		  
		int result = 0;
		Event &stream_event = static_cast<Event&>(e);
		switch( stream_event.m_class ) {
		case Event::ACCEPTED:
			Accepted( stream_event.m_stream );
			break;
		case Event::ACCEPTERROR:
			AcceptError( stream_event.m_stream, GETERROR );
			break;
		case Event::CONNECTED:
			Connected( stream_event.m_stream );
			break;
		case Event::CONNECTERROR:
			ConnectError( stream_event.m_stream, GETERROR );
			break;
		case Event::DISCONNECTED:
			Disconnected( stream_event.m_stream, GETERROR );
			break;
		case Event::RECEIVE:
			result = Receive( stream_event.m_stream, 
							  static_cast<Event::Receive&>(e).GetPacket() );
			break;
		case Event::SENDFAILED:
			SendFailed( stream_event.m_stream, GETERROR );
			break;
		}
		return result;
	}

	//-------------------------------------------------------------------------
	Dispatcher::Dispatcher( StreamPtr &stream ) : 
				m_stream(stream), 
				Asev::Dispatcher(*stream) {

	}
	
	//-------------------------------------------------------------------------
	void Dispatcher::Accepted() {
		Send( Event::Accepted( m_stream ) );
	}

	//------------------------------------------------------------------------- 
	void Dispatcher::AcceptError( const boost::system::error_code &error ) {
		Send( Event::AcceptError( m_stream, error ) );
	}

	//-------------------------------------------------------------------------
	void Dispatcher::ConnectError( const boost::system::error_code &error ) {
		Send( Event::ConnectError( m_stream, error ) );
	}

	//-------------------------------------------------------------------------
	void Dispatcher::Connected() {
		Send( Event::Connected( m_stream ) );
	}

	//-------------------------------------------------------------------------
	void Dispatcher::Disconnected( const boost::system::error_code &error ) {
		Send( Event::Disconnected( m_stream, error ) );
	}

	//-------------------------------------------------------------------------
	void Dispatcher::SendFailed( const boost::system::error_code &error ) {
		Send( Event::SendFailed( m_stream, error ) );
	}

	//-------------------------------------------------------------------------
	bool Dispatcher::Receive( Packet &packet ) {
		return Send( Event::Receive( m_stream, packet ) ) != 0;
	}
}

}}
