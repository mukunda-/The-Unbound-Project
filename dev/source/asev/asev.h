//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

// Asynchronous Events

#pragma once 

#if !defined(_MUTEX_) || !defined(_MEMORY_) || !defined(_TYPEINFO_)
#  error "Requires <mutex>, <memory>, <typeinfo>."
#endif

namespace Asev {
	
	/// -----------------------------------------------------------------------
	/// Represents an event.
	///
	class Event { 
		
	public:
		virtual const type_info &Info() { 
			return typeid( Event ); 
		}

		virtual ~Event() {}
	}; 

	/// -----------------------------------------------------------------------
	/// A handler listens to event sources.
	///
	class Handler { 

		friend class Source;
		friend class Dispatcher;
		
		std::recursive_mutex m_mutex;
		int m_sourcecount = 0;
		std::condition_variable_any m_on_unsub;

		void IncrementSources();
		void DecrementSources();

		//---------------------------------------------------------------------
	public: 
		Handler();
		virtual ~Handler();
		
		/// -------------------------------------------------------------------
		/// Handle an event.
		///
		virtual int Handle( Event &e ) = 0;

		// TODO: function that waits until all of the handler's sources
		// are destroyed

		//---------------------------------------------------------------------
		std::recursive_mutex &GetMutex() { return m_mutex; }
		 
		//---------------------------------------------------------------------
		typedef std::shared_ptr<Handler> ptr; 
	};

	/// -----------------------------------------------------------------------
	/// A source represents an event source.
	///
	class Source {
		friend class Dispatcher;
		friend class Handler;

		//---------------------------------------------------------------------
		// this container auto increments/decrements 
		// the handler's source counter
		class HandlerRef final {
			using T = std::shared_ptr<Handler>;
			T m_ptr;
			
		public:
			T& operator*() { return m_ptr; }
			
			HandlerRef( T &ptr );
			HandlerRef( HandlerRef &&other ) NOEXCEPT;
			HandlerRef& operator=( HandlerRef &&other ) NOEXCEPT;
			~HandlerRef();

			// we dont need to copy this, so make sure we arent
			HandlerRef( HandlerRef &other ) = delete;
			HandlerRef& operator=( HandlerRef &other ) = delete;
		};

		//---------------------------------------------------------------------
		std::recursive_mutex m_mutex;

		// if we are inside a dispatcher.
		bool m_handler_is_executing = false;
		
		std::vector<HandlerRef> m_handlers;     // subscribed handlers
		std::vector<HandlerRef> m_newhandlers;  // handlers to be added
		std::vector<Handler*> m_removehandlers; // handlers to be removed

		bool m_disabled = false;

		void ModifyPipes();
		int  SendEvent( Event &e );
		
		//---------------------------------------------------------------------
	public: 
		Source();
		virtual ~Source();

		/// -------------------------------------------------------------------
		/// Add an event handler to this source.
		///
		/// @param handler Event handler pointer. If the pointer is null, this
		///                function will do nothing
		///
		virtual void AsevSubscribe( Handler::ptr &handler );
		
		/// -------------------------------------------------------------------
		/// Remove an event handler from this source.
		///
		/// @param handler Event handler pointer. If null, this function will
		///                do nothing.
		///
		virtual void AsevUnsubscribe( Handler::ptr &handler );
		virtual void AsevUnsubscribe( Handler &handler ); // so much
		virtual void AsevUnsubscribe( Handler *handler ); // convenience..

		/// -------------------------------------------------------------------
		/// Stop any further events from being dispatched.
		///
		virtual void AsevDisable();

		/// -------------------------------------------------------------------
		/// Allow events to be dispatched again.
		///
		virtual void AsevEnable();
	};
	 
	/// -----------------------------------------------------------------------
	/// A dispatcher generates and sends events.
	///
	class Dispatcher {
		Source &m_source;
		
		// dispatcher has a lock on a source when it is constructed.
		std::lock_guard<std::recursive_mutex> m_lock; 

	public:
		/// -------------------------------------------------------------------
		/// Capture an event source to send events.
		///
		Dispatcher( Source &parent );
		
		/// -------------------------------------------------------------------
		/// Dispatch an event.
		///
		/// @param e Event to forward to all registered handlers.
		/// @returns Return value of the last handler executed.
		///
		virtual int Send( Event &e );
	};
}
