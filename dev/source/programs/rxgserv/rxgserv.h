
// ****************************************************************************
// RXG Services
// ****************************************************************************

#pragma once

#include "system/program.h"
#include "net/textstream.h"
#include "net/listener.h"
#include "net/events.h"

#include "procs.h"

//-----------------------------------------------------------------------------
namespace User { namespace RXGServ {

	class Stream;

	//-------------------------------------------------------------------------
	class MyProcs : public ProcHandler {
	
		Stream &m_stream;
		 
	public:
		MyProcs( Stream &stream );

		std::shared_ptr<ProcContext> CreateContext( 
				const std::string &cmd) override;
		void Unknown( ProcContext::ptr& ) override;
		void Test( ProcContext::ptr& ) override;
	};
	
	//-------------------------------------------------------------------------
	class Stream : public Net::TextStream {
		friend class NetHandler;
		friend class ProcContext;

		bool m_authed = false;

		MyProcs m_prochandler; 

		std::deque<std::string> m_procqueue;
		std::mutex m_mutex; 
		
		void NextProc(); // executed after proc
		void ExecProc( const std::string &command );

	public:
		Stream();
	
	public: // procs
		void RunProc( const std::string &command ); // executed from outside
	};

	//-------------------------------------------------------------------------
	class NetHandler : public Net::Events::Stream::Handler {
		RXGServ &m_parent;

		void Accepted( Net::StreamPtr &stream ) override;

		void AcceptError( 
				Net::StreamPtr &stream,
				const boost::system::error_code &error ) override;
			
		void Disconnected( 
				Net::StreamPtr &stream,
				const boost::system::error_code &error ) override;
			 
		void SendFailed( 
				Net::StreamPtr &stream,
				const boost::system::error_code &error ) override;
			 
		void Receive( Net::StreamPtr &stream, 
						Net::Message &msg ) override;
	public:
		NetHandler( RXGServ &parent );
	};
	
	//-------------------------------------------------------------------------
	class RXGServ : public System::Program {
		 
		NetHandler    m_netevents;
		Net::Listener m_listener;
	//	Procs         m_procs;

		std::string   m_password;

		enum {
			LISTEN_PORT = 12107
		};

		static Net::StreamPtr StreamFactory() {
			return std::make_shared<Stream>();
		}
		
	protected:
		void OnStart() override;

	public:
		RXGServ();
		~RXGServ();
	};


}}