//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

/// simple binary file class

#pragma once

#include <string>
#include <boost/cstdint.hpp>

//-------------------------------------------------------------------------------------------------
class BinaryFile {

private:
	FILE *f;

	bool opened;
	int filesize;

public:
	BinaryFile();
	BinaryFile( const char *file, int mode );
	~BinaryFile();
	bool Open( const char *file, int mode );
	bool Close();
	bool IsOpen();

	boost::uint8_t Read8();
	boost::uint16_t Read16();
	boost::uint32_t Read24();
	boost::uint32_t Read32();

	void ReadString( std::string &target, int length );
	void ReadBytes( boost::uint8_t *dest, int length );

	void Write8(boost::uint8_t);
	void Write16(boost::uint16_t);
	void Write24(boost::uint32_t);
	void Write32(boost::uint32_t);
	void WriteBytes(boost::uint8_t *src, int length);
	void Stream( BinaryFile &source, boost::uint32_t size );

	void Seek(int);
	void SeekCur(int);
	void SeekEnd(int);

	int Tell();

	int Size();

	bool Eof();
	
	enum {
		MODE_READ,
		MODE_WRITE,
	};
};

