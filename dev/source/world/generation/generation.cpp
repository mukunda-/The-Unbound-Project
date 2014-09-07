//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

namespace World {
namespace Generation {
namespace Sampler {

static const int cosinterp2[] = {
0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 6, 7, 7, 8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 14, 15, 16, 17, 17, 18, 19, 20, 21, 22, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 
37, 39, 40, 41, 42, 43, 44, 46, 47, 48, 49, 50, 52, 53, 54, 56, 57, 58, 60, 61, 62, 64, 65, 66, 68, 69, 70, 72, 73, 75, 76, 78, 79, 80, 82, 83, 85, 86, 88, 89, 91, 92, 94, 95, 97, 98, 100, 101, 103, 105, 106, 108, 109, 111, 112, 114, 115, 117, 119, 120, 122, 123, 125, 126, 
128, 130, 131, 133, 134, 136, 137, 139, 141, 142, 144, 145, 147, 148, 150, 151, 153, 155, 156, 158, 159, 161, 162, 164, 165, 167, 168, 170, 171, 173, 174, 176, 177, 178, 180, 181, 183, 184, 186, 187, 188, 190, 191, 192, 194, 195, 196, 198, 199, 200, 202, 203, 204, 206, 207, 208, 209, 210, 212, 213, 214, 215, 216, 217, 
219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 234, 235, 236, 237, 238, 239, 239, 240, 241, 242, 242, 243, 244, 244, 245, 246, 246, 247, 247, 248, 249, 249, 250, 250, 250, 251, 251, 252, 252, 253, 253, 253, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, };

static const int costab[] = {

65536, 65535, 65531, 65525, 65516, 65505, 65492, 65476, 65457, 65436, 65413, 65387, 65358, 65328, 65294, 65259, 65220, 65180, 65137, 65091, 65043, 64993, 64940, 64884, 64827, 64766, 64704, 64639, 64571, 64501, 64429, 64354, 64277, 64197, 64115, 64031, 63944, 63854, 63763, 63668, 63572, 63473, 63372, 63268, 63162, 63054, 62943, 62830, 62714, 62596, 62476, 62353, 62228, 62101, 61971, 61839, 61705, 61568, 61429, 61288, 61145, 60999, 60851, 60700, 
60547, 60392, 60235, 60075, 59914, 59750, 59583, 59415, 59244, 59071, 58896, 58718, 58538, 58356, 58172, 57986, 57798, 57607, 57414, 57219, 57022, 56823, 56621, 56418, 56212, 56004, 55794, 55582, 55368, 55152, 54934, 54714, 54491, 54267, 54040, 53812, 53581, 53349, 53114, 52878, 52639, 52398, 52156, 51911, 51665, 51417, 51166, 50914, 50660, 50404, 50146, 49886, 49624, 49361, 49095, 48828, 48559, 48288, 48015, 47741, 47464, 47186, 46906, 46624, 
46341, 46056, 45769, 45480, 45190, 44898, 44604, 44308, 44011, 43713, 43412, 43110, 42806, 42501, 42194, 41886, 41576, 41264, 40951, 40636, 40320, 40002, 39683, 39362, 39040, 38716, 38391, 38064, 37736, 37407, 37076, 36744, 36410, 36075, 35738, 35401, 35062, 34721, 34380, 34037, 33692, 33347, 33000, 32652, 32303, 31952, 31600, 31248, 30893, 30538, 30182, 29824, 29466, 29106, 28745, 28383, 28020, 27656, 27291, 26925, 26558, 26190, 25821, 25451, 
25080, 24708, 24335, 23961, 23586, 23210, 22834, 22457, 22078, 21699, 21320, 20939, 20557, 20175, 19792, 19409, 19024, 18639, 18253, 17867, 17479, 17091, 16703, 16314, 15924, 15534, 15143, 14751, 14359, 13966, 13573, 13180, 12785, 12391, 11996, 11600, 11204, 10808, 10411, 10014, 9616, 9218, 8820, 8421, 8022, 7623, 7224, 6824, 6424, 6023, 5623, 5222, 4821, 4420, 4019, 3617, 3216, 2814, 2412, 2010, 1608, 1206, 804, 402, };


static int cosinterp[] = {

0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 
2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 6, 7, 7, 8, 9, 9, 
10, 10, 11, 12, 12, 13, 14, 14, 15, 16, 17, 17, 18, 19, 20, 21, 
22, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 
37, 39, 40, 41, 42, 43, 44, 46, 47, 48, 49, 50, 52, 53, 54, 56, 
57, 58, 60, 61, 62, 64, 65, 66, 68, 69, 70, 72, 73, 75, 76, 78, 
79, 80, 82, 83, 85, 86, 88, 89, 91, 92, 94, 95, 97, 98, 100, 101, 
103, 105, 106, 108, 109, 111, 112, 114, 115, 117, 119, 120, 122, 123, 125, 126, 
128, 130, 131, 133, 134, 136, 137, 139, 141, 142, 144, 145, 147, 148, 150, 151, 
153, 155, 156, 158, 159, 161, 162, 164, 165, 167, 168, 170, 171, 173, 174, 176, 
177, 178, 180, 181, 183, 184, 186, 187, 188, 190, 191, 192, 194, 195, 196, 198, 
199, 200, 202, 203, 204, 206, 207, 208, 209, 210, 212, 213, 214, 215, 216, 217, 
219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 
234, 235, 236, 237, 238, 239, 239, 240, 241, 242, 242, 243, 244, 244, 245, 246, 
246, 247, 247, 248, 249, 249, 250, 250, 250, 251, 251, 252, 252, 253, 253, 253, 
254, 254, 254, 254, 255, 255, 255, 255, 255, 256, 256, 256, 256, 256, 256, 256, 
256, };

enum {
	SEED_Y_SCALE=32,
	SEED_Z_SCALE=128,
	SEED_LENGTH = 0x4000,
	SEED_MASK = SEED_LENGTH-1
};

sample3d random_table[SEED_LENGTH+1];

//------------------------------------------------------------------------------------------------------------------------
// sse code constants
//
__m128i shuffle_maskA = _mm_set_epi8( 5,4,5,4,5,4,5,4,5,4,5,4,5,4,5,4 );
__m128i shuffle_maskB = _mm_set_epi8( (char)128,3, (char)128,2, (char)128,3, (char)128,2, (char)128,3, (char)128,2, (char)128,3, (char)128,2 );
__m128i shuffle_maskC = _mm_set_epi8( (char)128,(char)128,(char)128,(char)1,  (char)128,(char)128,(char)128,(char)0,  (char)128,(char)128,(char)128,(char)1,  (char)128,(char)128,(char)128,(char)0 );
__m128i xor_mask = _mm_set_epi32(0x00000000,0x00000000,0x00FF00FF,0x00FF00FF);

//------------------------------------------------------------------------------------------------------------------------
int TrilinearCosine( boost::uint32_t x, boost::uint32_t y, boost::uint32_t z, boost::uint32_t offset ) {
	// compute table index
	int xi = (x>>8), yi = (y>>8), zi = (z>>8);
	int index = (offset + xi + yi * SEED_Y_SCALE + zi * SEED_Z_SCALE) & (SEED_MASK);

	boost::uint8_t xf = x&0xFF, yf = y&0xFF, zf = z&0xFF; // x,y,z fractions
	//u8 xf = cosinterp2[x&0xFF], yf = cosinterp2[y&0xFF], zf = cosinterp2[z&0xFF]; // x,y,z fractions

	// compute a,b,c factor pairs (f),(1-f)
	int a = ((zf^255))|(zf<<8);
	int b = ((16384-(yf<<6)) )|((yf<<6)<<16);
	int c = ((256-(xf)))|((xf)<<16);
	//int b = ((yf^255)<<6)|((yf<<6)<<16);
	//int c = ((xf^255))|((xf<<16));

	sample3d *s = random_table + index;
	__m128i data =  _mm_loadl_epi64( (__m128i *)s );// data = random sample data
													// data = 8 bytes, .8 precision

	__m128i factors = _mm_cvtsi32_si128( a );		// factors = [~z][z] (2 bytes)
	factors = _mm_shufflelo_epi16( factors, 0x00 );	// factors = [~z][z][~z][z][~z][z][~z][z] (8 bytes)
	data = _mm_maddubs_epi16( factors, data );		// multiply vertically and add
													// data = 4 words, .16 precision

	factors = _mm_cvtsi32_si128( b );				// factors = [~y][y] (2 words)
	factors = _mm_shufflelo_epi16( factors, 0x44 );	// factors = [~y][y][~y][y] (4 words)
	data = _mm_mulhi_epi16( data, factors );		// multiply vertically and save hiword
													// data = 4 words, .14 precision

	factors = _mm_cvtsi32_si128( c );				// factors = [~z][z] (2 words)	
	factors = _mm_shufflelo_epi16( factors, 0x50 );	// factors = [~z][z][~z][z] (4 words)
	data = _mm_madd_epi16( data, factors );			// multiply vertically and add
													// data = 4 words, .22 precision
	data = _mm_hadd_epi32( data, data );			// add products together
													// data = 2 words, .22 precision
	int result = _mm_cvtsi128_si32( data ) >> 6;	// return shifted result (.16 precision)
	return result;
}

//------------------------------------------------------------------------------------------------------------------------
void TrilinearCosinePair( boost::uint32_t x1, boost::uint32_t y1, boost::uint32_t z1, boost::uint32_t offset1, boost::uint32_t offset2, int &result1, int &result2 ) {
	// split fractions and integers
	// compute table indices
	int xi1 = x1 >> 8, yi1 = y1 >> 8, zi1 = z1 >> 8;
	int index1 = (xi1 + yi1 * SEED_Y_SCALE + zi1 * SEED_Z_SCALE);// & (random_mask);
	int index2 = (index1 + offset2) & (SEED_MASK);
	index1 = (index1 + offset1) & (SEED_MASK);

	int xf1 = x1 & 255, yf1 = y1 & 255, zf1 = z1 & 255;
	//int a = (xf1)|(yf1<<8)|(zf1<<16);
	int a = (cosinterp2[xf1])|(cosinterp2[yf1]<<8)|(cosinterp2[zf1]<<16);
	
	__m128i factors1 = _mm_cvtsi32_si128(a);			// [0][z][y][x]
	factors1 = _mm_unpacklo_epi8( factors1, factors1 ); // [0][0][z][z][y][y][x][x]
	factors1 = _mm_xor_si128( factors1, xor_mask );		
	// factors1: [0][FF][z][~z][y][~y][x][~x]
	//			  7   6  5   4  3   2  1   0
	
	// data = [data2][data1] 16 bytes
	sample3d *s1 = random_table + index1;
	__m128i data =  _mm_loadl_epi64( (__m128i *)s1 );
	sample3d *s2 = random_table + index2;
	data =  _mm_castpd_si128(_mm_loadh_pd( _mm_castsi128_pd(data), (double *)s2 ));
	//----------------------------------------------------------------------------
	// Z AXIS:
	// factors = [z][~z][z][~z][z][~z][z][~z][z][~z][z][~z][z][~z][z][~z]
	__m128i factors = _mm_shuffle_epi8( factors1, shuffle_maskA );
	data = _mm_maddubs_epi16( factors, data );			// multiply and add words
														// data is now 4+4 words (.16 precision)
	//----------------------------------------------------------------------------
	// Y AXIS:
	factors = _mm_shuffle_epi8( factors1, shuffle_maskB );// factors = [y][~y][y][~y][y][~y][y][~y] (words)
	data = _mm_madd_epi16( data, factors );				// multiply and add words
														// data is now 2+2 dwords (.24 precision)
	//----------------------------------------------------------------------------
	// X AXIS:
	factors = _mm_shuffle_epi8( factors1, shuffle_maskC );// factors = [x][~x][x][~x] (dwords)
	data = _mm_mullo_epi32( data, factors );			// multiply by factors
														// data is now 2+2 dwords (.32 precision)
	data = _mm_hadd_epi32( data, data );				// add adjacent samples
	//----------------------------------------------------------------------------
	data = _mm_srai_epi32( data, 16 );					// reduce samples (32->16)
	result1 = (_mm_cvtsi128_si32( data ));				// data[63:32] = sample2
	result2 = (_mm_extract_epi32( data, 1 ));			// data[31:0]  = sample1
}

//------------------------------------------------------------------------------------------------------------------------
int BilinearCosine( boost::uint32_t x, boost::uint32_t y, boost::uint32_t offset ) {
	int index = (x>>8)+(y>>8)*SEED_Y_SCALE+offset;
	sample3d *samp = random_table + index;

	x &= 255; y &= 255;
	x = cosinterp[x]; 
	y = cosinterp[y];
	                                                                 
	int a = (samp[0].data[0]<<8) + (samp[1].data[0] - samp[0].data[0]) * x; // 16 bit results
	int b = (samp[0].data[2]<<8) + (samp[1].data[2] - samp[0].data[2]) * x;
	a = (a<<8) + (b-a) * y; // 24bit result
	return (a) >> 8; // no rounding? return 16bit unsigned result
}

//------------------------------------------------------------------------------------------------------------------------
int Bilinear( boost::uint32_t x, boost::uint32_t y, boost::uint32_t offset ) {
	int index = (x>>8)+(y>>8)*SEED_Y_SCALE+offset;
	sample3d *samp = random_table + index;
	
	x &= 255; y &= 255;
	
	int a = (samp[0].data[0]<<8) + (samp[1].data[0] - samp[0].data[0]) * x; // 16 bit results
	int b = (samp[0].data[2]<<8) + (samp[1].data[2] - samp[0].data[2]) * x;
	a = (a<<8) + (b-a) * y; // 24bit result
	return (a) >> 8; // no rounding? return 16bit unsigned result
}

//------------------------------------------------------------------------------------------------------------------------
__forceinline int Point( boost::uint32_t x, boost::uint32_t y, boost::uint32_t offset ) {
	sample3d *data = &random_table [((x>>8)*SEED_Y_SCALE+(y>>8)*SEED_Z_SCALE+offset)&SEED_MASK ];

	return data->data[0];
}


//------------------------------------------------------------------------------------------------------------------------
}

}

}

