//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "basiclistener.h"
#include "events.h"
#include "types.h"

namespace Net {
	 
	class Listener : public BasicListener {

		//---------------------------------------------------------------------------
		class EventHandler : public Events::Stream::Handler {

			Listener &m_parent;

		public:
			void Accepted( StreamPtr &stream ) override;

			void AcceptError( StreamPtr &stream, 
							  const boost::system::error_code &error ) override;

			EventHandler( Listener &parent );
		};

		// factory to create a desired stream object
		StreamFactory m_factory;

		Events::Stream::Handler *m_user_handler;

		EventHandler m_accept_handler;
		StreamPtr    m_current_stream;
		std::mutex   m_mutex;
		bool         m_started = false;
		bool         m_ignore_errors = false;

		void Accept();
		void OnCompleted();
		void OnError();
		
	public:
		
		/// -------------------------------------------------------------------
		/// Construct a listener.
		///
		/// @param port    Port number to listen to.
		/// @param factory Factory function to produce a new Stream object.
		/// @param handler Event handler to associate with the stream object. 
		///                You can also hook your event handler in the 
		///                factory and/or leave this omitted.
		/// @param start   false to not start the listener until Start() is
		///                called.
		///
		Listener( unsigned short port, StreamFactory factory, 
				  Events::Stream::Handler *handler = nullptr, 
				  bool start = true );

		~Listener();

		/// -------------------------------------------------------------------
		/// Start listening for connections.
		///
		void Start();

		/// -------------------------------------------------------------------
		/// Stop listening.
		///
		void Stop();
		
		/// -------------------------------------------------------------------
		/// Set the option to ignore errors.
		///
		/// Ignore errors means that when an AcceptError occurs, it will
		/// ignore it and start listening for another connection. Otherwise
		/// it will Stop()
		///
		/// @param ignore Ignore errors option.
		///
		void SetIgnoreErrors( bool ignore );

		// The listener works like this:
		//   - Create a new stream using the factory provided.
		//   - Start listening for a connection with that stream.
		//   - The stream triggers an Accept or AcceptError events.
		//   - Repeat.
		//
		// Both the listener and the parent class that registered its event
		// handler with the listener (or manually in the factory) will
		// receive Accept and AcceptError events.
	};

}
