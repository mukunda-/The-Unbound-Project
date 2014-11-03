//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once


namespace DB {

class Test1 {
	std::shared_ptr<sql::Connection> m_db;

public:
	Test1() {
		YAML::Node config = YAML::LoadFile("private/sql.yaml");
		
		const std::string address = config["address"].as<std::string>();
		const std::string user = config["user"].as<std::string>();
		const std::string password = config["password"].as<std::string>();

		sql::mysql::MySQL_Driver *driver;
		driver = sql::mysql::get_mysql_driver_instance();
		sql::Connection *conn = driver->connect( address, user, password );
		m_db.reset(conn);

		System::Console::Print( "Running Database Test1" );
		
		std::shared_ptr<sql::Statement> statement( m_db->createStatement() );
		statement->execute( "CREATE TABLE IF NOT EXISTS Test (test INT)" );
		
		System::Console::Print( "OK!" );
		//statement->execute( 

	}

};

}