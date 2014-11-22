
#include "stdafx.h"
#include "responses.h"
#include "rxgserv.h"

namespace User { namespace RXGServ {

//-----------------------------------------------------------------------------
SimpleResponse::SimpleResponse( const std::string &text ) : 
		m_text(text) {

	EscapeLine( m_text );
}

//-----------------------------------------------------------------------------
void SimpleResponse::Write( Stream &stream ) {
	stream.Write().Formatted( "RT1: %s", m_text );
}

//-----------------------------------------------------------------------------
ListResponse &ListResponse::operator <<( const std::string &text ) {
	std::string escaped = text;
	EscapeLine( escaped );
	list.push_back( std::move(escaped) );
	return *this;
}

//-----------------------------------------------------------------------------
void ListResponse::Write( Stream &stream ) {
	auto writer = stream.Write();
	stream.Write().Formatted( "RT2:" );
}

}}