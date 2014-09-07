//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

Bitmap::Bitmap() {
	pixels = 0;
}

Bitmap::~Bitmap() {
	if( pixels ) {
		delete[] pixels;
	}
}

void Bitmap::load( const char *filename ) {
	
	if( pixels ) {
		delete[] pixels;
	}
	
	FILE *in;
	in = fopen( filename, "rb" );
	
	fread( &bmfh, sizeof( BITMAPFILEHEADER ), 1, in );
	fread( &bmih, sizeof( BITMAPINFOHEADER ), 1, in );
	
	DWORD size;
	size = bmfh.bfSize - bmfh.bfOffBits;
	BYTE *tempPixelData;
	tempPixelData = new BYTE[size];
	fread( tempPixelData, sizeof( BYTE ), size, in );
	fclose(in);

	    //byteWidth is the width of the actual image in bytes
    //padWidth is the width of the image plus the extra padding
    LONG byteWidth,padWidth;

    //initially set both to the width of the image
    byteWidth=padWidth=(LONG)((float)bmih.biWidth*(float)bmih.biBitCount/8.0);

	int bytesPerPixel;
	bytesPerPixel = bmih.biBitCount/8 ;

    //add any extra space to bring each line to a DWORD boundary
    while(padWidth%4!=0) {
       padWidth++;
    }

    DWORD diff;
    int offset;
    LONG height;

    height=bmih.biHeight;
    //set diff to the actual image size(no padding)
    diff=height*byteWidth;
    //allocate memory for the image
    pixels=new BYTE[diff];

    //bitmap is inverted, so the padding needs to be removed
    //and the image reversed
    //Here you can start from the back of the file or the front,
    //after the header.  The only problem is that some programs
    //will pad not only the data, but also the file size to
    //be divisible by 4 bytes.
    if(height>0) {

		for( int y = 0; y < height; y++ ) {
			for( int x = 0; x < byteWidth/bytesPerPixel; x++ ) {
				for( int b = 0; b < bytesPerPixel; b++ ) {
					//pixels[x*3+y*byteWidth+2] = tempPixelData[x*3+(height-y-1)*byteWidth+0];
					//pixels[x*3+y*byteWidth+1] =  tempPixelData[x*3+(height-y-1)*byteWidth+1];
					pixels[x*bytesPerPixel+y*byteWidth+b] =  tempPixelData[x*bytesPerPixel+(height-y-1)*byteWidth+b];
				}
			}
		}
        /*
		for( int i =0; i < diff; i += 3 ) {
			pixels[i+2] = tempPixelData[diff-3-i];
			pixels[i+1] = tempPixelData[diff-3-i+1];
			pixels[i+0] = tempPixelData[diff-3-i+2];
		}*/
    }
    //the image is not reversed.  Only the padding needs to be removed.
    else {
        height=height*-1;
        offset=0;
        do {
            memcpy((pixels+(offset*byteWidth)),
                   (tempPixelData+(offset*padWidth)),
                    byteWidth);
            offset++;
        } while(offset<height);
    }

	delete[] tempPixelData;
}


int Bitmap::width() const {
	return bmih.biWidth;
}

int Bitmap::height() const {
	return bmih.biHeight;
}

tBitmapPixel Bitmap::sample( int x, int y ) {
	BitmapPixel result;
	result.r = result.g = result.b = 0;
	if( !pixels ) return result;
	if( x < 0 || y < 0 || x >= width() || y >= height() ) return result;

	int bytesPerPixel = bmih.biBitCount>>3;

	result.b = pixels[(x + y * width()) * bytesPerPixel + 0];
	result.g = pixels[(x + y * width()) * bytesPerPixel + 1];
	result.r = pixels[(x + y * width()) * bytesPerPixel + 2];
	if( bytesPerPixel == 4 ) 
		result.a = pixels[(x + y * width()) * bytesPerPixel + 3];
	return result;
}
