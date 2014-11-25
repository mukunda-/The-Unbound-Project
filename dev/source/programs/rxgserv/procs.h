
#pragma once 

#include "util/stringles.h"

//-----------------------------------------------------------------------------
namespace User { namespace RXGServ {
	
class RXGServ;
class Stream;

namespace Procs {
	
	//-------------------------------------------------------------------------
	class Context {
		std::shared_ptr<Stream> m_stream;
		bool m_completed = false;
		Util::ArgString m_args;
	public:

		Context( std::shared_ptr<Stream> &stream, 
						const std::string &commandline );
		~Context();

		void Complete();
		Util::ArgString &Args() { return m_args; }

		Stream &GetStream() {
			return *m_stream;
		}

		using ptr = std::shared_ptr<Context>;
	};

	//-------------------------------------------------------------------------
	class Proc {

		virtual void Run( Context::ptr &ct ) = 0;

	public:
		void operator()( Context::ptr &ct ) { Run( ct ); }
		virtual const char *Command() = 0;
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

		void Run( std::shared_ptr<Context> &ct );
	};

}}}
