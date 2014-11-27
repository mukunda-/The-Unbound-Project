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

#define CMDARGS( command, reqargs ) \
	const char *Command() const override { return command; } \
	const int RequiredArgs() const override { return reqargs; }
	
	// simple procs
	DEFPROC( Unknown,   ""          ,-1 );
	DEFPROC( Test,      "TEST"      , 0 );
	DEFPROC( Auth,      "AUTH"      , 3 );
	DEFPROC( Perks,     "PERKS"     , 2 );
	DEFPROC( Member,    "MEMBER"    , 1 );

	//-------------------------------------------------------------------------
	class Donations : public Procs::Proc {

	public:
		enum class Query {
			UNKNOWN, TOTAL, TOP, LASTTOP, RAND
		};
		static Query GetQuery( const std::string &str );
		
		void RespondTotal  ( Procs::Context::ptr &ct );
		void RespondTop    ( Procs::Context::ptr &ct );
		void RespondLastTop( Procs::Context::ptr &ct );
		void RespondRand   ( Procs::Context::ptr &ct );

	private:
		friend class DonationQuery;
		double m_total;            // cache every 15 minutes.
		double m_goal;            
		int    m_total_ctime = 0;

		double      m_top_amt;     // cache every 5 minutes.
		std::string m_top_name;    
		int         m_top_ctime = 0;

		double      m_lasttop_amt; // cache every 60 minutes.
		std::string m_lasttop_name;
		int         m_lasttop_ctime = 0;

		double      m_rand_amt;    // cache every 120 seconds.
		std::string m_rand_name;
		int         m_rand_ctime = 0;

		void Run( Procs::Context::ptr &ct ) override;
	public:
		CMDARGS( "DONATIONS", 1 ); 
		const bool Locking() const { return true; }
		
	};
}}}