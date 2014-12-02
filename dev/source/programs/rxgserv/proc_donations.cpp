 
#include "stdafx.h"
#include "myprocs.h" 

#include "db/line.h"
#include "db/connection.h"
#include "db/statement.h"
#include "db/core.h"
#include "util/time.h"

using namespace std;

//-----------------------------------------------------------------------------
namespace User { namespace RXGServ { namespace MyProcs {

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

}}}