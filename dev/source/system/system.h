//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace System {

/// ---------------------------------------------------------------------------
/// Wrapper for boost::asio::io_service, also manages a thread pool.
///
class Service {

private:
	// work to keep the thread pool alive
	std::unique_ptr<boost::asio::io_service::work> m_dummy_work;

	// thread pool
	boost::thread_group m_threads;

protected:
	boost::asio::io_service m_io_service;
	 
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
	/// Add the current thread into the thread pool.
	///
	void Join();

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
	/// @param handler Handler to execute.
	/// @param delay   Optional delay in milliseconds to 
	///                wait before execution.
	///
	void Post( std::function<void()> handler, int delay = 0 ); 
	 
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
/// @param format printf syntax for output.
/// @param ... Formatted arguments.
///
void Log( const char *format, ... );

/// ---------------------------------------------------------------------------
/// Print a message to the error logs. May also print to the console.
///
/// @param format printf syntax for output.
/// @param ... Formatted arguments.
///
void LogError( const char *format, ... );

/// ---------------------------------------------------------------------------
/// Get the main Service instance.
///
Service &GetService();

/// ---------------------------------------------------------------------------
/// Post a task to be executed by the system.
///
/// @param handler Function to run.
/// @param main    If true, the handler will be wrapped in the system strand.
/// @param delay   If nonzero, will wait this many milliseconds before
///                executing the handler.
/// 
void Post( std::function<void()> handler, bool main = true, int delay = 0 );
 
/// ---------------------------------------------------------------------------
/// Checks the status of the system.
///
/// @returns true if the system is live and running. false if the system
///          hasn't started yet or is shutting down.
///
bool Live();
	
/// ---------------------------------------------------------------------------
/// Merge current thread with the system service.
///
void Join();
  
/// ---------------------------------------------------------------------------
/// System instance
///
class Instance : public Service {
	 
public:
	/// -----------------------------------------------------------------------
	/// @param threads Number of threads to start the main service with.
	///
	Instance( int threads ); 
	~Instance();

private: 
	bool m_live; 
	boost::asio::strand m_strand;

public: 
	bool Live() { return m_live; }
	void PostSystem( std::function<void()> handler, 
					 bool main = true, int delay = 0 );
};

} // namespace System
