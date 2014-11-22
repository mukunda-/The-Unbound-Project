//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "lidstream.h"
#include "error.h"

namespace Net {
	
namespace {
	/// -------------------------------------------------------------------
	/// Read a Varint from a stream. Max 28 bits.
	///
	/// @param stream Stream to read from.
	/// @param avail  How many bytes are available in the stream.
	/// @param value  Where to store the value.
	/// @returns Amount of bytes read from the stream. 
	///          0 means there wasn't sufficient data
	///          and the operation was cancelled.
	///
	/// @throws ParseError when the data is errornous (too long).
	///
	int ReadVarint( std::istream &stream, int avail, int *value ) {
		if( avail == 0 ) return 0;
		int result = 0;

		// 28 bits max.
		for( int i = 0; i < 4; i++ ) {
			int data = stream.get();
			if( data == EOF ) throw ParseError();
			
			result |= (data & 127) << (i*7);
			if( data & 128 ) {
				if( avail < (i+2) ) {
					// not enough data. rewind and fail.
					stream.seekg( -(i+1), std::ios_base::cur );
					return 0;
				}
			} else {
				(*value) = result;
				return i+1;
			}
		}
		throw ParseError();
	}
}

//-----------------------------------------------------------------------------
int LidStream::ProcessInput( std::istream &is, int bytes_available ) { 

	if( m_read_length == 0 ) {

		int bytesread = ReadVarint( is, bytes_available, &m_read_length );
		if( bytesread ) {
			return bytesread;
		}
		return 0;
		
	} else {
		if( bytes_available < m_read_length ) return 0; // need more data.
		int header;
		int bytesread = ReadVarint( is, bytes_available, &header );
		if( bytesread == 0 ) throw ParseError();
		m_read_length -= bytesread;

		Message msg( header, m_read_length, is  );

		Events::Stream::Dispatcher( shared_from_this() )
			.Receive( msg );
		
		m_read_length = 0;
		return true;
	}
}

//-----------------------------------------------------------------------------
auto LidStream::Write() -> Writer {
	return Writer( AcquireSendBuffer() );
}

//-----------------------------------------------------------------------------
LidStream::Message::Message( int header, int length, std::istream &stream ) :
		m_header( header ), m_length( length ), m_stream(stream) {
}

//-----------------------------------------------------------------------------
LidStream::Message::~Message() {
	m_stream.ignore( m_length );
}

//-----------------------------------------------------------------------------
void LidStream::Message::Parse( google::protobuf::MessageLite &msg ) {
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
