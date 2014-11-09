//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#ifndef _MUTEX_
#  error "Missing <mutex>."
#endif

#ifndef _MEMORY_
#  error "Missing <memory>."
#endif

#include "stream.h"
#include "packet.h"
 
//-----------------------------------------------------------------------------
namespace Net { namespace Event {


	class StreamEvent : public Event {

	protected:
		
		enum Classes {
			ACCEPTED,
			ACCEPTERROR,
			CONNECTERROR,
			CONNECTED,
			DISCONNECTED,
			SENDFAILED,
			RECEIVE
		};

		Stream &m_stream;
		Classes m_class;

		StreamEvent( Classes event_class );

	public:
		class Accepted;
		class AcceptError;
		class ConnectError;
		class Connected;
		class Disconnected;
		class SendFailed;
		class Receive;
	};

	class StreamErrorEvent : public StreamEvent {

		const boost::system::error_code &m_error;
	public:
		StreamErrorEvent( const boost::system::error_code &error );
		const boost::system::error_code &GetError() { return m_error; }
	};

	class StreamEvent::Accepted : public StreamEvent {
		Accepted();
	};

	class StreamEvent::AcceptError : public StreamErrorEvent {
		AcceptError();
	};

	class StreamEvent::ConnectError : public StreamErrorEvent {
		ConnectError();
	};

	class StreamEvent::Connected : public StreamEvent {
		Connected();
	};

	class StreamEvent::SendFailed : public StreamErrorEvent {
		SendFailed();
	};

	class StreamEvent::Receive : public StreamEvent {
		Receive();
	};


	/// -----------------------------------------------------------------------
	/// The interface for network events.
	///
	class Interface {

	public:
		/// -------------------------------------------------------------------
		/// Called when a new stream is accepted/created.
		///
		/// @param stream The stream associated with the event.
		///
		virtual void Accepted( Stream::ptr &stream ) {}

		/// -------------------------------------------------------------------
		/// Called from a listener when an error occurs when trying to
		/// accept a connection.
		///
		/// @param stream The stream associated with the event.
		/// @param error  The error code.
		///
		virtual void AcceptError( 
				Stream::ptr &stream,
				const boost::system::error_code &error ) {}
		 
		/// -------------------------------------------------------------------
		/// Called when an attempt to connect to a remote point fails.
		///
		/// @param stream The stream associated with the event.
		/// @param error  The error code.
		///
		virtual void ConnectError( 
				Stream::ptr &stream,
				const boost::system::error_code &error ) {}

		/// -------------------------------------------------------------------
		/// Called when a remote connection is established.
		///
		/// @param stream The stream associated with the event.
		///
		virtual void Connected( Stream::ptr &stream ) {}

		/// -------------------------------------------------------------------
		/// Called when a stream is closed.
		///
		/// @param stream The stream associated with the event.
		/// @param error  The error code, to see if the disconnection
		///               was a problem or a normal socket closure.
		///
		virtual void Disconnected( 
				Stream::ptr &stream,
				const boost::system::error_code &error ) {}

		/// -------------------------------------------------------------------
		/// Called when a connection fails during an outgoing operation.
		///
		/// @param stream The stream associated with the event.
		/// @param error  The error code.
		///
		virtual void SendFailed( 
				Stream::ptr &stream,
				const boost::system::error_code &error ) {}

		/// ------------------------------------------------------------------
		/// Called when a stream receives a packet.
		///
		/// @param stream The stream associated with the event.
		/// @param packet The packet that was received.
		/// @return       Return true to not buffer the packet (handled)
		///               Return false to buffer the packet (not handled)
		///               Buffered packets are obtained from Stream::Read()
		///
		virtual bool Receive( 
				Stream::ptr &stream,
				Packet &packet ) { 
			
			return false; 
		}
	};
	
	
	
	
	/// -----------------------------------------------------------------------
	/// Dispatcher simplified for a stream.
	/// 
	class StreamDispatcher : public Dispatcher {

		Stream &m_stream;
	public:
		StreamDispatcher( Stream &parent );

		void Accepted();
		void AcceptError( const boost::system::error_code &error );
		void ConnectError( const boost::system::error_code &error );
		void Connected();
		void Disconnected( const boost::system::error_code &error );
		void SendFailed( const boost::system::error_code &error );
		bool Receive( Packet &packet );
	};

}}
