
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

using CT = Procs::Context::ptr;

//-----------------------------------------------------------------------------
class ContextTransaction : public DB::Transaction {

public:
	ContextTransaction( CT &ct ) : m_ct(ct) {}

protected:
	virtual void OnSuccess( DB::TransactionPtr ptr ) = 0;

	CT m_ct;

private:
		
	void Completed( DB::TransactionPtr ptr, bool failed ) override {
		if( failed ) {
			m_ct->RespondDBError();
		} else {
			OnSuccess( std::move(ptr) );
		}
	}

};

//-----------------------------------------------------------------------------
void Unknown::Run( CT &c ) {

	ErrorResponse( "BADINPUT", "Unknown command" ).Write( *c );
}

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

//-----------------------------------------------------------------------------
class PerksTransaction : public ContextTransaction {

public:
	enum class Type {
		STEAM,
		FORUM
	};

	/// -----------------------------------------------------------------------
	/// @param ct   Procedure context
	/// @param type Type of query
	/// @param id   Steam Account ID or forum user id.
	///
	PerksTransaction( CT &ct, Type type, long long id ) : 
			ContextTransaction(ct), m_type(type), m_id(id)
	{}

	static DB::TransactionPtr Create( CT &ct, Type type, long long id ) {

		return DB::TransactionPtr( new PerksTransaction( ct, type, id ));
	}
		
	//-------------------------------------------------------------------------
private:
	PostAction Actions( DB::Line &line ) {

		auto statement = line.CreateStatement(); 
		unique_ptr<sql::ResultSet> result;

		if( m_type == Type::STEAM ) {
			result = statement->ExecuteQuery( 
				"SELECT expires1, expires5 "
				"FROM SteamDonationCache "
				"WHERE steamid=%d ", m_id );

		} else if( m_type == Type::FORUM ) {
			result = statement->ExecuteQuery(
				"SELECT expires1, expires5 "
				"FROM UserDonationCache "
				"WHERE userid=%d ", m_id );
		}
		
		if( result->next() ) {

			m_expires1 = result->getInt( 1 );
			m_expires5 = result->getInt( 2 );
		}
				

		return NOP;
	}

	//-------------------------------------------------------------------------
	void OnSuccess( DB::TransactionPtr ptr ) override {
			
		KVResponse()
			.Put( "expires1", m_expires1 )
			.Put( "expires5", m_expires5 )
			.Write( m_ct );
	}
		
	Type m_type;
	long long m_id;

	int m_expires1 = 0;
	int m_expires5 = 0;
};
	  
//-----------------------------------------------------------------------------
void Perks::Run( CT &c ) {
	DB::Connection &connection = DB::Get( "FORUMS" );
	if( boost::iequals( c->Args()[1], "steam" ) ) {
		SteamID steamid( c->Args()[2] );
		if( !steamid ) return;

		// steamid lookup
		connection.Execute( PerksTransaction::Create( 
					c, PerksTransaction::Type::STEAM, 
					steamid.To64() ));

	} else if( boost::iequals( c->Args()[1], "forum" ) ) {
		if( !Util::IsDigits( c->Args()[2] ) ) return;

		// forumid lookup

		connection.Execute( PerksTransaction::Create( 
					c, PerksTransaction::Type::FORUM, 
					std::stoi( c->Args()[2] )));
	}
}

//-----------------------------------------------------------------------------
class MemberTransaction : public ContextTransaction {

public:
	//-------------------------------------------------------------------------
	MemberTransaction( CT &ct, SteamID &id ) : 
			ContextTransaction(ct), m_id(id) 
	{}
		
	//-------------------------------------------------------------------------
	static DB::TransactionPtr Create( CT &ct, SteamID &id ) {
		return DB::TransactionPtr( new MemberTransaction( ct, id ));
	}

private:
	//-------------------------------------------------------------------------
	PostAction Actions( DB::Line &line ) {
			
		auto statement = line.CreateStatement(); 
		auto result = statement->ExecuteQuery( 
			"SELECT usergroup.title AS rank, "
				    "usergroup.usergroupid AS groupid "
			"FROM usergroup "
			"LEFT JOIN steamuser ON steamuser.steamid=%d "
			"LEFT JOIN user ON steamuser.userid = user.userid "
			"WHERE usergroup.usergroupid = user.usergroupid ",
			m_id.To64() );
			
		if( result->next() ) {
			m_rank    = result->getString( 1 );
			m_groupid = result->getInt( 2 );
		}

		return NOP;
	}
		
	//-------------------------------------------------------------------------
	void OnSuccess( DB::TransactionPtr ptr ) override {

		bool member = false;
		if( m_rank == "Director" || m_rank == "Cabinet" 
			|| m_rank == "Captain" || m_rank == "Advisor" 
			|| m_rank == "Full Admin" || m_rank == "Basic Admin" 
			|| m_rank == "Member" ) {

			member = true;
		}
			
		KVResponse()
			.Put( "ismember", member ? "YES" : "NO" )
			.Put( "rank", m_rank )
			.Write( m_ct );
	}
		
	//-------------------------------------------------------------------------
	SteamID m_id;
	std::string m_rank;
	int m_groupid = 0;
};

//-----------------------------------------------------------------------------
void Member::Run( CT &c ) {
		
	DB::Connection &connection = DB::Get( "FORUMS" );
	SteamID steamid( c->Args()[1], SteamID::Formats::AUTO , true );
	if( !steamid ) return;

	connection.Execute( MemberTransaction::Create( c, steamid ));
}

//-----------------------------------------------------------------------------
class DonationQuery : public ContextTransaction {

public: //---------------------------------------------------------------------
	DonationQuery( CT &ct, Donations &proc, Donations::Query query ) : 
			ContextTransaction(ct), m_proc(proc), m_query(query) {

	}

	//-------------------------------------------------------------------------
	static DB::TransactionPtr Create( CT &ct, Donations &proc, 
										Donations::Query query ) {
		return DB::TransactionPtr( new DonationQuery( ct, proc, query ));
	}

private: //--------------------------------------------------------------------
	void CopyPersonResult( string &destname, double &destamount, int &desttime,
						   unique_ptr< sql::ResultSet > &result ) {

		if( result->next() ) {
			destname   = result->getString( 1 );
			destamount = result->getDouble( 2 );
		} else {
			destname   = "";
			destamount = 0.0;
		}
		desttime = Util::GetTime();
	}

	//-------------------------------------------------------------------------
	PostAction Actions( DB::Line &line ) {

		// get times.
		time_t rawtime;
		time( &rawtime );
		struct tm *timeinfo = localtime( &rawtime );
		timeinfo->tm_sec = 0;
		timeinfo->tm_min = 0;
		timeinfo->tm_hour = 0;
		timeinfo->tm_mday = 1;

		int month = timeinfo->tm_mon + 1;

		int month_start = (int)mktime( timeinfo );
		timeinfo->tm_mon++;
		int month_end = (int)mktime( timeinfo );
		timeinfo->tm_mon -= 2;
		int last_month_start = (int)mktime( timeinfo );

		auto statement = line.CreateStatement();

		// query template: start_time, end_time, order by
		static const std::string topquery(
				"SELECT custom AS name, "
				"       SUM(mc_gross*exchange_rate) AS amount "
				"FROM dopro_donations WHERE (payment_date >= '%d') "
				"AND (payment_date < '%d') AND event_id = '0' "
				"AND (payment_status = 'Completed' OR "
				"     payment_status = 'Refunded') AND "
				"option_seleczion1='Yes' GROUP BY name "
				"ORDER BY %s LIMIT 1" );
 
		//----------------------------------------------------------------
		switch( m_query ) {
		case Donations::Query::TOTAL: {
			auto result = statement->Execute( 
				"SELECT SUM(mc_gross*exchange_rate) AS total "
				"FROM dopro_donations "
				"WHERE (payment_date >= '%d') AND (payment_date < '%d') "
				"AND event_id = 0 "
				"AND (payment_status = 'Completed' OR "
					" payment_status = 'Refunded'); "
						
				"SELECT value FROM dopro_targets "
				"WHERE name='goal' AND subtype=%d; ",

					month_start, // payment date >= 
					month_end,   // payment date < 
					month );     // goal subtype
			
			if( result->next() ) {
				m_proc.m_total = result->getDouble( 1 );
				while( result->next() );
			} else {
				m_proc.m_total = 0.0;
			}

			result = statement->GetNextResultSet();
			if( result->next() ) {
				m_proc.m_goal = result->getDouble( 1 );
				while( result->next() );
			} else {
				m_proc.m_goal = 0.0;
			}

			m_proc.m_total_ctime = Util::GetTime();
			break;
		//----------------------------------------------------------------
		} case Donations::Query::TOP: {
			auto result = statement->ExecuteQuery( 
					topquery, month_start, month_end,
					DB::RawString( "amount DESC" ));

			CopyPersonResult( m_proc.m_top_name, 
							  m_proc.m_top_amt, 
							  m_proc.m_top_ctime, result );
			break;
		//----------------------------------------------------------------
		} case Donations::Query::LASTTOP: {
			auto result = statement->ExecuteQuery( 
					topquery, last_month_start, month_start, 
					DB::RawString( "amount DESC" ));

			CopyPersonResult( m_proc.m_lasttop_name, 
							  m_proc.m_lasttop_amt,
							  m_proc.m_lasttop_ctime, result );
			break;
		//----------------------------------------------------------------
		} case Donations::Query::RAND: {
			auto result = statement->ExecuteQuery(
					topquery, month_start, month_end,
					DB::RawString( "RAND()" ));
			
			CopyPersonResult( m_proc.m_rand_name, m_proc.m_rand_amt, 
							  m_proc.m_rand_ctime, result );
			break;
		}}

		return NOP;
	}
		
	//-------------------------------------------------------------------------
	void OnSuccess( DB::TransactionPtr ptr ) override {
		switch( m_query ) {
		case Donations::Query::TOTAL:
			m_proc.RespondTotal( m_ct );
			break;
		case Donations::Query::TOP:
			m_proc.RespondTop( m_ct );
			break;
		case Donations::Query::LASTTOP:
			m_proc.RespondLastTop( m_ct );
			break;
		case Donations::Query::RAND:
			m_proc.RespondRand( m_ct );
			break;
		}
	}

	//-------------------------------------------------------------------------
	Donations &m_proc;
	Donations::Query m_query;
};

//-----------------------------------------------------------------------------
auto Donations::GetQuery( const string &str ) -> Query {
	using boost::iequals;

	if( iequals( str, "total"   )) return Query::TOTAL;
	if( iequals( str, "top"     )) return Query::TOP;
	if( iequals( str, "lasttop" )) return Query::LASTTOP;
	if( iequals( str, "rand"    )) return Query::RAND;
	return Query::UNKNOWN;
}

//-----------------------------------------------------------------------------
void Donations::Run( CT &c ) {
	Query query = GetQuery( c->Args()[1] );
	if( query == Query::UNKNOWN ) return;
		
	switch( query ) {
	case Query::TOTAL:
		RespondTotal( c );
		return;
	case Query::TOP:
		RespondTop( c );
		return;
	case Query::LASTTOP:
		RespondLastTop( c );
		return;
	case Query::RAND:
		RespondRand( c );
		return;
	}
}

//-----------------------------------------------------------------------------
void Donations::RespondTotal( CT &c ) {
	if( Util::GetTime() < m_total_ctime + 15*60 ) {
		// cached response.
		KVResponse()
			.Put( "total", Util::RoundDecimal( m_total,2 ))
			.Put( "goal", Util::RoundDecimal( m_goal, 2 ))
			.Write( c );
	} else {
		// refresh
		DB::Get( "FORUMS" ).Execute( 
			DonationQuery::Create( c, *this, Query::TOTAL ));
	}
}

//-----------------------------------------------------------------------------
void Donations::RespondTop( CT &c ) {
	if( Util::GetTime() < m_top_ctime + 5*60 ) {
		KVResponse()
			.Put( "name", m_top_name )
			.Put( "amount", Util::RoundDecimal( m_top_amt, 2 ))
			.Write( c );
	} else {
		DB::Get( "FORUMS" ).Execute(
			DonationQuery::Create( c, *this, Query::TOP ));
	}
}

//-----------------------------------------------------------------------------
void Donations::RespondLastTop( CT &c ) {
	if( Util::GetTime() < m_lasttop_ctime + 60*60 ) {
		KVResponse()
			.Put( "name", m_lasttop_name )
			.Put( "amount", Util::RoundDecimal( m_lasttop_amt, 2 ))
			.Write( c );
	} else {
		DB::Get( "FORUMS" ).Execute(
			DonationQuery::Create( c, *this, Query::LASTTOP ));
	}
}

//-----------------------------------------------------------------------------
void Donations::RespondRand( CT &c ) {
	if( Util::GetTime() < m_rand_ctime + 60*60 ) {
		KVResponse()
			.Put( "name", m_rand_name )
			.Put( "amount", Util::RoundDecimal( m_rand_amt, 2 ))
			.Write( c );
	} else {
		DB::Get( "FORUMS" ).Execute(
			DonationQuery::Create( c, *this, Query::RAND ));
	}
}


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

}}}
