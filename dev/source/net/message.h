//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include <ostream>

namespace Net {

	class Message {

		// the header determines the type of message.
		uint32_t m_header;

	public:
		Message( uint32_t header );
		uint32_t Header() { return m_header; }
		template <typename T> T operator()() {
			return static_cast<T>(this);
		}

		/// -------------------------------------------------------------------
		/// Write this message to a stream.
		///
		virtual void Write( std::ostream &stream ) = 0;
		
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
