
#include "stdafx.h"
#include "responses.h"
#include "rxgserv.h"

namespace User { namespace RXGServ {
	
std::unordered_map<RCodes,std::string> RCodeText::m_values;
void RCodeText::Init() {
	m_values[RCodes::UNKNOWN_COMMAND] = "Unknown command.";
}

const std::string &RCodeText::Get( RCodes code ) {
	if( m_values.size() == 0 ) Init();
	static std::string empty;
	if( m_values.count(code) == 0 ) return empty;
	return m_values.at(code);
}


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
	writer << "RT2:\n";
	for( auto &i : list ) {
		writer << i << "\n";
	}
	writer << "\n";
}

//-----------------------------------------------------------------------------
ErrorResponse::ErrorResponse( RCodes code ) : m_code(code) {

}

//-----------------------------------------------------------------------------
void ErrorResponse::Write( Stream &stream ) {
	stream.Write().Formatted( "ERR: %d %s", (int)m_code, 
			RCodeText::Get( m_code ) );
}

}}