//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include <ostream>

namespace Net {

	class Message {

		// The header can contain up to 28 bits that
		// is sent with all messages.
		// lower 14 bits is the protocol id.
		// upper 14 bits is extra message data.
		uint32_t m_header;

	public:
		Message( uint32_t header );
		uint32_t Header() { return m_header; }
		uint16_t ID() { return m_header & 0x3FFF; }
		uint16_t Extra() { return m_header >> 14; }

		/// -------------------------------------------------------------------
		/// Write the message contents to a stream.
		///
		virtual void Write( std::ostream &stream );
	};

	/// an intermediate received message
	class Remsg : public Message {

		std::istream &m_stream; // source stream
		int m_length; // length of message data
		bool m_parsed = false; // if the message was parsed.

	public:
		Remsg( uint32_t header, std::istream &stream, int length );
		~Remsg();
		
		/// -------------------------------------------------------------------
		/// Parse a protobuf message from this.
		///
		/// @throws Net::ParseError on failure.
		///
		void Parse( google::protobuf::MessageLite &msg );
	};

	/// -----------------------------------------------------------------------
	/// PBMsg is a protobuf wrapper that does not copy the source.
	/// It represents a source that is used immediately and not stored.
	///
	class PBMsg : public Message {

		google::protobuf::MessageLite &m_msg;

	public:
		PBMsg( uint32_t header, google::protobuf::MessageLite &msg );
		void Write( std::ostream &stream ) final;
	};
}
