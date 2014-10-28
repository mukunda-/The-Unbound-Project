//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace System {

/// ---------------------------------------------------------------------------
/// Wrapper for boost::asio::io_service, also manages a thread pool.
///
class Service {

	boost::asio::io_service m_io_service;
	std::unique_ptr<boost::asio::io_service::work> m_dummy_work;
	boost::thread_group m_threads;

	
public:
	Service();
	~Service();
	
	/// -----------------------------------------------------------------------
	/// Get the underlying asio::io_service
	///
	boost::asio::io_service &operator ()() {
		return m_io_service;   
	}

	/// -----------------------------------------------------------------------
	/// Add threads into the thread pool.
	///
	/// This function can be called multiple times. but
	/// threads cannot be stopped.
	///
	void Run( int number_of_threads );
	
	/// -----------------------------------------------------------------------
	/// Shutdown system.
	///
	/// Runs io_service.stop and all threads should terminate.
	///
	/// Called by deconstructor.
	///
	void Stop();

	/// -----------------------------------------------------------------------
	/// Finishes work and destroys threads. Blocking function.
	///
	/// Will stall if a thread is not told to exit.
	///
	void Finish();

	/// -----------------------------------------------------------------------
	/// Run a task in the thread pool.
	///
	/// \param handler Handler to execute.
	///
	template <typename CompletionHandler> 
		void Post( const CompletionHandler &handler ) {
			
		m_io_service.post( handler );
	}
	
	/// -----------------------------------------------------------------------
	/// Run a task in the thread pool after a certain delay.
	///
	/// \param handler Handler to execute.
	/// \param delay Time to wait, in milliseconds.
	/// 
	void PostDelayed( std::function<void()> handler, int delay );
	static void PostDelayedHandler( 
					    const boost::system::error_code &error, 
						std::shared_ptr<boost::asio::deadline_timer> &timer,
						std::function<void()> &handler );
	 
};

/// ---------------------------------------------------------------------------
/// Calls GetService().Finish()
///
void Finish(); 

/// ---------------------------------------------------------------------------
/// Print a message to the info logs. May also print to the console.
///
/// \param format printf syntax for output.
/// \param ... Formatted arguments.
///
void Log( const char *format, ... );

/// ---------------------------------------------------------------------------
/// Print a message to the error logs. May also print to the console.
///
/// \param format printf syntax for output.
/// \param ... Formatted arguments.
///
void LogError( const char *format, ... );

/// ---------------------------------------------------------------------------
/// Get the main Service instance.
///
Service &GetService();

/// ---------------------------------------------------------------------------
/// Wrapper for GetService().Post (post a task to the main service).
///
/// \param handler Task to run.
///
template <typename CompletionHandler> 
	void Post( const CompletionHandler &handler ) {
		
	GetService().Post( handler );
}

/// ---------------------------------------------------------------------------
/// Wrapper for GetService().PostDelayed
///
static inline void PostDelayed( std::function<void()> handler, int delay ) {
		
	GetService().PostDelayed( handler, delay );
}
	
/// ---------------------------------------------------------------------------
/// System initializer
///
struct Init {
	 
	/// -----------------------------------------------------------------------
	/// \param threads Number of threads to start the main service with.
	///
	Init( int threads );

	~Init();
};

} // namespace System
