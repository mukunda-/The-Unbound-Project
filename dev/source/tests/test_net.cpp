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

namespace Tests {

///////////////////////////////////////////////////////////////////////////////

class NetTests : public ::testing::Test {
	static System::Main *i_system;
	static Net::Instance *i_net;

	
protected:
	static void SetUpTestCase() {
		m_lock = new std::mutex;

		i_system = new System::Main(1);
		i_net = new Net::Instance(1);
	}
	
	static void TearDownTestCase() {
		delete i_net;
		i_net = nullptr;

		delete i_system;
		i_system = nullptr;

		delete m_lock;
		m_lock = nullptr;
	}

public:
	static std::mutex *m_lock;
};

System::Main  *NetTests::i_system = nullptr;
Net::Instance *NetTests::i_net = nullptr;
std::mutex    *NetTests::m_lock = nullptr;

#define NETLOCKGUARD std::lock_guard<std::mutex> lock_guard_( *NetTests::m_lock )

///////////////////////////////////////////////////////////////////////////////
class MyStream1 : public Net::TextStream {
public:
	int m_expected = 4;
	static Net::StreamPtr Factory() {
		return std::make_shared<MyStream1>();
	}

	~MyStream1() {
		NETLOCKGUARD;
		EXPECT_EQ( 4, m_expected );
	}
};

///////////////////////////////////////////////////////////////////////////////
class StreamHandler1 : public Net::Events::Stream::Handler {
	
public:
	void Accepted( Net::StreamPtr &nstream ) override {
		NETLOCKGUARD;
		auto &stream = nstream->Cast<MyStream1>();
		stream.m_expected = 0;
	}
	
	void SendFailed( 
			Net::StreamPtr &stream,
			const boost::system::error_code &error ) override {

		NETLOCKGUARD;
		 
		std::cout << error.message();
	}

	void Connected( Net::StreamPtr &stream ) override {
		NETLOCKGUARD;
	}

	void Disconnected( Net::StreamPtr &stream, 
					   const boost::system::error_code &error ) override {
		NETLOCKGUARD;
		std::cout << error.message();
	}

	void Receive( Net::StreamPtr &nstream, Net::Message &nmsg ) override { 
		NETLOCKGUARD;
		auto &stream = nstream->Cast<MyStream1>();
		auto &msg    = nmsg.Cast<Net::TextStream::Message>();
		std::cout << msg() << std::endl;
		switch( stream.m_expected ) {
		case 0:
			EXPECT_EQ( "Test Message 1", msg() );
			break;
		case 1:
			EXPECT_EQ( "Test 2", msg() );
			break;
		case 2:
			EXPECT_EQ( "Test Message 3", msg() );
			break;
		case 3:
			EXPECT_EQ( "Test 4", msg() );
			
			break;
		case 4:
			FAIL();
			break;
		}
		stream.m_expected++;
	}
};

///////////////////////////////////////////////////////////////////////////////
TEST_F( NetTests, SimpleConnectionTest ) {
	
	auto handler = std::make_shared<StreamHandler1>();
	Net::Listener listener( 44412, MyStream1::Factory, handler );

	for( int i = 0; i < 1; i++ ) {
		
		auto stream = std::static_pointer_cast<MyStream1>(
			Net::Connect( "127.0.0.1", "44412", MyStream1::Factory ));

		{
			// DEBUG single message.
			stream->Write() << "Test Message 1\n" << "Test 2\n"
			 << "Test Message 3\n" << "Test 4\n";
		}
		
		stream->Close();
	
	}
 
}

///////////////////////////////////////////////////////////////////////////////
}
