
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

}}}