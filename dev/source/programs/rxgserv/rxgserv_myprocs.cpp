
#include "stdafx.h"
#include "rxgserv.h"
#include "responses.h"
 
//-----------------------------------------------------------------------------
namespace User { namespace RXGServ { namespace MyProcs {
	using CT = Procs::Context::ptr;

	//-------------------------------------------------------------------------
	void Unknown::Run( CT &c ) {

		ErrorResponse( "BADINPUT", "Unknown command" ).Write( c->GetStream() );
	}

	//-------------------------------------------------------------------------
	void Test::Run( CT &c ) {
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

	//-------------------------------------------------------------------------
	void Perks::Run( CT &c ) {

	}

}}}