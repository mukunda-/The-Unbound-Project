#pragma once

#include "procs.h"

namespace User { namespace RXGServ { namespace MyProcs {

#define DEFPROC( type, command, reqargs )	\
	class type : public Procs::Proc {								\
		void Run( Procs::Context::ptr &ct ) override;				\
	public:															\
		const char *Command() const override { return command; }	\
		const int RequiredArgs() const override { return reqargs; } \
	}
	
	DEFPROC( Unknown, ""       ,-1 );
	DEFPROC( Test,    "TEST"   , 0 );
	DEFPROC( Perks,   "PERKS"  , 2 );
	DEFPROC( Member,  "MEMBER" , 1 );

}}}