//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once 

#include "stream.h"
#include "message.h"
#include "writerbase.h"

namespace Net {

/// ---------------------------------------------------------------------------
/// A Length-ID-Data stream. A stream that uses messages that contain 
/// a small header and delimited byte string.
///
class LidStream : public Stream {
	class Writer;

	int m_read_length = 0;

protected:

	virtual int ProcessInput( std::istream &is, int bytes_available );
	
public:
	class Message;
	
	/// -----------------------------------------------------------------------
	/// Obtain an object used to write messages.
	///
	Writer Write();
};

/// ---------------------------------------------------------------------------
/// Implementation of the stream writer to output data.
///
class LidStream::Writer : public WriterBase {
	 
	bool m_expecting_data = false;
	int m_next_header;

public:
	Writer( Stream::SendLock &&lock ) : WriterBase( std::move(lock) ) {}

	/// -----------------------------------------------------------------------
	/// This writer accepts an integer first which is the header.
	/// and then it is followed by the data.
	///
	Writer &operator<<( int header ) {
		assert( !m_expecting_data );
		m_expecting_data = true;
		m_next_header = header;
		return *this;
	}
	
	/// -----------------------------------------------------------------------
	/// Output the message payload, must send header before this.
	///
	/// @param data protobuf message.
	///
	Writer &operator<<( google::protobuf::MessageLite &data );
};

/// ---------------------------------------------------------------------------
/// A received message.
///
class LidStream::Message : public Net::Message {

	int m_header;  // message header
	int m_length;  // length of data portion

	bool m_parsed = false; // true if the data was read, 
				           // false if it should be discarded 
						   // in the destructor.

	std::istream &m_stream; // stream to read data from.

public:

	MessageInfo Info() {
		return 0x00010001; // todo magic number.
	}

	Message( int header, int length, std::istream &stream );
	~Message();

	/// -----------------------------------------------------------------------
	/// @returns the message header.
	///
	int Header() { return m_header; }

	/// -----------------------------------------------------------------------
	/// @returns the length of the message data in bytes.
	///
	int Length() { return m_length; }

	/// -----------------------------------------------------------------------
	/// Parse a protobuf message from this.
	///
	/// @throws Net::ParseError on failure.
	///
	void Parse( google::protobuf::MessageLite &msg );
};

}