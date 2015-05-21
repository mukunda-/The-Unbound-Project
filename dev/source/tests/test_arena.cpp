//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>

#include "system/system.h"
#include "mem/arena/arena.h"
#include "mem/arena/use.h"

// Arena Allocator Tests

namespace Tests {

///////////////////////////////////////////////////////////////////////////////
TEST( ArenaTests, ArenaTestBasic ) {

	System::Main system( 32, System::StartMode::PASS );
	System::Start();

	char *data = (char*)Mem::Arena::Alloc( 256 );
	for( int i = 0; i < 256; i++ ) data[i] = 'a';

	EXPECT_EQ( ((int)data) & 15, 0 ); // check alignment.

	char *data2 = (char*)Mem::Arena::Alloc( 500, 2048 );
	EXPECT_EQ( ((int)data2) & 2047, 0 ); // check alignment.
	for( int i = 0; i < 256; i++ ) EXPECT_EQ( 'a', data[i] );
	for( int i = 0; i < 500; i++ ) data2[i] = 'b';
	for( int i = 0; i < 256; i++ ) EXPECT_EQ( 'a', data[i] );
	for( int i = 0; i < 500; i++ ) EXPECT_EQ( 'b', data2[i] );

	char value = 'c';
	for( int ii = 0; ii < 1000; ii++ ) { 
		char *data3 = (char*)Mem::Arena::Alloc( 2000, 32 );
		EXPECT_EQ( ((int)data3) & 31, 0 ); // check alignment.
		for( int i = 0; i < 2000; i++ ) data3[i] = value;
		value++;
		for( int i = 0; i < 256; i++ ) EXPECT_EQ( 'a', data[i] );
		for( int i = 0; i < 500; i++ ) EXPECT_EQ( 'b', data2[i] );

		Mem::Arena::Free( data3 );
	}
	
	for( int ii = 0; ii < 10; ii++ ) {
		char *data3 = (char*)Mem::Arena::Alloc( 500000, 256 );
		EXPECT_EQ( ((int)data3) & 255, 0 ); // check alignment.
		for( int i = 0; i < 500000; i++ ) data3[i] = value;
		value++;
		for( int i = 0; i < 256; i++ ) EXPECT_EQ( 'a', data[i] );
		for( int i = 0; i < 500; i++ ) EXPECT_EQ( 'b', data2[i] );

		Mem::Arena::Free( data3 );
	}

	for( int i = 0; i < 256; i++ ) EXPECT_EQ( 'a', data[i] );
	for( int i = 0; i < 500; i++ ) EXPECT_EQ( 'b', data2[i] );
	Mem::Arena::Free( data2 );

	for( int i = 0; i < 256; i++ ) EXPECT_EQ( 'a', data[i] );
	Mem::Arena::Free( data );
	
}

///////////////////////////////////////////////////////////////////////////////
class ArenaTestClass : public Mem::Arena::Use {

	int m_values[128];
	int m_seed;

public:
	ArenaTestClass( int seed ) {
		m_seed = seed;
		for( int i = 0; i < 128; i++ ) {
			m_values[i] = seed;
		}
	}

	void Verify() {
		for( int i = 0; i < 128; i++ ) {
			EXPECT_EQ( m_seed, m_values[i] );
			m_values[i]++;
		}
		m_seed++;
	}
};

///////////////////////////////////////////////////////////////////////////////
TEST( ArenaTests, ArenaTestUse ) {
	System::Main system( 32, System::StartMode::PASS );
	System::Start();
	
	int seed = 52320;

	ArenaTestClass *test1, *test2, *test3;
	test1 = new ArenaTestClass( seed++ );
	test2 = new ArenaTestClass( seed++ );
	test3 = new ArenaTestClass( seed++ );

	for( int i = 0; i < 1000; i++ ) {
		
		test1->Verify();
		test2->Verify();
		test3->Verify();

		delete test3;
		test3 = test2;
		test2 = test1;
		test1 = new ArenaTestClass( seed++ );
	}

	delete test1;
	delete test2;
	delete test3;
}

///////////////////////////////////////////////////////////////////////////////
TEST( ArenaTests, ArenaTestAllocator ) {
	// stl allocator test
	System::Main system( 32, System::StartMode::PASS );
	System::Start();

	// im not sure what im doing..
	std::vector< std::string, Mem::Arena::Allocator<std::string> > my_vector;

	my_vector.push_back( "test string 1" );
	my_vector.push_back( "test string 2" );
	my_vector.push_back( "test string 3" );
	my_vector.push_back( "test string 4" );
	my_vector.push_back( "test string 5" );

	EXPECT_EQ( "test string 1", my_vector[0] );
	EXPECT_EQ( "test string 2", my_vector[1] );
	EXPECT_EQ( "test string 3", my_vector[2] );
	EXPECT_EQ( "test string 4", my_vector[3] );
	EXPECT_EQ( "test string 5", my_vector[4] );
	my_vector.pop_back();

	my_vector.resize( 32 );
	my_vector.resize( 4 );
	my_vector.resize( 3200 );
	my_vector.resize( 4 );

	my_vector.erase( my_vector.begin() );
	EXPECT_EQ( "test string 2", my_vector[0] );
	EXPECT_EQ( "test string 3", my_vector[1] );
	EXPECT_EQ( "test string 4", my_vector[2] );
	EXPECT_EQ( 3, my_vector.size() );
}

///////////////////////////////////////////////////////////////////////////////
}