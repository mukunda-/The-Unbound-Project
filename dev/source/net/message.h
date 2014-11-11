//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

namespace Net {

	class Message {

		uint16_t header;
		::google::protobuff::MessageLite data;

	public:
		uint16_t Header() { return header; }
		template <typename T> Get
	};
}
