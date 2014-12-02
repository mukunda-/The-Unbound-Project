
#include "stdafx.h"
#include "myprocs.h"

using namespace std;
 
//-----------------------------------------------------------------------------
namespace User { namespace RXGServ { namespace MyProcs {

//-----------------------------------------------------------------------------
void Test::Run( CT &c ) {
	if( c->Args().Count() == 2 ) {
		const std::string &arg = c->Args()[1];
		if( arg == "rt3" ) {
			KVResponse()
				.Put( "Foo", "test" )
				.Put( "bar", "2" )
				.Put( "baz", "three" )
				.Put( "bar", "2" )
				.Write( c );
			return;
		} else if( arg == "rt2" ) {
			(ListResponse() <<
				"testing" <<
				"testing" <<
				"1 2 3").Write( c );
			return;
		}
	}

	c->RespondSimple( "OKAY" ); 
}

}}}