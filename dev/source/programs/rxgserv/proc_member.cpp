
#include "stdafx.h"
#include "myprocs.h" 

#include "db/line.h"
#include "db/connection.h"
#include "db/statement.h"
#include "db/core.h"

using namespace std;
 
//-----------------------------------------------------------------------------
namespace User { namespace RXGServ { namespace MyProcs {

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

}}}