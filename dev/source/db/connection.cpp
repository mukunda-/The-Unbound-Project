//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "connection.h"
#include "transaction.h"

using namespace std;

namespace DB {


//-----------------------------------------------------------------------------
Connection::Connection( const std::string &name, const Endpoint &endpoint, 
			int threads ) :
		m_name(name), m_endpoint( endpoint ) 
{
	
}

//-----------------------------------------------------------------------------
Connection::~Connection() {
}

//-----------------------------------------------------------------------------
void Connection::Execute( std::unique_ptr<Transaction> &t ) {
	lock_guard<std::mutex> lock(m_mut);
	
	if( m_free_threads > 0 ) {
		m_free_threads--;
	} else {
		m_work_queue.push_back( std::move(t) );
		return;
	}

	
}

}