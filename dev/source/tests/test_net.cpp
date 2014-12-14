//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "util/trie.h"
#include "util/stringles.h"

#include "system/system.h"
#include "net/core.h"
#include "net/listener.h"
#include "net/textstream.h"
#include "net/events.h"

namespace Testing {

///////////////////////////////////////////////////////////////////////////////

class NetTests : public ::testing::Test {
	static System::Instance *i_system;
	static Net::Instance *i_net;
	
protected:
	static void SetUpTestCase() {
		i_system = new System::Instance(1);
		i_net = new Net::Instance(1);
	}
	
	static void TearDownTestCase() {
		delete i_net;
		i_net = nullptr;

		delete i_system;
		i_system = nullptr;
	}

};

System::Instance *NetTests::i_system = nullptr;
Net::Instance    *NetTests::i_net = nullptr;

///////////////////////////////////////////////////////////////////////////////
class MyStream1 : public Net::TextStream {

public:
	static Net::StreamPtr Factory() {
		return std::make_shared<MyStream1>();
	}
};

class StreamHandler1 : public Net::Events::Stream::Handler {

public:
	void OnConnected( Net::StreamPtr &stream ) {
		stream->Close();
	}
};

TEST_F( NetTests, SimpleConnectionTest ) {
	StreamHandler1 handler;

	Net::Listener listener( 44412, MyStream1::Factory, &handler );
	Net::Connect( "127.0.0.1", "44412", MyStream1::Factory );
}

///////////////////////////////////////////////////////////////////////////////
}
