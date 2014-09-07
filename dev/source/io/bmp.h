//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#ifndef BITMAP_H
#define BITMAP_H

/*
typedef struct tagBITMAPINFOHEADER{
  DWORD  biSize; 
  LONG   biWidth; 
  LONG   biHeight; 
  WORD   biPlanes; 
  WORD   biBitCount; 
  DWORD  biCompression; 
  DWORD  biSizeImage; 
  LONG   biXPelsPerMeter; 
  LONG   biYPelsPerMeter; 
  DWORD  biClrUsed; 
  DWORD  biClrImportant; 
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

//Colour palette
typedef struct tagRGBQUAD {
  BYTE    rgbBlue; 
  BYTE    rgbGreen; 
  BYTE    rgbRed; 
  BYTE    rgbReserved; 
} RGBQUAD;
*/

typedef struct tBitmapPixel {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} BitmapPixel;

class Bitmap {
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;

public:
	BYTE *pixels;
	
	void load(const char*);
	int width() const ;
	int height() const ;
	Bitmap();
	~Bitmap();
	 
	tBitmapPixel sample( int x, int y );

};

#endif
