
#include "stdafx.h"
#include "myprocs.h" 

#include "db/line.h"
#include "db/connection.h"
#include "db/statement.h"
#include "db/core.h"
#include "util/feed.h"

using namespace std;
 
//-----------------------------------------------------------------------------
namespace User { namespace RXGServ { namespace MyProcs {

//-----------------------------------------------------------------------------
class PerksQuery : public ContextTransaction {

public:
	enum class Type {
		STEAM,
		FORUM
	};

	/// -----------------------------------------------------------------------
	/// @param ct      Procedure context
	/// @param type    Type of query
	/// @param userids IDs for each request
	/// @param query   List of Steam or Forum IDs to look up.
	///
	PerksQuery( CT &ct, Type type, std::vector<int> &&userids, 
					  std::vector<long long> &&query ) : 
			ContextTransaction(ct), m_type(type), 
			m_userids( std::move(userids) ), m_query( std::move(query) )
	{}

	static DB::TransactionPtr Create( CT &ct, Type type, 
									  std::vector<int> &&userids, 
									  std::vector<long long> &&query ) {

		return DB::TransactionPtr(
			new PerksQuery( ct, type, std::move(userids), 
								  std::move(query) ));
	}
		
	//-------------------------------------------------------------------------
private:
	PostAction Actions( DB::Line &line ) {

		auto statement = line.CreateStatement(); 

		DB::QueryBuilder query( line, 
			"SELECT %s, expires1, expires5 FROM %s WHERE %s IN (%s)" );

		if( m_type == Type::STEAM ) {
			query % DB::RS( "steamid" )
				  % DB::RS( "SteamDonationCache" )
				  % DB::RS( "steamid" );
		} else if( m_type == Type::FORUM ) {
			query % DB::RS( "userid" )
				  % DB::RS( "UserDonationCache" )
				  % DB::RS( "userid" );
		} else {
			return NOP;
		}

		std::string idlist;
		for( long long id : m_query ) {
			if( idlist.empty() ) {
				idlist = std::to_string(id);
			} else {
				idlist += ",";
				idlist += std::to_string(id);
			}
		}

		if( idlist.empty() ) return NOP;
		query % DB::RS(idlist);

		auto result = statement->ExecuteQuery( query.String() );
		
		while( result->next() ) {

			m_results.push_back( result->getInt64( 1 ) );
			m_results.push_back( result->getInt( 2 ) );
			m_results.push_back( result->getInt( 3 ) ); 
		}
				
		return NOP;
	}

	//-------------------------------------------------------------------------
	void OnSuccess( DB::TransactionPtr ptr ) override {
		
		ListResponse response; 
		std::vector<bool> found;
		for( int i = 0; i < m_query.size(); i++ ) {
			found.push_back( false );
		}

		for( int i = 0; i < m_results.size(); i+=3 ) {
			
			for( int request_index = 0; request_index < m_query.size(); 
													request_index++ ) {

				if( m_results[i] != m_query[request_index] ) continue;

				response << Util::Format( "%d %d %d", 
								m_userids[request_index], 
								(int)m_results[i+1], 
								(int)m_results[i+2] );

				found[ request_index ] = true;
				break;
			}
		}
		
		for( int i = 0; i < m_query.size(); i++ ) {
			if( found[i] ) continue;

			// default response for missing results
			response << Util::Format( "%d 0 0", m_userids[i] );
		}

		response.Write( m_ct );
	}
		
	Type m_type;  
	std::vector<int>       m_userids;
	std::vector<long long> m_query;

	std::vector<long long> m_results;
};
	  
//-----------------------------------------------------------------------------
void Perks::Run( CT &c ) {
	DB::Connection &connection = DB::Get( "FORUMS" );

	std::vector<int> userids;
	std::vector<long long> queries;

	bool steam = false;
	if( boost::iequals( c->Args()[1], "steam" )) {
		steam = true;
	} else if( boost::iequals( c->Args()[1], "forum" )) {
		steam = false;
	} else {
		return;
	}

	try {

		for( int i = 2; i < c->Args().Count()-1; i += 2 ) {
			userids.push_back( std::stoi( c->Args()[i] ));
			 
			if( steam ) {
				SteamID steamid( c->Args()[i+1], SteamID::Formats::AUTO,true );
				if( !steamid ) return;
				queries.push_back( steamid.To64() );

			} else {
				queries.push_back( std::stoi( c->Args()[i+1] ));
			}
		}

	} catch( std::logic_error & ) {
		// when an invalid number is passed.
		return;
	}
	
	DB::Get( "FORUMS" ).Execute( 
		PerksQuery::Create( c, 
			steam ? PerksQuery::Type::STEAM : PerksQuery::Type::FORUM,
			std::move( userids ),
			std::move( queries )));
}

}}}