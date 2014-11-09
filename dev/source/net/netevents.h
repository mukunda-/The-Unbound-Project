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
 
#include "packet.h"
#include "asev/asev.h"

namespace Net {

	class Stream;  
}
 
//-----------------------------------------------------------------------------
namespace Net { namespace Events { 
	
	namespace Stream { 
		typedef std::shared_ptr<Net::Stream> StreamPtr;

		class Event : public Asev::Event {

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

			StreamPtr m_stream;
			Classes m_class;

			Event( Classes event_class );

		public:
			class Accepted;
			class AcceptError;
			class ConnectError;
			class Connected;
			class Disconnected;
			class SendFailed;
			class Receive;

			type_info Info() override {
				return typeid( Event );
			}
		};

		class ErrorEvent : public Event {

			const boost::system::error_code &m_error;
		public:
			ErrorEvent( const boost::system::error_code &error );
			const boost::system::error_code &GetError() { return m_error; }
		};

		class Event::Accepted : public Event {
			Accepted();
		};

		class Event::AcceptError : public ErrorEvent {
			AcceptError();
		};

		class Event::ConnectError : public ErrorEvent {
			ConnectError();
		};

		class Event::Connected : public Event {
			Connected();
		};

		class Event::SendFailed : public ErrorEvent {
			SendFailed();
		};

		class Event::Receive : public Event {
			Receive();
		};


		/// -----------------------------------------------------------------------
		/// The interface for network events.
		///
		class Handler : public Asev::Handler {

		public:
			/// -------------------------------------------------------------------
			/// Called when a new stream is accepted/created.
			///
			/// @param stream The stream associated with the event.
			///
			virtual void Accepted( StreamPtr &stream ) {}

			/// -------------------------------------------------------------------
			/// Called from a listener when an error occurs when trying to
			/// accept a connection.
			///
			/// @param stream The stream associated with the event.
			/// @param error  The error code.
			///
			virtual void AcceptError( 
					StreamPtr &stream,
					const boost::system::error_code &error ) {}
			
			/// -------------------------------------------------------------------
			/// Called when an attempt to connect to a remote point fails.
			///
			/// @param stream The stream associated with the event.
			/// @param error  The error code.
			///
			virtual void ConnectError( 
					StreamPtr &stream,
					const boost::system::error_code &error ) {}

			/// -------------------------------------------------------------------
			/// Called when a remote connection is established.
			///
			/// @param stream The stream associated with the event.
			///
			virtual void Connected( StreamPtr &stream ) {}

			/// -------------------------------------------------------------------
			/// Called when a stream is closed.
			///
			/// @param stream The stream associated with the event.
			/// @param error  The error code, to see if the disconnection
			///               was a problem or a normal socket closure.
			///
			virtual void Disconnected( 
					StreamPtr &stream,
					const boost::system::error_code &error ) {}

			/// -------------------------------------------------------------------
			/// Called when a connection fails during an outgoing operation.
			///
			/// @param stream The stream associated with the event.
			/// @param error  The error code.
			///
			virtual void SendFailed( 
					StreamPtr &stream,
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
					StreamPtr &stream,
					Packet &packet ) { 
			
				return false; 
			}
		};
	 
		/// -----------------------------------------------------------------------
		/// Dispatcher simplified for a stream.
		/// 
		class Dispatcher : public Asev::Dispatcher {

			StreamPtr &m_stream;
		public:
			Dispatcher( StreamPtr &parent );

			void Accepted();
			void AcceptError( const boost::system::error_code &error );
			void ConnectError( const boost::system::error_code &error );
			void Connected();
			void Disconnected( const boost::system::error_code &error );
			void SendFailed( const boost::system::error_code &error );
			bool Receive( Packet &packet );
		};
	}
}}
