#include "stdafx.h"
#include "system/system.h"
#include "net/core.h"
#include "system/server/serverconsole.h"
#include "programs/rxgserv/rxgserv.h"

namespace User {

	//-------------------------------------------------------------------------
	void RunProgram() {
		System::RunProgram( User::RXGServ::RXGServ() );
	 
		//builder.AddEscapedString( "hi" );
		//builder % "hi";
	}

	//-------------------------------------------------------------------------
	void Main( int argc, char *argv[] ) { 
		System::Instance i_system(2); 
		System::ServerConsole::Instance i_serverconsole( "RXG SERVICES" );
		Net::Instance i_net(1);
	//	DB::Manager i_db(1);
		{
		
			RunProgram(); 
		} 
	
	}
}

//-----------------------------------------------------------------------------
int main( int argc, char *argv[] ) {
	User::Main( argc, argv );
	return 0;
}
