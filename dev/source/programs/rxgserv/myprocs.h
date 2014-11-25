#pragma once

#include "procs.h"

namespace User { namespace RXGServ { namespace MyProcs {

#define DEFPROC( type, command )	\
	class type : public Procs::Proc {								\
		void Run( Procs::Context::ptr &ct ) override;				\
	public:															\
		const char *Command() override { return command; }	\
	}
	
	DEFPROC( Unknown, "UNKNOWN" );
	DEFPROC( Test,    "TEST"    );
	DEFPROC( Perks,   "PERKS"   );

	/*
	//-------------------------------------------------------------------------
	class Unknown : public Procs::Proc {
		void Run( Procs::Context::ptr &ct ) override;
	};

	//-------------------------------------------------------------------------
	class Test : public Procs::Proc {
		void Run( Procs::Context::ptr &ct ) override;

	public:
		const std::string &Command() override { return "TEST"; }
	};

	*/

}}}