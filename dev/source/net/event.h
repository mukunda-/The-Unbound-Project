//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#ifndef _MUTEX_
#  error "Missing <mutex>."
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
		virtual void DisconnectedError( 
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
	/// An interface implementation that has a mechanism for safe handling.
	///
	class Handler : public Interface {

		friend class Lock;
		std::mutex m_lock; // mutex to trap the parent object while an
							// event is being executed.
		bool m_disabled;
		
	public:
		/// -------------------------------------------------------------------
		/// Disable the event handler.
		///
		/// This should be called when you are no longer expecting events
		/// to trigger. After this is called, the callbacks will never be
		/// triggered.
		///
		/// This MUST be called before the handler is destructed. This is 
		/// a safety measure to catch errors if Disable is forgotten.
		///
		virtual void Disable();

		Handler();
		~Handler();
	};
	 
	/// -------------------------------------------------------------------
	/// Class to lock an EventHandler for sending events. 
	/// 
	/// The event handler must not be used directly to send events.
	/// Instead, use the interface returned from this class via operator()
	///
	class Lock {
	
		std::lock_guard<std::mutex> m_lock;
		Interface &m_interface;
	public:

		/// ---------------------------------------------------------------
		/// Lock an event handler.
		///
		Lock( Handler &parent );
			
		/// ---------------------------------------------------------------
		/// @returns the locked event interface.
		///
		Interface &operator ()() { return m_interface; }
	};

}}
