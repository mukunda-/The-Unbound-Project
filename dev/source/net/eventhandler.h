//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

template <class T>
class EventLock {
	
	std::shared_ptr<int> m_latch;
	std::mutex m_latch; 
	T *m_instance;


};

class EventLatch {

};

//-----------------------------------------------------------------------------
namespace Net {

	class EventHandler : public EventLock<EventHandler> {
		friend class EventLatch;

		//ObjectLatch<EventHandler> m_latch;
//		std::shared_ptr<EventHandler> m_this;

//		std::mutex m_latch; // mutex to trap the parent object while an
						    // event is being executed.
//		bool m_disabled;
		
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

		/// -------------------------------------------------------------------
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

		/// -------------------------------------------------------------------
		/// Disable the event trigger.
		///
		/// This is important to keep events from occuring when your object
		/// doesn't expect them. e.g. when the owner is being destructed.
		///
		void Disable();

		EventHandler();
	};

}
