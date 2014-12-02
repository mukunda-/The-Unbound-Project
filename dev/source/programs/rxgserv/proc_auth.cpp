
#include "stdafx.h"
#include "myprocs.h"
#include "rxgserv.h"

using namespace std;
 
//-----------------------------------------------------------------------------
namespace User { namespace RXGServ { namespace MyProcs {

//-----------------------------------------------------------------------------
void Auth::Run( CT &c ) {
	if( !c->GetStream().IsAuthed() ) {
		if( c->Args()[1] != "" ) {
			if( c->Args()[1] != 
					c->GetStream().GetServer().GetPassword() ) {

				c->RespondError( "DENIED", "Wrong password." );
				return;
			}
			c->GetStream().SetAuthed();
		}
	}
	c->GetStream().SetInfo( c->Args()[2], c->Args()[3] );
	c->RespondSimple( "OKAY" );
}

}}}