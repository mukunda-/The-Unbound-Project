//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

namespace Net {

	class Message {

		// the header determines the type of message.
		uint32_t m_header;

	public:
		Message( uint32_t header );
		uint32_t Header() { return header; }	
		template <typename T> operator()() {
			return static_cast<T>(this);
		}
	};

	/// -----------------------------------------------------------------------
	/// PBWrapper is a protobuf wrapper that does not copy the source.
	/// It represents a source that is used immediately and not stored.
	///
	class PBWrapper : public Message {

		google::protobuf::MessageLite &m_msg;

	public:
		PBWrapper( uint32_t header, google::protobuf::MessageLite &msg );

	};
}
