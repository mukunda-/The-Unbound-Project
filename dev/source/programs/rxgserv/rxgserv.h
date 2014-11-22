
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

		void Unknown( ProcContext::ptr& ) override;
		void Test( ProcContext::ptr& ) override;
	public:
		MyProcs( Stream &stream );

	};
	
	//-------------------------------------------------------------------------
	class Stream : public Net::TextStream {
		friend class NetHandler;
		friend class ProcContext;

		enum class State {
			AUTH, // waiting for authentication
			READY // active
		};
		
		State m_state = State::AUTH;

		MyProcs m_prochandler;
		ProcQueue m_procq;


		//std::deque<std::string> m_queue;
		//std::mutex m_mutex;

		//void ExecCommand( const std::string &cmd );
	public:
		Stream();
		// queue or run a command.
		//void RunCommand( const std::string &command );
		//void NextCommand();

	public: // procs
		
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