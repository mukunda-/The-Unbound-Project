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
std::string PasswordHasher::Hash( const Util::StringRef &input ) const {

	const int seedlength = 16;
	char saltseed[seedlength];

	for( int i = 0; i <= sizeof saltseed; i++ ) {

		// TODO: need to use a CSPRNG and not rand()!!

		saltseed[i] = (char)rand();
	}

	crypt_gensalt_rn( "$2y$", m_strength, input.CStr(), 
}

//-----------------------------------------------------------------------------
bool PasswordHasher::Verify( const Util::StringRef &input, 
							 const Util::StringRef &hash ) { 


}

//-----------------------------------------------------------------------------
}

