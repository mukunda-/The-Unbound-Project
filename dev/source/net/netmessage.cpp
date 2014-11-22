//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//


#include <stdafx.h>
#include "message.h"
#include "error.h"

namespace Net {

namespace {
	// get size of 28-bit varint
	int SizeofVarint( int value ) {
		if( value >= (1<<21) ) return 4;
		if( value >= (1<<14) ) return 3;
		if( value >= (1<<7) ) return 2;
		return 1;
	}
}
/*
//-----------------------------------------------------------------------------
Message::Message( uint32_t header ) : 
		m_header(header) 
{
	
}*/
/*
//-----------------------------------------------------------------------------
void Message::Write( std::ostream &stream ) {
	throw std::runtime_error( "You cannot write this message." );
}
*/

/*
//-----------------------------------------------------------------------------
Remsg::Remsg( uint32_t header, std::istream &stream, int length ) :
		Message(header), m_stream(stream), m_length(length)
{
}

//-----------------------------------------------------------------------------
Remsg::~Remsg() {
	if( !m_parsed ) {
		// if not parsed, discard the data.
		m_stream.ignore( m_length );
	}
}
*/

/*
//-----------------------------------------------------------------------------
void Remsg::Parse( google::protobuf::MessageLite &msg ) {
	
	google::protobuf::io::IstreamInputStream raw_input(&m_stream);
	google::protobuf::io::CodedInputStream input( &raw_input );
		
	google::protobuf::io::CodedInputStream::Limit limit = 
		input.PushLimit( m_length );
		
	if( !msg.MergeFromCodedStream( &input ) ) throw ParseError();
	if( !input.ConsumedEntireMessage() ) throw ParseError();
	input.PopLimit( limit );
	m_parsed = true;
	
}

//-----------------------------------------------------------------------------
PBMsg::PBMsg( uint32_t header, google::protobuf::MessageLite &msg ) :
		m_msg(msg), Message(header) {

}

//-----------------------------------------------------------------------------
void PBMsg::Write( std::ostream &std_stream ) {
	google::protobuf::io::OstreamOutputStream stream( &std_stream );
	google::protobuf::io::CodedOutputStream output( &stream );
	
	// Write the size.
	const int size = m_msg.ByteSize() + SizeofVarint( Header() );
	output.WriteVarint32( size );
	output.WriteVarint32( Header() );
	m_msg.SerializeToCodedStream( &output );
}*/

}
