//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "stringref.h"

//-----------------------------------------------------------------------------
namespace Util {

	//-------------------------------------------------------------------------
	class PasswordHasher {

		int m_strength = 10;

	public:

		/** -------------------------------------------------------------------
		 * Set the strength of password hashing.
		 *
		 * @param strength 0=weakest, 100=strongest
		 */
		void SetStrength( int strength );

		/** -------------------------------------------------------------------
		 * Hash a string.
		 */
		std::string Hash( const Util::StringRef &input ) const;

		/** -------------------------------------------------------------------
		 * Check if a string matches a hash.
		 */
		bool Verify( const Util::StringRef &input, 
					 const Util::StringRef &hash ) const;
	};

}