//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace System {

/** ---------------------------------------------------------------------------
 * Wrapper for boost::asio::io_service, also manages a thread pool.
 */
class Service {

private:
	// work to keep the thread pool alive
	std::unique_ptr<boost::asio::io_service::work> m_dummy_work;

	// thread pool
	boost::thread_group m_threads;

	void StopWork();

protected:
	boost::asio::io_service m_io_service;
	 
public:
	Service();
	virtual ~Service();
	
	/** -----------------------------------------------------------------------
	 * Get the underlying asio::io_service
	 */
	boost::asio::io_service &operator ()() {
		return m_io_service;   
	}

	/** -----------------------------------------------------------------------
	 * Add threads into the thread pool.
	 *
	 * This function can be called multiple times. but
	 * threads cannot be stopped.
	 */
	void Run( int number_of_threads );
	
	/** -----------------------------------------------------------------------
	 * Add the current thread into the thread pool.
	 */
	void Join();

	/** -----------------------------------------------------------------------
	 * Finishes work and destroys threads.
	 * 
	 * Will stall if a thread is not told to exit.
	 *
	 * @param wait Join the thread pool. Set to false if this is being called
	 *             from inside a service handler.
	 */
	void Finish( bool wait );

	/** -----------------------------------------------------------------------
	 * Run a task in the thread pool.
	 *
	 * @param handler Handler to execute.
	 * @param delay   Optional delay in milliseconds to 
	 *                wait before execution.
	 */
	void Post( std::function<void()> handler, int delay = 0 ); 
	 
	static void PostDelayedHandler( 
					    const boost::system::error_code &error, 
						std::shared_ptr<boost::asio::deadline_timer> &timer,
						std::function<void()> &handler );
	 
};

}
