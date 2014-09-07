//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

namespace System {

	
class Service {

	boost::asio::io_service m_io_service;
	std::unique_ptr<boost::asio::io_service::work> m_dummy_work;
	boost::thread_group m_threads;

	
public:
	Service();
	~Service();

	boost::asio::io_service &operator ()() {
		return m_io_service;   
	}

	// this function can be called multiple times to add network threads
	// threads cannot be stopped individually.
	void Run( int number_of_threads );

	// shutdown system, runs io_service.stop and all threads should terminate.
	// called by deconstructor.
	void Stop();

	// finishes work and destroys threads (blocking function)
	//
	void Finish();
};

	/*
class ThreadList {
	std::vector<std::thread> m_threads;


};*/
//void AddGlobalThread( std::thread &&thread );
void Finish();


void Log( const char *format, ... );
void LogError( const char *format, ... );

Service &GetService();

template <typename CompletionHandler> void Post( const CompletionHandler &handler ) {
	GetService()().post( handler );
}


struct Init {
	Init( int threads );
	~Init();
};

} // namespace System
