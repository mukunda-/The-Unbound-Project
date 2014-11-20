//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "forwards.h"
#include "connection.h"
#include "core.h"
#include "transaction.h"

using namespace std;

namespace DB {

//-----------------------------------------------------------------------------
Connection::Connection( Manager &manager, const std::string &name, 
			const Endpoint &endpoint, int threads ) :
		m_manager(manager), m_name(name), 
		m_endpoint( endpoint ), m_free_threads(threads)
{
	
}

//-----------------------------------------------------------------------------
Connection::~Connection() {
}

//-----------------------------------------------------------------------------
void Connection::Execute( TransactionPtr &t ) {
	t->parent = this;
	{
		lock_guard<std::mutex> lock( m_mut );
	
		if( m_free_threads > 0 ) {
			m_free_threads--;
		} else {
			m_work_queue.push_back( std::move(t) );
			return;
		}
	}
	m_manager.QueueWork( std::move(t) );
}

//-----------------------------------------------------------------------------
LinePtr Connection::GetLine() {
	{
		lock_guard<std::mutex> lock( m_mut );
		if( !m_linepool.empty() ) {
			LinePtr line = std::move(m_linepool.top());
			m_linepool.pop();
			return line;
		}
	}

	// create new line
	return LinePtr( new Line( m_manager, m_endpoint ) );
}

//-----------------------------------------------------------------------------
void Connection::PushLine( LinePtr &&line ) {
	
	lock_guard<std::mutex> lock( m_mut );
	m_linepool.push( std::move(line) );

}

}