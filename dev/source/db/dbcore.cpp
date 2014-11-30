//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "core.h"
#include "connection.h"
#include "system/console.h"
#include "system/system.h"
#include "exception.h"

using namespace std;

//-----------------------------------------------------------------------------
namespace DB {

Manager *g_manager = nullptr;

//-----------------------------------------------------------------------------
Connection &Register( const string &name, const Endpoint &endpoint, 
					  int threads ) {
	assert( g_manager );
	return g_manager->RegisterConnection( name, endpoint, threads );
}

//-----------------------------------------------------------------------------
Connection &Get( const string &name ) {
	assert( g_manager );
	return g_manager->GetConnection( name );
}

//-----------------------------------------------------------------------------
void Manager::QueueWork( unique_ptr<Transaction> &&transaction ) {
	{
		lock_guard<mutex> lock(m_work_mutex);
		m_work_queue.push_back( std::move(transaction) );
	}
	m_work_signal.notify_one();
}

//-----------------------------------------------------------------------------
Connection &Manager::RegisterConnection( const string &name, 
										 const Endpoint &endpoint,
										 int threads ) {
	if( m_conmap.count( name ) ) {
		throw invalid_argument( 
				"Connection name is already registered." );
	}

	Connection *con = new Connection( *this, name, endpoint, threads );
	m_conmap[ name ] = ConnectionPtr( con );
	return *con;
}

//-----------------------------------------------------------------------------
Connection &Manager::GetConnection( const string &name ) {
	return *m_conmap.at( name );
}

//-----------------------------------------------------------------------------
void Manager::DoActions( TransactionPtr &transaction, LinePtr &line, 
						 int &retries ) {
	while(true) {
		try {
			auto action = transaction->Actions( *line );

			if( action == Transaction::COMMIT ) {
				// todo run commit.
				(*line)->commit();
			} else if( action == Transaction::ROLLBACK ) {
				// todo run rollback.
				(*line)->rollback();
			}

			break; // success!
		} catch( const sql::SQLException &e ) {
			// retry, or fail!
			Exception ex(e);
			if( ex.Type() == Exception::RETRY && retries > 0 ) {
				// rollback and try again.
				retries--;
				(*line)->rollback();
				continue;
			}
			throw e;
		}
	}
}

//-----------------------------------------------------------------------------
void Manager::ExecuteTransaction( TransactionPtr transaction ) {
	Connection &conn = *transaction->m_parent;
	if( conn.GetFailState() ) {
		transaction->m_mysql_error = 0;
		transaction->Completed( std::move( transaction ), true );
		return;
	}

	int retries = 10;
	while(true) {
		try {
			LinePtr line = conn.GetLine();
		
			DoActions( transaction, line, retries );

			// push line back into pool, if an exception occurs
			// this is skipped and the connection is deleted.
			conn.PushLine( std::move(line), true );

			transaction->Completed( std::move(transaction), false );
			return;

		} catch( const sql::SQLException &e ) {
			Exception ex(e);
			if( ex.Type() == Exception::RECOVERABLE && retries > 0 ) {
				retries--;
				continue;
			} else if( ex.Type() == Exception::SOFTFAIL ) {
				System::Log( "DB Soft Failure: %d/%s: %s", 
					ex.MySQLCode(), ex.SQLState(), ex.what() );
			} else if( ex.Type() == Exception::HARDFAIL ) {
				System::Log( "DB Fatal Error: %d/%s: %s", 
					ex.MySQLCode(), ex.SQLState(), ex.what() );
				conn.SetFailState();
			} else {
				System::Log( "DB Failure: %d/%s: %s", 
					ex.MySQLCode(), ex.SQLState(), ex.what() );
			}
			transaction->m_mysql_error = ex.MySQLCode();
			transaction->Completed( std::move( transaction ), true );
			conn.FreeThread();
			return;
		}
	}
}

//-----------------------------------------------------------------------------
void Manager::ThreadMain() {
	unique_lock<mutex> lock( m_work_mutex, std::defer_lock );
	while( true ) {
		lock.lock();
		while( m_work_queue.empty() && !m_shutdown ) {
			m_work_signal.wait( lock );
		}
		
		if( !m_work_queue.empty() ) {
			std::unique_ptr<Transaction> transaction = 
					std::move(m_work_queue.front());
			m_work_queue.pop_front();
			lock.unlock();


			ExecuteTransaction( std::move(transaction) );
			
			// <process>.
			continue;
		}

		if( m_shutdown ) return;
	}
}

/// ---------------------------------------------------------------------------
/// Create an sql connection. This is called by a work thread.
///
/// @param endpoint Address and credentials to use.
///
unique_ptr<sql::Connection> Manager::Connect( const Endpoint &endpoint ) {
	 
	sql::ConnectOptionsMap props;
	props["hostName"] = endpoint.address;
	props["userName"] = endpoint.username;
	props["password"] = endpoint.password;
	if( !endpoint.database.empty() ) {
		props["schema"] = endpoint.database;
	}
	props["CLIENT_MULTI_STATEMENTS"] = true;
			
	unique_ptr<sql::Connection> conn(
		m_driver.connect( props )); 
			
	conn->setAutoCommit( true );
	return conn; 
	// exceptions caught in transaction execution.
}

//-----------------------------------------------------------------------------
Manager::Manager( int threads ) : 
			m_driver( *sql::mysql::get_mysql_driver_instance() ) {
	g_manager = this;

	for( int i = 0; i < threads; i++ ) {
		m_threadpool.push_back( 
			std::thread( std::bind( &Manager::ThreadMain, this )));
	}
	 
	// system.console isn't available yet!
	//	System::Console::Print( "%s", "Database subsystem started." );
}

//-----------------------------------------------------------------------------
Manager::~Manager() {

	// set shutdown flag, send signal, and wait for work to finish.
	{
		lock_guard<mutex> lock(m_work_mutex);
		m_shutdown = true;
	}

	m_work_signal.notify_all();
	for( auto &i : m_threadpool ) i.join();

	g_manager = nullptr;
}

//-----------------------------------------------------------------------------
sql::SQLString Manager::SqlString( const std::string &str ) {
	return sql::SQLString( str.c_str() );
}

}