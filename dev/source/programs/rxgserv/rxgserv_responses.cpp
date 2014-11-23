
#include "stdafx.h"
#include "responses.h"
#include "rxgserv.h"

namespace User { namespace RXGServ {

//-----------------------------------------------------------------------------
std::unordered_map<RCodes,std::string> RCodeText::m_values;
void RCodeText::Init() {
	m_values[RCodes::UNKNOWN_COMMAND] = "Unknown command.";
}

//-----------------------------------------------------------------------------
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
	stream.Write().Formatted( "RT1: %s\n", m_text );
}

//-----------------------------------------------------------------------------
ListResponse &ListResponse::operator <<( const std::string &text ) {
	std::string escaped = text;
	EscapeLine( escaped );
	m_list.push_back( std::move(escaped) );
	return *this;
}

//-----------------------------------------------------------------------------
void ListResponse::Write( Stream &stream ) {
	auto writer = stream.Write();
	writer << "[RT2]\n";
	for( auto &i : m_list ) {
		writer << ':' << i << '\n';
	}
	writer << '\n';
}

//-----------------------------------------------------------------------------
KVResponse &KVResponse::Erase( const std::string &key ) {
	if( m_values.count(key) ) {
		m_values.erase( key );
	}
	return *this;
}

//-----------------------------------------------------------------------------
KVResponse &KVResponse::Put( const std::string &key, 
							 const std::string &value ) {

	std::string escaped = value;
	EscapeLine(escaped);
	m_values[key] = escaped;
	return *this;
}

//-----------------------------------------------------------------------------
void KVResponse::Write( Stream &stream ) {
	auto writer = stream.Write();
	writer << "[RT3]\n";
	for( auto &i : m_values ) {
		writer.Formatted( "%s: %s\n", i.first, i.second );
	}
	writer << '\n';
}

//-----------------------------------------------------------------------------
ErrorResponse::ErrorResponse( const std::string &status, 
		const std::string &desc ) : m_status( status ), m_desc( desc ){ 
}

//-----------------------------------------------------------------------------
void ErrorResponse::Write( Stream &stream ) {
	stream.Write().Formatted( "[ERR] %s %s\n", m_status, m_desc );
}

}}