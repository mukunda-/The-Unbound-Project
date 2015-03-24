//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once
 
#include "mem/arena/arena.h"
#include "util/feed.h"
#include "util/stringref.h"
#include "console/forwards.h"
#include "forwards.h"
#include "variables.h"
#include "service.h"

namespace System {
 
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
void ExecuteCommand( const Stref &command_string, 
					 bool command_only = false );

/// ---------------------------------------------------------------------------
/// Execute a script file.
///
/// @param file Path to script file, relative to game contents folder.
///
bool ExecuteScript( const Stref &file );
  
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
	void RegisterModule( Module *module );

	Service &GetService();

private: 

	std::unique_ptr<::Console::Instance> m_console;

	bool m_live; 

	int m_next_command_id = 0;
	
	Mem::Arena::Manager i_arenas;
	Service m_service;
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
	
	// mapping of command names to command instances
	std::unordered_map< std::string, Commands::Instance* > m_command_map;

	std::mutex m_mutex;

	std::condition_variable m_cvar_shutdown;
	bool m_shutdown_complete = false;

	int m_busy_modules = 0;

public: 
	// internal use by the global functions:
	Variable &CreateVariable( const Stref &name, 
							  const Stref &default_value,
							  const Stref &description, int flags );

	bool DeleteVariable( const Stref &name );
	Variable *FindVariable( const Stref &name );
	bool TryExecuteCommand( const Stref &command_string );
	void SaveCommand( CommandPtr &&cmd );
	void ExecuteCommand( const Stref &command_string, 
					     bool command_only = false ); 
	bool ExecuteScript( const Stref &file );
	
	void Start();

private:
	Commands::InstancePtr FindCommandInstance( const Stref &name );

	int AllocCommandID() { return ++m_next_command_id; }

	void OnModuleIdle( Module &module );
	void OnModuleBusy( Module &module );

	bool AnyModulesBusy();
	void ShutdownEx();
	void SystemEnd();

	friend class Module;
	friend class Commands::Instance;
	friend class Command;
};

} // namespace System
