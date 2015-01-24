//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>

#include "system/system.h"


#include "db/core.h"
#include "db/transaction.h"

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
TEST_F( DbTests, DBTest1 ) {

	YAML::Node config = YAML::LoadFile("private/sqltest.yaml");
	
	DB::Endpoint info;
	info.address  = config["address" ].as<std::string>();
	info.username = config["user"    ].as<std::string>();
	info.password = config["password"].as<std::string>();
	info.database = config["database"].as<std::string>();
		
	auto &con = DB::Register( "test", info );
		
	auto test = DB::TransactionPtr( 
				new TestX( DB::CallbackTransaction::Bind( &TestProgram::Test, this )));  
	con.Execute( test );
	 
}

}