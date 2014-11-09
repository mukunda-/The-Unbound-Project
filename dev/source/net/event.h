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
	/// A source generates events to be passed to handlers.
	///
	class Source {

		std::mutex m_mutex;

		// subscribed handlers
		std::vector<Handler> m_handlers;
		 
	public:
		Source();
		~Source();

		/// -------------------------------------------------------------------
		/// Add an event handler to this source.
		///
		/// @param handler Event handler instance.
		/// @param check_for_duplicates If true, this function will check
		///        for a duplicate of the handler first and exit if it's found.
		///        Otherwise, the handler may be added multiple times and will
		///        be called multiple times when an event dispatches.
		///
		void RegisterEventHandler( Handler &handler, 
								   bool check_for_duplicates = false );

		/// -------------------------------------------------------------------
		/// Remove an event handler from this source.
		///
		/// @param handler              Event handler instance.
		/// @param check_for_duplicates If false, assume there is only up to
		///        one copy of the handler in the list, i.e. only remove one
		///        copy of the handler.
		///
		void UnregisterEventHandler( Handler &handler, 
									 bool check_for_duplicates = false );
	};
	
	/// -----------------------------------------------------------------------
	/// A handler listens to an event source.
	///
	class Handler : public Interface { 
		friend class Dispatcher;

		std::mutex m_mutex; // lock for dispatching events, the source's
							// mutex is used for sub/unsub
		bool m_disabled;
		
	public:
		/// -------------------------------------------------------------------
		/// Subscribe to an event source.
		///
		/// @param source Source to listen to.
		///
		virtual void Subscribe( Source &source );

		/// -------------------------------------------------------------------
		/// Unsubscribe from an event source.
		/// 
		/// @param source Source to stop listening to.
		///
		virtual void Unsubscribe( Source &source );

		/// -------------------------------------------------------------------
		/// Disable the event handler.
		///
		/// This should be called when you are no longer expecting events
		/// to trigger. After this is called, the callbacks will not be
		/// triggered.
		///
		/// This MUST be called before the handler is destructed. This is 
		/// a safety measure to catch cases where Disable is not used.
		///
		virtual void Disable();

		Handler();
		~Handler();

		typedef std::shared_ptr<Handler> ptr;
	};
	 
	/// -----------------------------------------------------------------------
	/// Class to lock an EventHandler for sending events. 
	/// 
	/// The event handler must not be used directly to send events.
	/// Instead, use the interface returned from this class via operator()
	///
	class Dispatcher : public Interface {
	
		std::lock_guard<std::mutex> m_lock;
		Interface &m_interface;
	public:

		/// -------------------------------------------------------------------
		/// Lock an event handler.
		///
		Lock( Handler &parent );
		Lock( Handler::ptr &parent );
			
		/// -------------------------------------------------------------------
		/// @returns the locked event interface.
		///
		Interface &operator ()() { return m_interface; }
		Interface *operator ->() { return &m_interface; }
	};

}}
