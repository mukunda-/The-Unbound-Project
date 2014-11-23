
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

		ErrorResponse( "BADINPUT", "Unknown command" ).Write( c->GetStream() );
	}

	//-------------------------------------------------------------------------
	void MyProcs::Test( ProcContext::ptr &c ) {
		if( c->Args().Count() == 2 ) {
			const std::string &arg = c->Args()[1];
			if( arg == "rt3" ) {
				KVResponse()
					.Put( "Foo", "test" )
					.Put( "bar", "2" )
					.Put( "baz", "three" )
					.Put( "bar", "2" )
					.Write( c->GetStream() );
				return;
			} else if( arg == "rt2" ) {
				(ListResponse() <<
					"testing" <<
					"testing" <<
					"1 2 3").Write( c->GetStream() );
				return;
			}
		}

		SimpleResponse( "OKAY" ).Write( c->GetStream() );
	}
}}