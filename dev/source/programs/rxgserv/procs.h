
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

		Stream &GetStream() {
			return *m_stream;
		}

		using ptr = std::shared_ptr<ProcContext>;
	};

	//-------------------------------------------------------------------------
	class ProcHandler {

	public:
		void Run( ProcContext::ptr &context );
		virtual std::shared_ptr<ProcContext> CreateContext( 
				const std::string &cmd ) = 0;

		virtual void Unknown( ProcContext::ptr& ) = 0;
		virtual void Test( ProcContext::ptr& ) = 0;
	};


}}
