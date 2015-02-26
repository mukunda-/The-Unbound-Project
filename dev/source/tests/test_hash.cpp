//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>

#include "util/pwhash.h"
#include "hash/sha256.h"
#include "util/stringles.h"

namespace Tests {

///////////////////////////////////////////////////////////////////////////////
TEST( HashTests, SHA256 ) {
	std::string hash = "test";

	std::vector<std::string> expected = {
		"9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08",
		"7b3d979ca8330a94fa7e9e1b466d8b99e0bcdea1ec90596c0dcc8d7ef6b4300c",
		"5b24f7aa99f1e1da5698a4f91ae0f4b45651a1b625c61ed669dd25ff5b937972",
		"2ace3a22375fdf5c60d78b612ccc70c88e31cfa7c3f9be023388980a2326f2fd",
		"d32b3b15471a3ddfa23c5d6d147958e8e817f65878f3df30436e61fa639127b1",
		"c475204b01b18aa30282df8f80c602c13b2c9cc813b86a481a7b821bd80f075b",
		"4e6a8d5354c5df23ebd7a7d8eba5061d02d28e000f7fadecf73d4b5bca40e793",
		"cb90fcef122aaeed3ff1c881fd131172a55f86084cf513970ab80086d2d9fa4b",
		"d36e4f43c5243135e038611e679adee4bf197290e84e0203727cb6761929e072",
		"bc89c6f72947bcd2f783d342a46cafcfccfcc2e7884a34f1cfe8f55bad2d200e",
		"960741afdda2c23a81f5fbadd12e9e333f1eeddd23e3cda0129ca47bc4ae8b9c",
		"3231e8e38f7220abc38a231b6761af796f730e562c77f31d69bfa97257fc4f0f"
	};

	for( std::string &iter : expected ) {
		hash = Hash::sha256( hash );
		EXPECT_EQ( iter, hash );
	}
}

///////////////////////////////////////////////////////////////////////////////
TEST( HashTests, BCRYPT ) {
	Util::PasswordHasher hasher;

	std::vector<std::string> verify_tests = {
//		"testes",   "$2a$10$QkiCClLZnK6zyxFx0SLZ1.zmIz0Eds7MDBSgQzK8/KZ6T231EsF0m",
		"password", "$2a$10$pWnhpz/LVJepDb.CieZFdOIuaeU0OHRKftDcAs80NgKor1tZA.GQy",
//		"fotm",     "$2a$10$7ECDVuCySRDPeoyyuBSDfeJYQnqUiLbb5YIVrEGjDQoL5XMA09uOG",
//		"Chance",   "$2a$10$DlM5KH.Df0/LuOUCbDiX/.4QYAivrRJT6Oqk4UuL6jbJraViT7GLu",
		"Pandora",  "$2a$10$.0XO5Vf7ZGUcCuCnSZaVf.ff8r0pUz7/WkyVd98A1WsLJIYOxuvEW"
	};

	for( int i = 0; i < (int)verify_tests.size(); i += 2 ) {
		EXPECT_TRUE( hasher.Verify( verify_tests[i], verify_tests[i+1] ));

		verify_tests[i] += ' ';
		EXPECT_FALSE( hasher.Verify( verify_tests[i], verify_tests[i+1] ));
	}

	for( int i = 0; i < 16; i++ ) {
		std::string password = Util::StringGarbage( 12+i );
		std::string hash = hasher.Hash( password );
		EXPECT_TRUE( hasher.Verify( password, hash ));
		password += ' ';
		EXPECT_FALSE( hasher.Verify( password, hash )); 
	}
}

}
