//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "system/program.h"
#include "mem/arena/arena.h"
#include "util/feed.h"
#include "util/stringref.h"
#include "console/forwards.h"
#include "forwards.h"
#include "variables.h"

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

	void StopWork();

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
	/// @param wait Join the thread pool. Set to false if this is being called
	///             from inside a service handler.
	///
	void Finish( bool wait );

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


//-----------------------------------------------------------------------------
template<typename F, typename ... Args>
void LogEx( F output, const std::string &format, Args ... args ) {
	try {
		boost::format formatter(format); 
		Util::Feed( formatter, args... ); 
		output( formatter.str().c_str() );
	} catch( boost::io::format_error &e ) {
		output( (boost::format( "FORMAT ERROR: %s" ) % e.what())
				.str() );
	}
}

/// ---------------------------------------------------------------------------
/// Print a formatted message to the info log.
///
/// @param format boost::format format string. (printf compatible)
/// @param ...    Formatted arguments.
///
template<typename ... Args>
void Log( const std::string &format, Args ... args ) {
	LogEx( Log, format, args... );
}

/// ---------------------------------------------------------------------------
/// Print a message to the info log. May also print to the console.
///
/// @param message Message to log.
///
void Log( const std::string &message );

/// ---------------------------------------------------------------------------
/// Print a formatted message to the error log.
///
/// @param format boost::format format string. (printf compatible)
/// @param ...    Formatted arguments.
///
template<typename Arg, typename ... Args>
void LogError( const std::string &format, Arg arg1, Args ... args ) {
	LogEx( LogError, format, arg1, args... );
}

/// ---------------------------------------------------------------------------
/// Print a message to the error logs. May also print to the console.
///
/// @param message Message to log.
///
void LogError( const std::string &message );

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
/// Run a Program
///
/// This also joins the current thread into the system thread pool.
///
/// @param program Program instance. Both prototypes take ownership 
///                of the pointer.
///
//void RunProgram( std::unique_ptr<Program> &&program );
//void RunProgram( Program *program );

//template <class T, typename ... A> void RunProgram( A...args ) {
//	RunProgram( new T( args... ));
//}

void RegisterModule( std::unique_ptr<Module> &&program );
void RegisterModule( Module *program );

template <class T, typename ... A> void RegisterModule( A...args ) {
	RegisterModule( new T( args... ));
}

/// ---------------------------------------------------------------------------
/// Start clean program exit sequence.
///
void Shutdown();

/// ---------------------------------------------------------------------------
/// Execute a system command.
///
/// @param command_string Command to execute.
/// @param command_only   If true, only try to execute a command, and do not
///                       change system variables.
///
void ExecuteCommand( Util::StringRef command_string, 
					 bool command_only = false );

/// ---------------------------------------------------------------------------
/// Execute a script file.
///
/// @param file Path to script file, relative to game contents folder.
///
bool ExecuteScript( Util::StringRef file );
  
/// ---------------------------------------------------------------------------
/// Main system class
///
class Main {
	 
public:

	/// -----------------------------------------------------------------------
	/// @param threads Number of threads to start the main service with.
	///
	Main( int threads );

	~Main();
	bool Live() { return m_live; }
	void PostSystem( std::function<void()> handler, 
					 bool main = true, int delay = 0 );
	void Shutdown();

	//void RunProgram( std::unique_ptr<Program> &&program );
	void RegisterModule( std::unique_ptr<Module> &&module );

	Service &GetService();

private: 

	std::unique_ptr<::Console::Instance> m_console;

	bool m_live; 

	int m_next_command_id = 0;
	
	Mem::Arena::Manager i_arenas;
	Service  m_service;
	boost::asio::strand m_strand;

	//std::unique_ptr<Program> m_program;

	// list of modules, these are ordered by their system level
	std::vector<std::unique_ptr<Module>> m_modules;

	// module map indexed by module names
	std::unordered_map<std::string, Module*> m_module_map;

	// registered variables
	std::unordered_map<std::string, VariablePtr> m_variables;
	
	// "global" commands are commands that belong to the system 
	// and cannot be deleted.
	std::vector< CommandPtr > m_global_commands;
	
	// the command map
	std::unordered_map< std::string, Commands::Instance* > m_command_map;

public: 
	// internal use by the global functions:
	Variable &CreateVariable( const Util::StringRef &name, 
							  const Util::StringRef &default_value,
							  const Util::StringRef &description, int flags );

	bool DeleteVariable( const Util::StringRef &name );
	Variable *FindVariable( const Util::StringRef &name );
	bool TryExecuteCommand( Util::StringRef command_string );
	void SaveCommand( CommandPtr &&cmd );
	void ExecuteCommand( Util::StringRef command_string, 
					     bool command_only = false ); 
	bool ExecuteScript( Util::StringRef file );
	
	void Start();

private:
	Commands::InstancePtr FindCommandInstance( const Util::StringRef &name );

	int AllocCommandID() { return ++m_next_command_id; }

	void OnModuleIdle( Module &module );
	bool AnyModulesBusy();
	void SystemEnd();

	friend class Module;
	friend class Commands::Instance;
	friend class Command;
};

} // namespace System
