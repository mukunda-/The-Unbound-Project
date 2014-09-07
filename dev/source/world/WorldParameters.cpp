//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

namespace World {
//-------------------------------------------------------------------------------------------------
WorldParameters::WorldParameters() {
	loaded = false;
	data = 0;
	data_h = 0;
}

//-------------------------------------------------------------------------------------------------
WorldParameters::~WorldParameters() {
	Clean();
}

//-------------------------------------------------------------------------------------------------
void WorldParameters::Clean() {
	if( data ) delete[] data;
	if( data_h ) delete[] data_h;
	data = 0;
	data_h = 0;
	loaded = false;
}

#define seastamp_size 512
//-------------------------------------------------------------------------------------------------
void WorldParameters::PaintStamp( int x, int z, boost::uint8_t *stamp, boost::uint8_t *temp ) {
	int px,pz;
	for( pz = 0; pz < seastamp_size+1; pz++ ) {
		for( px = 0; px < seastamp_size+1; px++ ) {
			int spx = px-seastamp_size/2;
			int spz = pz-seastamp_size/2;
			if( x+spx < 0 || x+spx >= width*16 ) continue;
			if( z+spz < 0 || z+spz >= length*16 ) continue;
			spx += x;
			spz += z;
			int stamp_sample = stamp[px+pz*(seastamp_size+1)];

			if( temp[spx+spz*width*16] >= 128 ) { // land
				if( temp[spx+spz*width*16] > 128+stamp_sample ) 
					temp[spx+spz*width*16] = 128+stamp_sample;
			} else {
				if( temp[spx+spz*width*16] < 127-stamp_sample )
					temp[spx+spz*width*16] = 127-stamp_sample;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
void WorldParameters::ComputeSeaLevel() {
	boost::uint8_t *temp;
	temp = new boost::uint8_t[width*16*length*16];
	for( int i = 0; i < width*16*length*16; i++ ) {
		// swizzle data and apply threshold function
		int source, dest;
		int sx = i & ((width*16)-1);
		int sz = i / (width*16);
		source = (sx&15)+(sx/16)*256+(sz&15)*16+(sz/16)*width*256;
		dest = i;
		int a = data_h[source].params[MPH_ZONES] == Generation::C1_ZONE_SEA ? 0 : 255;
		temp[i] = a;
	}

	
	boost::uint8_t stamp[(seastamp_size+1)*(seastamp_size+1)];

	for( int x = 0; x < seastamp_size+1; x++ ) {
		for( int y = 0; y < seastamp_size+1; y++ ) {
			int dx = seastamp_size/2-x, dy = seastamp_size/2-y;
			int d = dx*dx+dy*dy;
			d = (int)(sqrt((double)d) * (127.0 / (seastamp_size/2.0)));
			if( d > 127 ) d = 127;
			stamp[x+y*(seastamp_size+1)] = d;
		}
	}
	
	for( int z = 1; z < length*16-1; z++ ) {
		for( int x = 1; x < width*16-1; x++ ) {
			if( temp[x+z*width*16] >= 128 ) {
				if( temp[x-1 + z*width*16] < 128 ||
					temp[x+1 + z*width*16] < 128 ||
					temp[x + (z-1)*width*16] < 128 ||
					temp[x + (z+1)*width*16] < 128 )  {

					// paint here
					PaintStamp( x, z, stamp, temp );
				}
			}
		}
	}

	// copy/swizzle to param data
	for( int i = 0; i < width*16*length*16; i++ ) {
		
		int source, dest;
		int sx = i & ((width*16)-1);
		int sz = i / (width*16);
		source = i; 
		dest = (sx&15)+(sx/16)*256+(sz&15)*16+(sz/16)*width*256;
		int a = temp[source];
		data_h[dest].params[MPH_SEALEVEL] = a;
	}

	/*
	////////////////////////////////////////////////////
	
	u8 *test_data;
	test_data = new u8[width*16*length*16*3];

	for( int x = 0; x < width*16; x++ ) {
		for( int z = 0; z < length*16; z++ ) {
			int index = (x%16)+(z%16)*16+(x/16)*256+(z/16)*width*256;
			int color = data_h[index].params[MPH_SEALEVEL];
			test_data[(x+z*width*16)*3] = color;
			test_data[(x+z*width*16)*3+1] = color;
			test_data[(x+z*width*16)*3+2] = color;
		}
	}
	debug_dump( width*16, length*16, test_data );
	delete[] test_data;*/
	
	delete[] temp;
}

//-------------------------------------------------------------------------------------------------
void WorldParameters::ComputeZoneForms() {

	// this is a function that passes over the zone data
	// the upscaled data (16x) is formed by doing a turbulated sampling filter under a box filter to
	// determine which zone is dominant for that single upscaled cell

	

	for( int cz = 0; cz < length; cz++ )  {
		for( int cx = 0; cx < width; cx++ ) {
			for( int z = 0; z < 16; z++ ) {
				for( int x = 0; x < 16; x++ ) {
					int tx = cx*16+x;
					int tz = cz*16+z;


					short sample_counts[256];
					for( int i = 0; i < 256; i++ ) {
						sample_counts[i] = 0;

					}

					int gaussian_range = 8;
					
					// BOX FILTER
					int gx,gz;
					for( gz = -gaussian_range; gz <= gaussian_range; gz++) {
						for( gx = -gaussian_range; gx <= gaussian_range; gx++ ) {
							int turbulence_x = Generation::Sampler::Bilinear( (tx+gx)*16, (tz+gz)*16, 0 ) * 16/65536-8;
							int turbulence_z = Generation::Sampler::Bilinear( (tx+gx)*16, (tz+gz)*16, 4444 ) * 16/65536-8;
							int sample_x = tx + turbulence_x + gx;
							int sample_z = tz + turbulence_z + gz;
							sample_x = sample_x < 0 ? 0 : sample_x;
							sample_x = sample_x >= width*16  ? width*16-1  : sample_x;
							sample_z = sample_z < 0 ? 0 : sample_z;
							sample_z = sample_z >= length*16 ? length*16-1 : sample_z;
							int sample = data[(sample_x>>4)+(sample_z>>4)*width].params[MP_ZONE];
							sample_counts[sample]++;
						}
					}

					int max_index = 16;

					int best_count=0, best_index=0;
					for( int i = 0; i < max_index; i++ ) {
						if( sample_counts[i] > sample_counts[best_index] ) {
							best_index = i;
							best_count = sample_counts[i];
						}
					}

					data_h[ cx*256 + x + z*16 + cz*256*width ].params[MPH_ZONES] = best_index;
				}
			}
		}
	}
	 
}

//-------------------------------------------------------------------------------------------------
bool WorldParameters::LoadZoneFormsCache() {
	BinaryFile f( ZoneFormsCachePath().c_str(), BinaryFile::MODE_READ );
	if( f.IsOpen() ){
		return false;
	}

	for( int i = 0; i < width*length*256; i++ ) {
		data_h[i].params[MPH_ZONES] = f.Read8();
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
void WorldParameters::SaveZoneFormsCache() {
	BinaryFile f( ZoneFormsCachePath().c_str(), BinaryFile::MODE_WRITE );
	
	for( int i = 0; i < width*length*256; i++ ) f.Write8( data_h[i].params[MPH_ZONES] );
}

//-------------------------------------------------------------------------------------------------
bool WorldParameters::LoadSeaLevelCache() {
	BinaryFile f( SeaLevelCachePath().c_str(), BinaryFile::MODE_WRITE );

	if( !f.IsOpen() ) {
		return false;
	}

	for( int i = 0; i < width*length*256; i++ ) data_h[i].params[MPH_SEALEVEL] = f.Read8();

	return true;
}

//-------------------------------------------------------------------------------------------------
void WorldParameters::SaveSeaLevelCache() {
	BinaryFile f( SeaLevelCachePath().c_str(), BinaryFile::MODE_WRITE );

	for( int i = 0; i < width*length*256; i++ ) f.Write8( data_h[i].params[MPH_SEALEVEL] );
}

//-------------------------------------------------------------------------------------------------
std::string WorldParameters::ZoneFormsCachePath() {
	std::string path = filepath;
	path.append( ".zonecache" );
	return path;
}

//-------------------------------------------------------------------------------------------------
std::string WorldParameters::SeaLevelCachePath() {
	std::string path = filepath;
	path.append( ".seacache" );
	return path;
}

//-------------------------------------------------------------------------------------------------
void WorldParameters::Load( const char *file ) {
	if( loaded ) Clean();
	filepath = file;

	BinaryFile f( file, BinaryFile::MODE_READ );
	f.Read32(); // 'abws'
	f.Read32(); // version

	f.ReadString( worldname, 64 );

	width = f.Read16();
	length = f.Read16();
	depth = f.Read16();
	kernel = f.Read16();
	int compression = f.Read16();
	sea_level = f.Read16();
	f.Seek(256);

	data = new map_param[ width*length ];
	data_h = new map_param_h[ width*length*256 ];

	for( int i = 0; i < width*length*256; i++ ) data_h[i].params[MPH_HEIGHT] = f.Read8();
	for( int i = 0; i < width*length*256; i++ ) data_h[i].params[MPH_OVERLAY] = f.Read8(); 

	for( int param = 0; param < 3; param++ ) {
		for( int i = 0; i < width*length; i++ ) {
			data[i].params[param] = f.Read8();
		}
	}

	f.Close();

	if( !LoadZoneFormsCache() ) {
		ComputeZoneForms();
		SaveZoneFormsCache();
	}

	if( !LoadSeaLevelCache() ) {
		ComputeSeaLevel();
		SaveSeaLevelCache();
	}

	loaded = true;
}

//-------------------------------------------------------------------------------------------------
const map_param *WorldParameters::GetData() const {
	return data;
}

//-------------------------------------------------------------------------------------------------
const map_param_h *WorldParameters::GetDataH() const {
	return data_h;
}

//-------------------------------------------------------------------------------------------------
const map_param *WorldParameters::ReadData( int x, int y ) const {
	if( x < 0 || y < 0 || x >= width || y >= length ) return 0;
	return &data[ (x+y*width) ];
}

//-------------------------------------------------------------------------------------------------
const map_param_h *WorldParameters::ReadDataH( int xi, int yi, int x, int y ) const {
	if( xi < 0 || yi < 0 || xi >= width || yi >= length ) return 0;
	return &data_h[ (xi+yi*width)*256 + x + y*16 ];
}

}

