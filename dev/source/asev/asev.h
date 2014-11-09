//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

// Asynchronous Events

#pragma once 

#if !defined(_MUTEX_) || !defined(_MEMORY_) || !defined(_TYPEINFO_)
#  error "Requires <mutex>, <memory>, <typeinfo>"
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
	};
	
	/// -----------------------------------------------------------------------
	/// Events are passed to interfaces. 
	///
	class Interface {

	protected:
		
		/// -------------------------------------------------------------------
		/// Handle an event.
		///
		virtual int Handle( Event &e ) { return 0; }
	};

	/// -----------------------------------------------------------------------
	/// A handler implements the base interface and listens to event sources.
	///
	class Handler : public Interface { 
		
		/// -------------------------------------------------------------------
		/// A pipe is a medium between a handler and a source. It provides
		/// safe access to the handler.
		///
		class Pipe {
			friend class Lock;
			friend class Handler;
			Handler    *m_handler;
			std::mutex m_mutex; 

		public:
			Pipe( Handler &parent );
			std::mutex &GetLock() { return m_mutex; }
		};
		
		/// -------------------------------------------------------------------
		/// A handler Lock secures a handler pipe for thread safe access.
		///
		class Lock {
			std::lock_guard<std::mutex> m_lock;
			Pipe &m_pipe;
		public:
			Lock( Pipe &pipe );

			/// ---------------------------------------------------------------
			/// @returns The handler for the locked pipe, or nullptr if the
			///          pipe is closed.
			///
			Handler *operator()();

			/// ---------------------------------------------------------------
			/// Close the pipe.
			///
			void Close();
			
			/// ---------------------------------------------------------------
			/// Check if the pipe is closed.
			///
			/// @returns true if the locked pipe is closed, i.e. it doesn't
			///          have a live handler pointer.
			///
			bool IsClosed();
		};

		friend class Source;
		friend class Dispatcher;
		 
		std::shared_ptr<Pipe> m_pipe;
		 
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

		/// -------------------------------------------------------------------
		/// Add an event handler to this source.
		///
		/// @param handler Event handler instance.
		///
		virtual void AsevSubscribe( Handler &handler );
		
		/// -------------------------------------------------------------------
		/// Remove an event handler from this source.
		///
		/// @param handler Event handler instance.
		///
		virtual void AsevUnsubscribe( Handler &handler );
	};
	 
	/// -----------------------------------------------------------------------
	/// A dispatcher generates and sends events.
	///
	class Dispatcher {
		Source &m_source;
		std::lock_guard<std::mutex> m_lock; 

	public:
		/// -------------------------------------------------------------------
		/// Capture an event source to send events.
		///
		Dispatcher( Source &parent );
		
		/// -------------------------------------------------------------------
		/// Dispatch an event.
		///
		/// @param e Event to forward to all registered handlers.
		///
		virtual void Send( Event &e );
	};
}
