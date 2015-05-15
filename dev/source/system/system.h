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
void LogEx( F output, const Stref &format, Args ... args ) {
	try {
		boost::format formatter( format ); 
		Util::Feed( formatter, args... ); 
		output( formatter.str().c_str() );
	} catch( boost::io::format_error &e ) {
		output( (boost::format( "FORMAT ERROR: %s" ) % e.what())
				.str() );
	}
}

/** ---------------------------------------------------------------------------
 * Print a formatted message to the info log.
 *
 * @param format boost::format format string. (printf compatible)
 * @param ...    Formatted arguments.
 */
template<typename ... Args>
void Log( const Stref &format, Args ... args ) {
	LogEx( Log, format, args... );
}

/** ---------------------------------------------------------------------------
 * Print a message to the info log. May also print to the console.
 *
 * @param message Message to log.
 */
void Log( const Stref &message );

/** ---------------------------------------------------------------------------
 * Print a formatted message to the error log.
 *
 * @param format boost::format format string. (printf compatible)
 * @param ...    Formatted arguments.
 */
template<typename Arg, typename ... Args>
void LogError( const Stref &format, Arg arg1, Args ... args ) {
	LogEx( LogError, format, arg1, args... );
}

/** ---------------------------------------------------------------------------
 * Print a message to the error logs. May also print to the console.
 *
 * @param message Message to log.
 */
void LogError( const Stref &message );

/** ---------------------------------------------------------------------------
 * Get the work Service instance.
 */
Service &GetService();

/** ---------------------------------------------------------------------------
 * Post a task to be executed by the system.
 *
 * @param handler Function to run.
 * @param main    If true, the handler will be wrapped in the system strand.
 * @param delay   If nonzero, will wait this many milliseconds before
 *                executing the handler.
 */
void Post( std::function<void()> handler, bool main = true, int delay = 0 );

/** ---------------------------------------------------------------------------
 * Checks the status of the system.
 *
 * @returns true if the system is live and running. false if the system
 *          hasn't started yet or is shutting down.
 */
bool Live();
 
/** ---------------------------------------------------------------------------
 * Register a system module.
 *
 * System modules work together to implement program behavior.
 */
void RegisterModule( std::unique_ptr<Module> &&program );
void RegisterModule( Module *program );

template <class T, typename ... A> void RegisterModule( A...args ) {
	RegisterModule( new T( args... ));
}

/** ---------------------------------------------------------------------------
 * Behavior of the main thread on Start()
 */
enum class StartMode {

	/** -----------------------------------------------------------------------
	 * Merge as a dedicated thread. Main-posts will execute on this thread
	 * only, and it will not share time with the worker threads. This is
	 * to keep compatibility with APIs that require certain functions to
	 * be executed on one thread only.
	 */
	DEDICATED_MAIN,

	/** -----------------------------------------------------------------------
	 * Merge as a normal work thread. Main-posts will run in a dedicated
	 * strand, but may execute on any of the worker threads.
	 */
	JOIN_WORK,

	/** -----------------------------------------------------------------------
	 * The main thread will not join with the system, and Start will return
	 * immediately allowing it to execute additional code from the outside.
	 */
	PASS

	// a Main-post is a task that is scheduled via Post with main=true
};

/** ---------------------------------------------------------------------------
 * Start the system.
 *
 * This is called after all desired modules are registered. Must only be
 * called by main thread.
 *
 * If using the DEDICATED_MAIN or JOIN_WORK start modes, this function
 * will block until the system shuts down. 
 *
 * If using PASS, this will return immediately and allow you to execute 
 * additional code from the outside. Care should be taken as that is not 
 * normal behavior (but may be used for testing etc.)
 */
void Start();

/** ---------------------------------------------------------------------------
 * Notifies all modules of a shutdown, and enters shutdown mode.
 *
 * Basically starts a clean program exit sequence.
 *
 * @param reason Reason the shutdown was requested.
 */
void Shutdown( const Stref &reason );

/** ---------------------------------------------------------------------------
 * Execute a system command.
 *
 * @param command_string Command to execute.
 * @param command_only   If true, only try to execute a command, and do not
 *                       change system variables.
 */
void ExecuteCommand( const Stref &command_string, 
					 bool command_only = false );

/** ---------------------------------------------------------------------------
 * Execute a script file.
 *
 * @param file Path to script file, relative to game contents folder.
 * @returns false if the file is not found.
 */
bool ExecuteScript( const Stref &file );

/** ---------------------------------------------------------------------------
 * Register an event type.
 *
 * Event types must be registered before being dispatched in the system.
 */
template< typename T > void RegisterEvent() {
	RegisterEvent( T::Name(), 
}
void RegisterEvent( const Stref &name, int &result );
  
/** ---------------------------------------------------------------------------
 * Main system class.
 */
class Main final {
	 
public:

	/** -----------------------------------------------------------------------
	 * Construct the Main instance.
	 *
	 * @param threads    Number of threads to add to the system service. 
	 *                   This arguement may be 0 if you plan on merging
	 *                   the main thread as a work thread in System::Start
	 * @param start_mode How Start() works, see StartModes.
	 */
	Main( int threads, StartMode start_mode = StartMode::DEDICATED_MAIN );
	~Main();

private:

	std::unique_ptr<::Console::Instance> m_console;

	bool m_live; 

	int m_next_command_id = 0;
	
	Mem::Arena::Manager i_arenas;
	Service m_service;

	// main-thread dedicated service, used in DEDICATED_MAIN startup mode,
	// not used otherwise.
	Service m_service_main;

	// startup mode, set by constructor
	StartMode m_start_mode;

	bool m_started; // has Start been called.

	// using a strand for the main thread or dedicated service
	bool m_using_strand;

	boost::asio::strand m_strand;
	
	// list of modules, these are ordered by their system level
	std::vector<std::unique_ptr<Module>> m_modules;

	// module map indexed by module names
	std::unordered_map<std::string, Module*> m_module_map;

	// registered system variables
	std::unordered_map<std::string, VariablePtr> m_variables;
	
	// "global" commands are commands that belong to the system 
	// and cannot be deleted.
	std::vector< CommandPtr > m_global_commands;
	
	// mapping of command names to command instances
	std::unordered_map< std::string, Commands::Instance* > m_command_map;
	
	std::mutex m_mutex;

	// cvar and variable for blocking the destructor until shutdown completes.
	std::condition_variable m_cvar_shutdown;
	bool m_shutdown_complete = false;

	// number of modules that are busy, updated with Module::SetBusy
	// when shutdown is active, the system waits for this to become zero
	// before terminating the program.
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
	void StartI();
	bool Live() { return m_live; }
	void Post( std::function<void()> handler, bool main, int delay );
	void Shutdown( const Stref &reason );
	
	void RegisterModule( Module *module );

	Service &GetService() { return m_service; }
	void Log( const Stref &message );
	void LogError( const Stref &message );

	void RegisterEvent( const Stref &name, int &result );

private:
	Commands::InstancePtr FindCommandInstance( const Stref &name );

	int AllocCommandID() { return ++m_next_command_id; }

	void OnModuleIdle( Module &module );
	void OnModuleBusy( Module &module );

	bool AnyModulesBusy();
	void ShutdownI( const Stref &reason );
	void SystemEnd();

	friend class Module;
	friend class Commands::Instance;
	friend class Command;
};

} // namespace System
