//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include <boost/cstdint.hpp>
#include "world/Cell.h"

//------------------------------------------------------------------------------------------------------------------------
namespace World {

	//------------------------------------------------------------------------------------------------------------------------
	namespace Generation {

		//------------------------------------------------------------------------------------------------------------------------
		// zones for default generator (C1)
		enum {
			C1_ZONE_SEA
		};

		//------------------------------------------------------------------------------------------------------------------------
		namespace Sampler {

			// 3d sample, data[0] is the actual sample and data[1-3] are copies of
			typedef struct t_sample3d {
				boost::int8_t data[4]; // x*4 (uses next sample) + y*2 + z
			} sample3d;

			// sse optimized sampling function set

			// function name affixes:
			// "Cosine": cosine mapped linear version
			// "Pair": computes and returns two values for better efficiency

			// coordinates are specified in 24.8 fixed point
			// "offsets" are an index into the random table where the sampling begins (aka "seed")
			//   this value is a direct offset into the random table! for best sample variation use
			//   large offsets between sampling sets
			// samples returned (unless otherwise stated) are in range 0-65535 (x.16 precision)

			//------------------------------------------------------------------------------------------------------------------------
			// trilinear sampling
			int TrilinearCosine( boost::uint32_t x, boost::uint32_t y, boost::uint32_t z, boost::uint32_t offset );
			void TrilinearCosinePair( boost::uint32_t x, boost::uint32_t y, boost::uint32_t z, 
													boost::uint32_t offset1, boost::uint32_t offset2, 
													int &result1, int &result2 );

			//------------------------------------------------------------------------------------------------------------------------
			// bilinear sampling
			int BilinearCosine( boost::uint32_t x, boost::uint32_t y, boost::uint32_t offset );
			int Bilinear( boost::uint32_t x, boost::uint32_t y, boost::uint32_t offset );

			//------------------------------------------------------------------------------------------------------------------------
			// point sampling
			// fractional part of sampling coordinates is ingored
			int Point( boost::uint32_t x, boost::uint32_t y, boost::uint32_t offset );
			
			//------------------------------------------------------------------------------------------------------------------------
			// initialize random table
			void Initialize( int seed );

		}

		
		//------------------------------------------------------------------------------------------------------------------------
		// generate a chunk for a context
		//
		Cell *Create( Context *ct, boost::uint64_t world_index );
	}

}

