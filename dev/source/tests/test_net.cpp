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
#include "net/lidstream.h"
#include "net/events.h"

#include "proto/test/test.pb.h"
#include "proto/test/test2.pb.h"
#include "proto/test/test3.pb.h"

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
	}

	void Receive( Net::StreamPtr &nstream, Net::Message &nmsg ) override { 
		NETLOCKGUARD;
		auto &stream = nstream->Cast<MyStream1>();
		auto &msg    = nmsg.Cast<Net::TextStream::Message>();
		
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

	auto factory = [&handler]() -> Net::StreamPtr {
		auto stream = std::make_shared<MyStream1>();
		stream->AsevSubscribe( handler );
		return stream;
	}

	Net::Listener listener( 44412, MyStream1::Factory );

	for( int i = 0; i < 100; i++ ) {
		
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
TEST_F( NetTests, ProtobufTest ) {

#define teststr1  "abc123qb37ui5mo89,;7r8l6n7kbej6wh5e" 

	class MyStream : public Net::LidStream {

		int m_flags = 0;
		bool m_accepted = false;

		void Accepted() override {
			m_accepted = true;
		}

		void Connected() override {
			m_flags = 0xFFFF;
		}

		void SendFailed( const boost::system::error_code & ) override {
			FAIL();
		}

		void Disconnected( const boost::system::error_code &err ) {
			if( err ) {
				if( err != boost::asio::error::eof ) {
					FAIL();
				}
			}
		}
		
		void Receive( Net::Message &nmsg ) override {
			auto msg = nmsg.Cast<Net::LidStream::Message>();

			if( msg.Header() == 12 ) {
				m_flags |= 1;
				msg.Parse(
			} else if( msg.Header() == 25 ) {
				m_flags |= 2;
			} else if( msg.Header() == 0x1111 ) {
				m_flags |= 4;
			} else if( msg.Header() == 0x22222 ) {
				m_flags |= 8;
			} else if( msg.Header() == 0x333333 ) {
				m_flags |= 16;
			} else if( msg.Header() == 0x4444444 ) {
				m_flags |= 32;
			} else {
				FAIL();
			}

			if( m_accepted ) {
				//Write() << msg;
			}
		}

		~MyStream() {
			
		}
	};
 
	auto handler = std::make_shared<Handler>();
	auto factory = []() -> Net::StreamPtr {
		return std::make_shared<MyStream>();
	};

	for( int i = 0; i < 100; i++ ) {
		Net::Proto::Test::Test testmsg1;
		testmsg1.set_test_int( 33232535 );
		testmsg1.set_test_string( teststr1 );
		
		Net::ConnectAsync( "localhost", "9021", nullptr, factory );

	}
}

///////////////////////////////////////////////////////////////////////////////
}
