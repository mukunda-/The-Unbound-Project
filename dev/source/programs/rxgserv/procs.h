
#pragma once 

#include "util/stringles.h"

//-----------------------------------------------------------------------------
namespace User { namespace RXGServ {
	
	class RXGServ;
	class ProcHandler;
	class Stream;

	//-------------------------------------------------------------------------
	class ProcContext {
		std::shared_ptr<Stream> m_stream;

		bool m_completed = false;
		Util::ArgString m_args;
	public:
		ProcContext( std::shared_ptr<Stream> &stream, 
					 const std::string &command );
		~ProcContext();

		void Complete();
		Util::ArgString &Args() { return m_args; }

		using ptr = std::shared_ptr<ProcContext>;
	};

	//-------------------------------------------------------------------------
	class ProcHandler {

	public:
		void Run( ProcContext::ptr &context );

		virtual void Unknown( ProcContext::ptr& ) = 0;
		virtual void Test( ProcContext::ptr& ) = 0;
	};

	//-------------------------------------------------------------------------
	class ProcQueue {
	
		std::deque<std::string> m_queue;
		std::mutex m_mutex;
		ProcHandler &m_handler;

	public:
		ProcQueue( ProcHandler &handler );
		void Run( const std::string &command ); // executed from outside
		void Next(); // executed after proc

	private:
		void Exec( const std::string &command );
	};

}}
