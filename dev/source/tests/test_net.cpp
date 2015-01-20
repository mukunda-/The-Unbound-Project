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
	
public:
	static std::mutex *m_lock; 
	System::Main *i_system = nullptr;
	Net::Instance *i_net = nullptr; 
	
protected:

	void SetUp() {
		m_lock = new std::mutex;

		i_system = new System::Main(1);
		i_net = new Net::Instance ;
	}
	
	void TearDown() {
		delete i_net;
		i_net = nullptr;

		delete i_system;
		i_system = nullptr;

		delete m_lock;
		m_lock = nullptr;
	}

	 
};

//System::Main  *NetTests::i_system = nullptr;
//Net::Instance *NetTests::i_net = nullptr;
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

	for( int jjj = 0; jjj < 50; jjj++ ) {{
		 
		//std::shared_ptr<StreamHandler1>//
		Asev::Handler::ptr handler = std::make_shared<StreamHandler1>();

		auto factory = [handler]() mutable {
			auto stream = std::make_shared<MyStream1>();
			stream->AsevSubscribe( handler );
			return stream;
		};

		Net::Listener listener( 44412, factory );

		for( int i = 0; i < 1; i++ ) {
		
			//std::cout << "connecting." << std::endl;
			auto stream = std::static_pointer_cast<MyStream1>(
				Net::Connect( "127.0.0.1", "44412", factory ));

			{ 
				stream->Write() << "Test Message 1\n" << "Test 2\n";
				stream->Write() << "Test Message 3\n" << "Test 4\n";
			}
			stream->Close();
	
		}
 
	}}
}

///////////////////////////////////////////////////////////////////////////////
class MyStream : public Net::LidStream {
		
	int m_progress = 0;
	bool m_accepted = false;

	///////////////////////////////////////////////////////////////////////
	template <typename T>
	void Mirror( T &pb, int header ) {
		if( !m_accepted ) return;

		Write() << header << pb;
	}

	///////////////////////////////////////////////////////////////////////
	void VerifyMsg1( Net::Proto::Test::Test &pb, int header ) {
		EXPECT_EQ( 33232535, pb.test_int() );
		EXPECT_EQ( "abc123qb37ui5mo89,;7r8l6n7kbej6wh5e", pb.test_string() );

		Mirror( pb, header );
	}

	///////////////////////////////////////////////////////////////////////
	void VerifyMsg2( Net::Proto::Test::Test2 &pb, int header ) { 
		EXPECT_EQ( "paofeijpowaiejfwae", pb.aa() );
		EXPECT_FALSE( pb.has_bb() );
		EXPECT_EQ( 0xabcd0123abcd022L, pb.cc() );

		Mirror( pb, header );
	}

	///////////////////////////////////////////////////////////////////////
	void VerifyMsg3( Net::Proto::Test::Test3 &pb, int header ) {
		EXPECT_EQ( "test@example.com", pb.email() );
		EXPECT_EQ( "jonathan doeminic", pb.name() );
		EXPECT_EQ( 90210, pb.id() );

		Mirror( pb, header );
	}

	///////////////////////////////////////////////////////////////////////
	void VerifyMsg4( Net::Proto::Test::Test &pb, int header ) {
		EXPECT_EQ( 8023049, pb.test_int() );
		EXPECT_EQ( "honda", pb.test_string() );

		Mirror( pb, header );
	}

	///////////////////////////////////////////////////////////////////////
	void Accepted() override {
		m_accepted = true;
	}

	///////////////////////////////////////////////////////////////////////
	void Connected() override {

		///////////////////////////////////////////////////////////////////
		Net::Proto::Test::Test testmsg1;
		testmsg1.set_test_int( 33232535 );
		testmsg1.set_test_string( "abc123qb37ui5mo89,;7r8l6n7kbej6wh5e"  );
		Write() << 12 << testmsg1;
		Write() << 0x999 << testmsg1; 

		///////////////////////////////////////////////////////////////////
		Net::Proto::Test::Test2 testmsg2;
		testmsg2.set_aa( "paofeijpowaiejfwae" );
		testmsg2.set_cc( 0xabcd0123abcd022L );
			
		Net::Proto::Test::Test3 testmsg3;
		testmsg3.set_email( "test@example.com" );
		testmsg3.set_name( "jonathan doeminic" );
		testmsg3.set_id( 90210 );
		Write() << 25 << testmsg2 << 0x1111 << testmsg2 << 0x22222 << testmsg3;
		Write() << 0x999 << testmsg1; 

		///////////////////////////////////////////////////////////////////
		testmsg1.Clear();
		testmsg1.set_test_int( 8023049 );
		testmsg1.set_test_string( "honda" );
		Write() << 0x4444444 << testmsg1; 
		Write() << 0x999 << testmsg1; 
		Write() << 0x333333 << testmsg2; 
	}

	///////////////////////////////////////////////////////////////////////
	void SendFailed( const boost::system::error_code & ) override {
		FAIL();
	}

	///////////////////////////////////////////////////////////////////////
	void Disconnected( const boost::system::error_code &err ) {
		if( err ) {
			if( err != boost::asio::error::eof ) {
				FAIL();
			}
		}
	}
		
	///////////////////////////////////////////////////////////////////////
	void Receive( Net::Message &nmsg ) override {
		auto msg = nmsg.Cast<Net::LidStream::Message>();

		static int headers[] = { 12, 25, 0x1111, 0x22222, 0x4444444, 0x333333 };

		///////////////////////////////////////////////////////////////////
		if( msg.Header() == 0x999 ) return; // (data skipping test)

		if( m_progress >= countof(headers) ) FAIL();
		if( msg.Header() != headers[m_progress] ) FAIL();
			
		///////////////////////////////////////////////////////////////////
		switch( m_progress ) {
		case 0: {
			Net::Proto::Test::Test pb;
			msg.Parse( pb );
			VerifyMsg1( pb, msg.Header() );
			break;
		} case 1:
			case 2:
			case 5: {
			Net::Proto::Test::Test2 pb;
			msg.Parse( pb );
			VerifyMsg2( pb, msg.Header() );
			break;
		} case 3: {
			Net::Proto::Test::Test3 pb;
			msg.Parse( pb );
			VerifyMsg3( pb, msg.Header() );
			break;
		} case 4: {
			Net::Proto::Test::Test pb;
			msg.Parse( pb );
			VerifyMsg4( pb, msg.Header() );
			Close();
			break;
		}}
		m_progress++;
	}

public:
	~MyStream() {
		EXPECT_EQ( 6, m_progress );
	}
};

///////////////////////////////////////////////////////////////////////////////
TEST_F( NetTests, ProtobufTest ) {
	
	static auto factory = []() -> Net::StreamPtr {
		return std::make_shared<MyStream>();
	};

	for( int i = 0; i < 1; i++ ) {
		
		Net::ConnectAsync( "localhost", "9021", factory );

	}

	std::this_thread::sleep_for( std::chrono::milliseconds(3000));
}

///////////////////////////////////////////////////////////////////////////////
}
