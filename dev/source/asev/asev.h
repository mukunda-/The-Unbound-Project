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
		virtual void Handle( Event &e ) {}
	};

	/// -----------------------------------------------------------------------
	/// A handler listens to an event source.
	///
	class Handler : public Interface { 
		friend class Source;
		friend class Dispatcher;

		std::mutex       m_mutex;
		std::atomic_bool m_disabled;

		std::vector<Source*> m_sources;
		
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

		std::mutex m_mutex;

		// subscribed handlers
		std::vector<Handler*> m_handlers;
		 
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
		/// @param e Event to forward to all regsitered handlers.
		void Send( Event &e );
	};
}