//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

// Asynchronous Events

#pragma once

#ifndef _MUTEX_
#  error "Missing <mutex>."
#endif

#ifndef _MEMORY_
#  error "Missing <memory>."
#endif

namespace Asev {
	
	/// -----------------------------------------------------------------------
	/// Represents an event.
	///
	class Event {
	};
	
	/// -----------------------------------------------------------------------
	/// Events are passed to interfaces.
	///
	/// Ideally an interface is extended to split the event into friendly
	/// handlers
	///
	class Interface {
	public:
		
		/// -------------------------------------------------------------------
		/// Handle an event.
		///
		virtual int Handle( Event &e ) {}
	};

	/// -----------------------------------------------------------------------
	/// A handler listens to an event source.
	///
	class Handler : public Interface { 
		
		/// -------------------------------------------------------------------
		/// A pipe is a medium between a handler and a source. It provides
		/// safe access to the handler.
		///
		class Pipe {
			friend class Lock;
			Handler    *m_handler;
			std::mutex m_mutex; 

		public:
			Pipe( Handler &parent );
			Handler &GetHandler();

			std::mutex &GetLock() { return m_mutex; }
		};
		
		/// -------------------------------------------------------------------
		/// A handler lock secures a handler pipe for thread safe access.
		///
		class Lock {
			std::lock_guard<std::mutex> m_lock;
		public:
			Lock( Pipe &pipe );
			Handler *operator()();
		};

		friend class Source;
		friend class Dispatcher;
		 
		std::shared_ptr<Pipe> pipe;
		 
	public:

		/// -------------------------------------------------------------------
		/// Disable the event handler.
		///
		/// This should be called when you are no longer expecting events
		/// to trigger. After this is called, the callbacks will not be
		/// triggered.
		///
		/// This MUST be called before the handler is destructed, since
		/// an event can occur during destruction and cause undefined
		/// behavior.
		///
		virtual void Disable();

		Handler();
		~Handler();

		typedef std::shared_ptr<Handler> ptr;

	};

	/// -----------------------------------------------------------------------
	/// A source represents an event source.
	///
	class Source {
		friend class Dispatcher;
		friend class Handler;

		std::mutex m_mutex;

		// subscribed handlers
		std::vector<std::shared_ptr<Handler::Pipe>> m_pipes;
		 
	public:
		Source();
		~Source();

		/// -------------------------------------------------------------------
		/// Add an event handler to this source.
		///
		/// @param handler Event handler instance.
		///
		void Subscribe( Handler &handler );
		
		/// -------------------------------------------------------------------
		/// Remove an event handler from this source.
		///
		/// @param handler Event handler instance.
		///
		void Unsubscribe( Handler &handler );
	};
	 
	/// -----------------------------------------------------------------------
	/// A dispatcher generates and sends events.
	///
	class Dispatcher {
		Source &m_source;
		std::lock_guard<std::mutex> m_lock; 

	public:
		/// -------------------------------------------------------------------
		/// Lock an event source to send events.
		///
		Dispatcher( Source &parent );
		
		/// -------------------------------------------------------------------
		/// Dispatch an event.
		///
		/// @param e Event to forward to all registered handlers.
		///
		void Send( Event &e );
	};
}