//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "basiclistener.h"
#include "events.h"

namespace Net {

	class Stream;
	typedef std::shared_ptr<Stream> StreamPtr;
	 
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
		std::function<StreamPtr()> m_factory;

		Events::Stream::Handler *m_user_handler;
		EventHandler m_accept_handler;
		StreamPtr m_current_stream;
		std::mutex m_mutex;
		bool m_started;

		void Accept();
		void OnCompleted();
		
	public:
		
		/// -------------------------------------------------------------------
		/// Construct a listener.
		///
		/// @param port    Port number to listen to.
		/// @param factory Factory function to produce a new Stream object.
		/// @param handler Event handler to associate with the stream object. 
		///                You can also hook your event handler in the 
		///                factory and/or leave this omitted.
		Listener( unsigned short port, std::function<StreamPtr()> factory, 
				  Events::Stream::Handler *handler = nullptr );
		~Listener();

		/// -------------------------------------------------------------------
		/// Start listening for connections.
		///
		void Start();

		/// -------------------------------------------------------------------
		/// Stop listening.
		///
		void Stop();

		// The listener works like this:
		//   - Create a new stream using the factory provided.
		//   - Start listening for a connection with that stream.
		//   - The stream triggers an Accept or AcceptError events.
		//   - Repeat.
	};

}
