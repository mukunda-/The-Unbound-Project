//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// simple file i/o class

#include "stdafx.h"
#include "io/BinaryFile.h"
#include "util/fopen2.h"

//-------------------------------------------------------------------------------------------------
BinaryFile::BinaryFile() {
	opened = false;
}

//-------------------------------------------------------------------------------------------------
BinaryFile::BinaryFile( const char *file, int mode ) {
	opened = false;
	Open( file, mode );
}

//-------------------------------------------------------------------------------------------------
BinaryFile::~BinaryFile() {
	Close();
}

//-------------------------------------------------------------------------------------------------
bool BinaryFile::IsOpen() {
	return opened;
}

//-------------------------------------------------------------------------------------------------
bool BinaryFile::Close() {
	if( opened ) {
		fclose(f);
		opened = false;
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
bool BinaryFile::Open( const char *file, int mode ) {
	if( opened ) Close();

	if( mode == MODE_READ ) {
		f = fopen2(file, "rb");
	} else {
		f = fopen2( file, "wb" );
	}

	opened = f != 0;

	if( opened ) {
		SeekEnd(0);
		filesize = Tell();
		Seek(0);
	}

	return opened;
}

//-------------------------------------------------------------------------------------------------
boost::uint8_t BinaryFile::Read8() {
	boost::uint8_t data;
	fread( &data, 1, 1, f );
	return data;
}

//-------------------------------------------------------------------------------------------------
boost::uint16_t BinaryFile::Read16() {
	boost::uint16_t data;
	data = Read8();
	data |= Read8() << 8;
	return data;
}

//-------------------------------------------------------------------------------------------------
boost::uint32_t BinaryFile::Read24() {
	boost::uint32_t data;
	data = Read16();
	data |= Read8() << 16;
	return data;
}

//-------------------------------------------------------------------------------------------------
boost::uint32_t BinaryFile::Read32() {
	boost::uint32_t data;
	data = Read16();
	data |= Read16() << 16;
	return data;
}

//-------------------------------------------------------------------------------------------------
void BinaryFile::ReadString( std::string &target, int length ) {
	target.clear();
	bool skip=false;
	for( int i = 0; i < length; i++ ) {
		char c = (char)Read8();
		if(   c && !skip  ) {
			target += c;
		} else {
			skip=true;
			
		}
	}
}

//-------------------------------------------------------------------------------------------------
void BinaryFile::ReadBytes( boost::uint8_t *dest, int length ) {
	fread( dest, 1, length, f );
}

//-------------------------------------------------------------------------------------------------
void BinaryFile::Write8( boost::uint8_t data ) {
	fwrite( &data, 1, 1, f );
}

//-------------------------------------------------------------------------------------------------
void BinaryFile::Write16( boost::uint16_t data ) {
	Write8((boost::uint8_t)data);
	Write8((boost::uint8_t)(data>>8));
}

//-------------------------------------------------------------------------------------------------
void BinaryFile::Write24( boost::uint32_t data ) {
	Write8((boost::uint8_t)data);
	Write8((boost::uint8_t)(data>>8));
	Write8((boost::uint8_t)(data>>16));
}

//-------------------------------------------------------------------------------------------------
void BinaryFile::Write32( boost::uint32_t data ) {
	Write16((boost::uint8_t)data);
	Write16((boost::uint8_t)(data>>16));
}

//-------------------------------------------------------------------------------------------------
void BinaryFile::WriteBytes( boost::uint8_t *src, int length ) {
	fwrite( src, 1, length, f );
}

//-------------------------------------------------------------------------------------------------
void BinaryFile::Stream( BinaryFile &source, boost::uint32_t size ) {
	boost::uint8_t buffer[32768];

	while( size >= 32768 ) {
		source.ReadBytes( buffer, 32768 );
		WriteBytes( buffer, 32768 );
		size -= 32768;
	}

	if( size == 0 ) return;

	source.ReadBytes( buffer, size );
	WriteBytes( buffer, size );
}

//-------------------------------------------------------------------------------------------------
void BinaryFile::Seek( int position ) {
	fseek(f, position, SEEK_SET);
}

//-------------------------------------------------------------------------------------------------
void BinaryFile::SeekCur( int position ) {
	fseek( f, position, SEEK_CUR );
}

//-------------------------------------------------------------------------------------------------
void BinaryFile::SeekEnd( int position ) {
	fseek( f, position, SEEK_END );
}

//-------------------------------------------------------------------------------------------------
int BinaryFile::Tell() {
	return ftell(f);
}

//-------------------------------------------------------------------------------------------------
int BinaryFile::Size() {
	return filesize;
}

//-------------------------------------------------------------------------------------------------
bool BinaryFile::Eof() {
	return !!feof(f);
}
