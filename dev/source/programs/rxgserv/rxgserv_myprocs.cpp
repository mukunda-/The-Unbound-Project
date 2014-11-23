
#include "stdafx.h"

#include "rxgserv.h"
#include "responses.h"

//-----------------------------------------------------------------------------
namespace User { namespace RXGServ {

	MyProcs::MyProcs( Stream &stream ) : m_stream(stream) {}
	
	std::shared_ptr<ProcContext> MyProcs::CreateContext( 
			const std::string &cmd ) {

		std::shared_ptr<Stream> ptr = 
			std::static_pointer_cast<Stream>(m_stream.shared_from_this());

		return std::make_shared<ProcContext>( ptr, cmd );
	}

	//-------------------------------------------------------------------------
	void MyProcs::Unknown( ProcContext::ptr &c ) {

		ErrorResponse( RCodes::UNKNOWN_COMMAND ).Write( c->GetStream() );
	}

	//-------------------------------------------------------------------------
	void MyProcs::Test( ProcContext::ptr &c ) {
		
		SimpleResponse( "OKAY" ).Write( c->GetStream() );
	}
}}