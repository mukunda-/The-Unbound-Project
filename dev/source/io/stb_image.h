//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"

class StbImage {

private:

	int vwidth, vheight;
	int vcomps;

	unsigned char *vdata;
public:
	
	StbImage( const char *filename ) {
		vdata = 0;
		vdata = stbi_load( filename, &vwidth, &vheight, &vcomps, 0 );
	}

	~StbImage() {
		if( vdata ) free( vdata );
	}

	bool Valid() const { return !!vdata; }
	unsigned char *Data() const { return vdata; }
	int Width() const { return vwidth; }
	int Height() const { return vheight; }
	int Components() const { return vcomps; }
};
