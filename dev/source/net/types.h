//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Net {
	class Stream;
	using StreamPtr = std::shared_ptr<Stream>;
	using StreamFactory = std::function<StreamPtr()>;
	class Message;
	class SSLContext;
	using SSLContextPtr = std::shared_ptr<SSLContext>;
}
