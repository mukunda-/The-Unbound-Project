//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//


#include <stdafx.h>
#include "message.h"
#include "error.h"

namespace Net {

//-----------------------------------------------------------------------------
Message::Message( uint32_t header ) : 
		m_header(header) 
{
	
}

//-----------------------------------------------------------------------------
void Message::Write( std::ostream &stream ) {
	throw std::runtime_error( "You cannot write this message." );
}

//-----------------------------------------------------------------------------
Remsg::Remsg( uint32_t header, std::istream &stream, int length ) :
		Message(header), m_stream(stream), m_length(length)
{
}

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

}
