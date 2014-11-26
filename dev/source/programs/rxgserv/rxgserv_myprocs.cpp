
#include "stdafx.h"
#include "rxgserv.h"
#include "responses.h"

#include "db/core.h"
#include "db/transaction.h"
#include "db/line.h"
#include "db/statement.h"

using namespace std;
 
//-----------------------------------------------------------------------------
namespace User { namespace RXGServ { namespace MyProcs {
	using CT = Procs::Context::ptr;

	//-------------------------------------------------------------------------
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

	//-------------------------------------------------------------------------
	void Unknown::Run( CT &c ) {

		ErrorResponse( "BADINPUT", "Unknown command" ).Write( *c );
	}

	//-------------------------------------------------------------------------
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

	//-------------------------------------------------------------------------
	class PerksTransaction : public ContextTransaction {

	public:
		enum class Type {
			STEAM,
			FORUM
		};

		/// -------------------------------------------------------------------
		/// @param ct   Procedure context
		/// @param type Type of query
		/// @param id   Steam Account ID or forum user id.
		///
		PerksTransaction( CT &ct, Type type, int id ) : 
				ContextTransaction(ct), m_type(type), m_id(id)
		{}

		static DB::TransactionPtr Create( CT &ct, Type type, int id ) {

			return DB::TransactionPtr( new PerksTransaction( ct, type, id ));
		}
		
		//---------------------------------------------------------------------
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

		//---------------------------------------------------------------------
		void OnSuccess( DB::TransactionPtr ptr ) override {
			
			KVResponse()
				.Put( "expires1", m_expires1 )
				.Put( "expires5", m_expires5 )
				.Write( m_ct );
		}
		
		Type m_type;
		int m_id;

		int m_expires1 = 0;
		int m_expires5 = 0;
	};
	 
	//-------------------------------------------------------------------------
	void Perks::Run( CT &c ) {
		DB::Connection &connection = DB::Get( "FORUMS" );
		if( boost::iequals( c->Args()[1], "steam" ) ) {
			SteamID steamid( c->Args()[2] );
			if( !steamid || !steamid.ToS32() ) return;

			// steamid lookup
			connection.Execute( PerksTransaction::Create( 
						c, PerksTransaction::Type::STEAM, 
						steamid.ToS32() ));

		} else if( boost::iequals( c->Args()[1], "forum" ) ) {
			if( !Util::IsDigits( c->Args()[2] ) ) return;

			// forumid lookup

			connection.Execute( PerksTransaction::Create( 
						c, PerksTransaction::Type::FORUM, 
						std::stoi( c->Args()[2] )));
		}
	}

	//-------------------------------------------------------------------------
	class MemberTransaction : public ContextTransaction {

	public:
		//---------------------------------------------------------------------
		MemberTransaction( CT &ct, SteamID &id ) : 
				ContextTransaction(ct), m_id(id) 
		{}
		
		//---------------------------------------------------------------------
		static DB::TransactionPtr Create( CT &ct, SteamID &id ) {
			return DB::TransactionPtr( new MemberTransaction( ct, id ));
		}

	private:
		//---------------------------------------------------------------------
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
				m_rank    = result->getString( 1 ).c_str();
				m_groupid = result->getInt( 2 );
			}

			return NOP;
		}
		
		//---------------------------------------------------------------------
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
		
		//---------------------------------------------------------------------
		SteamID m_id;
		std::string m_rank;
		int m_groupid = 0;
	};

	//-------------------------------------------------------------------------
	void Member::Run( CT &c ) {
		
		DB::Connection &connection = DB::Get( "FORUMS" );
		SteamID steamid( c->Args()[1], SteamID::Formats::AUTO , true );
		if( !steamid ) return;

		connection.Execute( MemberTransaction::Create( c, steamid ));
	}

}}}