//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "lidstream.h"
#include "error.h"

namespace Net {
	
namespace {

	/// -------------------------------------------------------------------
	/// Get the byte size of a packed 28-bit varint.
	///
	int SizeofVarint( int value ) {
		if( value >= (1<<21) ) return 4;
		if( value >= (1<<14) ) return 3;
		if( value >= (1<<7) ) return 2;
		return 1;
	}

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
int LidStream::ProcessInput(  int bytes_available ) { 

	if( m_read_length == 0 ) {

		// read "length" varint, if it isn't available yet, return 0 and wait
		// for more data
		int bytesread = ReadVarint( GetInputStream(), bytes_available, &m_read_length );
		if( bytesread ) {
			return bytesread;
		}
		return 0;
		
	} else {
		if( bytes_available < m_read_length ) {
			return 0; // need more data to complete the message
		}

		// read header
		int header;
		int bytesread = ReadVarint( GetInputStream(), bytes_available, &header );
		if( bytesread == 0 ) throw ParseError();

		// we want to consume this many bytes in the stream
		int total_length = m_read_length;

		// data length is the size of the protobuf data
		int data_length = total_length - bytesread; 

		m_read_length = 0; // reset for next message

		Message msg( header, data_length, GetInputStream() );

		if( !m_close_called ) {
			Receive( msg );
			Events::Stream::Dispatcher( shared_from_this() )
				.Receive( msg );
		}
		 
		return total_length;
	}
}

//-----------------------------------------------------------------------------
auto LidStream::Write() -> Writer {
	return Writer( AcquireSendBuffer() );
}

//-----------------------------------------------------------------------------
LidStream::Message::Message( int header, int length, std::istream &stream ) :
		m_header( header ), m_length( length ), m_stream( stream ) {
}

//-----------------------------------------------------------------------------
LidStream::Message::~Message() {

	// if the message wasn't parsed, ignore the bytes in the stream.
	if( !m_parsed ) {
		m_stream.ignore( m_length );
	}
}

//-----------------------------------------------------------------------------
void LidStream::Message::Parse( google::protobuf::MessageLite &msg ) {
	
	// user may not parse the message twice.
	if( m_parsed ) throw ParseError();

	if( m_length < 4096 ) {
	
		char buffer[4096];
		m_stream.read( buffer, m_length );
		if( !msg.ParseFromArray( buffer, m_length )) {
			throw ParseError();
		}
	
	} else {
		// TODO switch to arenas
		auto buffer = std::unique_ptr<char>( new char[ m_length ] );
		m_stream.read( buffer.get(), m_length );
		if( !msg.ParseFromArray( buffer.get(), m_length )) {
			throw ParseError();
		}
	}
	
	// (fuck it. google's stream shit is messed up.)
//	google::protobuf::io::IstreamInputStream raw_input(&m_stream);
//	google::protobuf::io::CodedInputStream input( &raw_input );
//	google::protobuf::io::CodedInputStream::Limit limit = 
//		input.PushLimit( m_length );
//	if( !msg.ParseFromCodedStream( &input ) ) throw ParseError();
//	if( !input.ConsumedEntireMessage() ) throw ParseError();
//	input.PopLimit( limit );

	m_parsed = true;

}

//-----------------------------------------------------------------------------
auto LidStream::Writer::operator<<( 
		google::protobuf::MessageLite &data ) -> Writer& {

	assert( m_expecting_data );
	m_expecting_data = false; 

	google::protobuf::io::OstreamOutputStream stream( &m_stream );
	google::protobuf::io::CodedOutputStream output( &stream );
	
	// Write the size.
	const int size = data.ByteSize() + SizeofVarint( m_next_header );
	output.WriteVarint32( size );
	output.WriteVarint32( m_next_header );
	data.SerializeToCodedStream( &output );
	return *this;
}

}
