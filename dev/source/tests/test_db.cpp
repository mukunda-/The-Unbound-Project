//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>

#include "system/system.h"

#include "db/core.h"
#include "db/transaction.h"
#include "db/statement.h"

namespace Tests {

///////////////////////////////////////////////////////////////////////////////
class DbTests : public ::testing::Test {
	
public:
	System::Main *i_system = nullptr;
	DB::Manager *i_db = nullptr; 
	
protected:

	void SetUp() {

		i_system = new System::Main(1);
		i_db = new DB::Manager(1);
	}
	
	void TearDown() {
		delete i_db;
		i_db = nullptr;

		delete i_system;
		i_system = nullptr;

	}

};

///////////////////////////////////////////////////////////////////////////////
class TestTransaction1 : public DB::Transaction {

public:

private:

	///////////////////////////////////////////////////////////////////////////
	void VerifyResult( std::unique_ptr<sql::ResultSet> &result ) {
		// (see tests.php in db scripts)

		if( !result->next() ) FAIL();
		EXPECT_EQ( 1,        result->getInt(    1 ));
		EXPECT_EQ( "testes", result->getString( 2 ));
		EXPECT_EQ( 1.125,    result->getDouble( 3 ));
		
		if( !result->next() ) FAIL();
		EXPECT_EQ( 2,        result->getInt(    1 ));
		EXPECT_EQ( "banana", result->getString( 2 ));
		EXPECT_EQ( 9.875,    result->getDouble( 3 ));
		
		if( !result->next() ) FAIL();
		EXPECT_EQ( 3,        result->getInt(    1 ));
		EXPECT_EQ( "apple",  result->getString( 2 ));
		EXPECT_EQ( 0.84375,  result->getDouble( 3 ));
		
		if( !result->next() ) FAIL();
		EXPECT_EQ( 4,        result->getInt(    1 ));
		EXPECT_EQ( "carrot", result->getString( 2 ));
		EXPECT_EQ( 0.5,      result->getDouble( 3 ));
	}

	///////////////////////////////////////////////////////////////////////////
	PostAction Actions( DB::Line &line ) override {
		
		auto statement = line.CreateStatement(); 
		auto result = statement->ExecuteQuery( 
			"SELECT col1,col2,col3 FROM test1 ORDER BY col1" );
		
		VerifyResult( result );

		return NOP;
	}

	///////////////////////////////////////////////////////////////////////////
	void Completed( DB::TransactionPtr ptr, bool failed ) override {
		if( failed ) {
			FAIL();
		}

	}

	///////////////////////////////////////////////////////////////////////////
	

};

///////////////////////////////////////////////////////////////////////////////
DB::Endpoint GetTestDBInfo() {
	YAML::Node config = YAML::LoadFile("private/sqltest.yaml");
	
	DB::Endpoint info;
	info.address  = config["address" ].as<std::string>();
	info.username = config["user"    ].as<std::string>();
	info.password = config["password"].as<std::string>();
	info.database = config["database"].as<std::string>();

	return info;
}

///////////////////////////////////////////////////////////////////////////////
TEST_F( DbTests, DBTest1 ) {

	auto info = GetTestDBInfo();
		
	auto &con = DB::Register( "test", info );
		
	for( int i = 0; i < 30; i++ ) {
		auto test = DB::TransactionPtr( new TestTransaction1 );  
		con.Execute( std::move(test) );
	}

	 
}

}