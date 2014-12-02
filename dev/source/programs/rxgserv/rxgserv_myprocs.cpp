
#include "stdafx.h"
#include "rxgserv.h"
#include "responses.h"
#include "util/time.h"

#include "db/core.h"
#include "db/transaction.h"
#include "db/line.h"
#include "db/statement.h"

#include <time.h>

using namespace std;
 
//-----------------------------------------------------------------------------
namespace User { namespace RXGServ { namespace MyProcs {

//-----------------------------------------------------------------------------
void Unknown::Run( CT &c ) {

	ErrorResponse( "BADINPUT", "Unknown command" ).Write( *c );
}


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

}}}
