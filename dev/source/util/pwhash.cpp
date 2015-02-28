//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "hash/ow-crypt.h"
#include "pwhash.h"

//-----------------------------------------------------------------------------
namespace Util {

//-----------------------------------------------------------------------------
void PasswordHasher::SetStrength( int strength ) {
	if( m_strength < 1 ) m_strength = 1;
	if( m_strength > 31 ) m_strength = 31;
	m_strength = strength;
}

//-----------------------------------------------------------------------------
std::string PasswordHasher::Hash( const Util::StringRef &password ) const {
	 
	char saltseed[16];

	char salt[256];
	char hash[256];

	for( int i = 0; i < countof(saltseed); i++ ) {

		// TODO: need to use a CSPRNG and not rand()!!

		saltseed[i] = (char)rand();
	}

	char *res;
	res = crypt_gensalt_rn( "$2y$", m_strength, saltseed, 
		              countof(saltseed), salt, countof(salt) );
	assert( res == salt );

	res = crypt_rn( *password, salt, hash, countof(hash) );
	assert( res == hash );

	return hash;
}

//-----------------------------------------------------------------------------
bool PasswordHasher::Verify( const Util::StringRef &password, 
							 const Util::StringRef &hash ) { 
	
	char hash2[256];

	char *res = crypt_rn( *password, *hash, hash2, countof(hash2) );
	assert( res == hash2 );

	return strcmp( hash2, *hash ) == 0;
}

//-----------------------------------------------------------------------------
}

