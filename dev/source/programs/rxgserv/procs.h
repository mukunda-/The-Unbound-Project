
#pragma once 

#include "util/argstring.h"

//-----------------------------------------------------------------------------
namespace User { namespace RXGServ {
	
class RXGServ;
class Stream;

namespace Procs {
	
	/// -----------------------------------------------------------------------
	/// A context manages command execution for a stream.
	///
	/// It also provides a handy cleanup routine.
	///
	class Context {
		friend class Proc;
		std::shared_ptr<Stream> m_stream;
		std::unique_lock<std::mutex> m_lock;

		bool m_completed = false; // if this context is invalid.
		bool m_responded = false; // if a response was sent to the stream.
		Util::ArgString m_args;   // parsed args from command line.
		
	public:

		/// -------------------------------------------------------------------
		/// Create a context.
		///
		/// @param stream      Stream to link to.
		/// @param commandline Full command string.
		///
		Context( std::shared_ptr<Stream> &stream, 
						const std::string &commandline );
		~Context();
		
		/// -------------------------------------------------------------------
		/// Mark this context as completed and execute the next command
		/// in the queue.
		///
		void Complete();

		/// -------------------------------------------------------------------
		/// Get the command arguments.
		///
		Util::ArgString &Args() { return m_args; }

		/// -------------------------------------------------------------------
		/// Get the stream associated with this context.
		///
		Stream &GetStream() {
			return *m_stream;
		}

		/// -------------------------------------------------------------------
		/// Check the responded state.
		///
		/// @param set Set the responded flag.
		/// @returns true if this context has already sent a response.
		///
		bool Responded() const { return m_responded; }
		bool Responded( bool set ) { return m_responded = set; }

		/// -------------------------------------------------------------------
		/// Send a simple (RT1) response.
		///
		/// @param text Response text.
		///
		void RespondSimple( const std::string &text );

		/// -------------------------------------------------------------------
		/// Send an ERR response.
		///
		/// @param text Response text.
		///
		void RespondError( const std::string &code, const std::string &desc );

		// send a database error response.
		void RespondDBError();

		using ptr = std::shared_ptr<Context>;
	};

	//-------------------------------------------------------------------------
	class Proc {

		std::mutex m_mutex;

		/// -------------------------------------------------------------------
		/// Command implementation
		///
		/// @param ct The procedure context.
		///
		virtual void Run( Context::ptr &ct ) = 0;

	public:
		 
		void operator()( Context::ptr &ct );

		/// -------------------------------------------------------------------
		/// Returns the name of this command.
		///
		virtual const char *Command() const = 0;

		/// -------------------------------------------------------------------
		/// Returns the required number of arguments for this command.
		///
		virtual const int RequiredArgs() const = 0;
		
		/// -------------------------------------------------------------------
		/// Return true to not allow this procedure to run concurrently
		/// e.g. when it is stateful
		///
		virtual const bool Locking() { return false; }
	};

	//-------------------------------------------------------------------------
	class Map {
		
		std::unordered_map< std::string, std::shared_ptr<Proc> > m_map;

	public:
		
		/// -------------------------------------------------------------------
		/// Get a proc by name
		///
		/// @param name Name of proc.
		/// @returns If the entry exists, returns the proc, otherwise
		///          returns the unnamed proc.
		///
		std::shared_ptr<Proc> &Get( const std::string &name );
		void Add( std::shared_ptr<Proc> &proc );

		/// -------------------------------------------------------------------
		/// Remap one proc to another. The target must exist first.
		///
		//void Alias( std::string &from, std::string &to );

		template< class T, typename ... A >
		void Add( A ... args ) {
			std::shared_ptr<Procs::Proc> instance = 
					std::make_shared<T>( args ... );

			Add( instance );
		}

		/// -------------------------------------------------------------------
		/// Execute a context.
		///
		void Run( std::shared_ptr<Context> &ct );
	};

}}}
