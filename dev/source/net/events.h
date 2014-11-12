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
	class Remsg;
}
 
//-----------------------------------------------------------------------------
namespace Net { namespace Events { 
	
	namespace Stream { 

		typedef std::shared_ptr<Net::Stream> StreamPtr;

		//-------------------------------------------------------------------------
		class Event : public Asev::Event {

		protected:
			friend class Handler;
		
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

			Event( StreamPtr &stream, Classes event_class );

		public:
			class Accepted;
			class AcceptError;
			class ConnectError;
			class Connected;
			class Disconnected;
			class SendFailed;
			class Receive;

			const type_info &Info() override {
				return typeid( Event );
			}
		};

		//-------------------------------------------------------------------------
		class ErrorEvent : public Event {

			const boost::system::error_code &m_error;
		protected:
			ErrorEvent( StreamPtr &stream, 
						const boost::system::error_code &error, 
						Classes event_class );
		public:
			const boost::system::error_code &GetError() { 
				return m_error; 
			}
		};

		//-------------------------------------------------------------------------
		class Event::Accepted : public Event {
		public:
			Accepted( StreamPtr &stream );
		};

		//-------------------------------------------------------------------------
		class Event::AcceptError : public ErrorEvent {
		public:
			AcceptError( StreamPtr &stream, 
						 const boost::system::error_code &error );
		};

		//-------------------------------------------------------------------------
		class Event::ConnectError : public ErrorEvent {
		public:
			ConnectError( StreamPtr &stream, 
						  const boost::system::error_code &error );
		};

		//-------------------------------------------------------------------------
		class Event::Connected : public Event {
		public:
			Connected( StreamPtr &stream );
		};

		//-------------------------------------------------------------------------
		class Event::Disconnected : public ErrorEvent {
		public:
			Disconnected( StreamPtr &stream,
						  const boost::system::error_code &error );
		};

		//-------------------------------------------------------------------------
		class Event::SendFailed : public ErrorEvent {
		public:
			SendFailed( StreamPtr &stream, 
						const boost::system::error_code &error );
		};

		//-------------------------------------------------------------------------
		class Event::Receive : public Event {
			Remsg &m_msg;
		public:
			Receive( StreamPtr &stream, Remsg &msg );

			Remsg &GetMessage() { return m_msg; }
		};
		 
		/// -----------------------------------------------------------------------
		/// The interface for network events.
		///
		class Handler : public Asev::Handler {

		protected:
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
			/// Called when a stream receives a message.
			///
			/// @param stream  The stream associated with the event.
			/// @param msg     The message that was received.
			///
			virtual void Receive( 
					StreamPtr &stream,
					Remsg &msg ) { 
			 
			}
		public:
			virtual int Handle( Asev::Event &e ) override;
		};
	 
		/// -----------------------------------------------------------------------
		/// Dispatcher simplified for a stream.
		/// 
		class Dispatcher : public Asev::Dispatcher {

			StreamPtr &m_stream;
		public:

			/// -------------------------------------------------------------------
			/// Lock a stream source to dispatch events.
			///
			/// @param parent Stream source to lock.
			///
			Dispatcher( StreamPtr &parent );

			// see Handler.
			void Accepted();
			void AcceptError( const boost::system::error_code &error );
			void ConnectError( const boost::system::error_code &error );
			void Connected();
			void Disconnected( const boost::system::error_code &error );
			void SendFailed( const boost::system::error_code &error );
			void Receive( Remsg &msg );
		};
	}
}}
