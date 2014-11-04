//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once


namespace DB {

class Test1 {
	std::shared_ptr<sql::Connection> m_db;

public:
	Test1() {
		using System::Console::Print;
		
		Print( "Running Database Test1" );
		 
		Print( " - loading config..." );
		YAML::Node config = YAML::LoadFile("private/sql.yaml");
		
		const std::string address = config["address"].as<std::string>();
		const std::string user = config["user"].as<std::string>();
		const std::string password = config["password"].as<std::string>();

		Print( " - getting driver" );
		sql::mysql::MySQL_Driver *driver;
		driver = sql::mysql::get_mysql_driver_instance();

		Print( " - Driver %s %d.%d.%d", 
				driver->getName().c_str(), 
				driver->getMajorVersion(), 
				driver->getMinorVersion(), 
				driver->getPatchVersion() );


		try {
			Print( " - Connecting to %s", address.c_str() );
			sql::Connection *conn = driver->connect( address.c_str(), user.c_str(), password.c_str() );
			m_db.reset(conn);
			conn->setSchema( "mukunda_unbound" );

			std::shared_ptr<sql::Statement> statement( m_db->createStatement() );
			statement->execute( "CREATE TABLE IF NOT EXISTS Test (test INT)" );
		
		} catch( const sql::SQLException e ) {
			Print( e.what() );
		}
		

		Print( "Finished." );
		//statement->execute( 

	}

};

}
