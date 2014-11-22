//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once 

#include "stream.h"
#include "message.h"
#include "writerbase.h"

namespace Net {

/// ---------------------------------------------------------------------------
/// A stream that communicates in newline delimited text strings.
///
class TextStream : public Stream {
	class Writer;

	std::string m_current_message;
	
protected:

	int ProcessInput( std::istream &is, int bytes_available ) override;
	
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
class TextStream::Writer : public WriterBase {
	
public:
	Writer( Stream::SendLock &&lock );
	
	template <typename ... Args>
	void expand( Args... ) {}

	/// -----------------------------------------------------------------------
	/// Output text.
	///
	/// @param message Text to send.
	/// @returns this for chaining.
	/// 
	Writer &operator<<( const std::string &message ) {
		m_stream << message;
		return *this;
	}
	
	/// -----------------------------------------------------------------------
	/// Output formatted text.
	///
	/// @param message Message or message template.
	///                A newline is added to the end.
	/// @param args    Arguments to substitute.
	/// @returns this for chaining.
	/// 
	template <typename ... Args>
	Writer & Formatted( const std::string &message, Args...args ) {
		
		if( sizeof...(args) == 0 ) {
			// raw message.
			m_stream << message << '\n';
		} else {
			boost::format formatter( message );
			expand( formatter % args ... );
			m_stream << formatter.str() << '\n';
		}
		
		return *this;
	}
	 
};

/// ---------------------------------------------------------------------------
/// A received message.
///
class TextStream::Message : public Net::Message {
	
	std::string m_string;

public:

	MessageInfo Info() {
		return 0x00020001; // todo magic number.
	}
	
	Message( const char *data ) : m_string(data) {};
	Message( const std::string &data ) : m_string(data) {};
	Message( const std::string &&data ) : m_string(std::move(data)) {};

	/// -----------------------------------------------------------------------
	/// @returns the message.
	///
	const std::string &operator()() { return m_string; }
	
};

}