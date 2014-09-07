//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#include "stdafx.h"

//-------------------------------------------------------------------------------------------------
namespace Audio {
	
extern TransferQueue<Sample> sample_garbage;

//-------------------------------------------------------------------------------------------------
Sample::Sample() {
	volume_scaler = 1.0;
	sampling_rate = 44100;
	data = 0;
	Erase();
}

//-------------------------------------------------------------------------------------------------
Sample::~Sample() {
	Erase();
}

//-------------------------------------------------------------------------------------------------
void Sample::Erase() {
	if( data ) delete[] data;
	data = 0;
	length = 0;
	has_loop = false;
	loop_start = 0;
	loop_length = 0;
}

//-------------------------------------------------------------------------------------------------
void Sample::CreateEmpty( int p_length, bool initialize ) {
	Erase();
	length = p_length;
	data = new s16[length];
	if( initialize ) {
		for( int i = 0; i < length; i++ ) data[i] = 0;
	}
}

//-------------------------------------------------------------------------------------------------
void Sample::SetNormalSamplingRate( float rate ) {
	sampling_rate = rate;
}

//-------------------------------------------------------------------------------------------------
float Sample::GetNormalSamplingRate() const {
	return sampling_rate;
}

//-------------------------------------------------------------------------------------------------
void Sample::SetVolumeScaler( float vs ) {
	volume_scaler = vs;
}

//-------------------------------------------------------------------------------------------------
float Sample::GetVolumeScaler() const {
	return volume_scaler;
}

//-------------------------------------------------------------------------------------------------
void Sample::SetLoopRange( int p_start, int p_length ) {
	loop_start = p_start;
	loop_length = p_length;
}

//-------------------------------------------------------------------------------------------------
void Sample::EnableLoop( bool enabled ) {
	has_loop = enabled;
}

//-------------------------------------------------------------------------------------------------
bool Sample::HasLoop() const {
	return has_loop;
}

//-------------------------------------------------------------------------------------------------
bool Sample::Oneshot() const {
	return !HasLoop();
}

//-------------------------------------------------------------------------------------------------
void Sample::GetLoopRange( int &r_start, int &r_length ) const {
	r_start = loop_start;
	r_length = loop_length;
}

//-------------------------------------------------------------------------------------------------
int Sample::Length() const {
	return has_loop ? (loop_start+loop_length) : length;
}

//-------------------------------------------------------------------------------------------------
int Sample::LoopStart() const {
	return loop_start;
}

//-------------------------------------------------------------------------------------------------
int Sample::LoopLength() const {
	return loop_length;
}

//-------------------------------------------------------------------------------------------------
s16 *Sample::GetDataPointer() {
	return data;
}

//-------------------------------------------------------------------------------------------------
int Sample::Read( double position ) const {
	position = position < 0.0 ? 0.0 : position;
	position = position >= (double)length ? (double)length-1.0 : position;

	//int a1, a2, p;
	int a1, p = (int)position;
	a1 = data[p];
	//a2 = (p+1) == length ? (has_loop ? data::

	return a1;
}

//-------------------------------------------------------------------------------------------------
void Sample::Delete() {
	deleting = true;
	sample_garbage.Add( this );
}


//-------------------------------------------------------------------------------------------------
bool Sample::CreateFromWAV( const char *filename ) {
	Erase();

	BinaryFile file;
	if( !file.Open( filename, BinaryFile::MODE_READ ) ) {
		return false;
	}
	
	// CHECK RIFF
	{
		char riff[5];
		riff[4] = 0;
		file.ReadBytes( (u8*)riff, 4 );
		if( riff[0] != 'R' || riff[1] != 'I' || riff[2] != 'F' || riff[3] != 'F' ) {
			return false;
		}
	}

	u32 filesize = file.Read32();

	
	{// CHECK WAVE   
		char wave[4];
		file.ReadBytes( (u8*)wave, 4 ); // WAVE

		if (wave[0]!='W' || wave[1]!='A' || wave[2]!='V' || wave[3]!='E') {
			return false;
		}
	}

	bool format_found=false;
	bool sample_complete=false;
	int format_bits=0;
	int format_channels=0;
	int format_freq=0;
  
	while (!file.Eof()) {

		char chunkID[4];
		file.ReadBytes( (u8*)chunkID, 4 );
		u32 chunksize = file.Read32();
		u32 file_position = file.Tell();

		if( file.Eof() ) {
			if( sample_complete ) {
				break;
			}
			Erase();
			return false;
			
		}
         
		if( chunkID[0] == 'f' && chunkID[1] == 'm' && chunkID[2] == 't' && chunkID[3] == ' ' && !format_found ) {
			// format chunk

			u16 compression_code = file.Read16();
			if( compression_code != 1 ) {
				Erase();
				return false;
			}

			format_channels = file.Read16();
			if( format_channels != 1 && format_channels != 2 ) {
				Erase();
				return false;
			}

			format_freq = file.Read32();
			file.Read32(); // skip average bits/second
			file.Read16(); // skip nblockalign

			format_bits = file.Read16();
			if( format_bits % 8 ) {
				Erase();
				return false;
			}
			format_found = true;

		} else if (chunkID[0]=='d' && chunkID[1]=='a' && chunkID[2]=='t' && chunkID[3]=='a') {
			// data chunk

			if( !format_found ) {
				Erase(); // data chunk before format chunk
				return false;
			}

			int frames = chunksize;
			frames /= format_channels;
			frames /= (format_bits>>3);

			
			CreateEmpty( frames, false );
			sampling_rate = (float)format_freq;
			
			if( format_bits == 8 ) {
				// 8 bits are UNSIGNED

				u8 *samples = new u8[length];
				file.ReadBytes( samples, length );

				// convert u8 -> s16
				for( int i = 0; i < length; i++ ) {
					// how2 convert 8->16..
					data[i] = ((int)samples[i] - 128) << 8;
				}
			} else {
				// 16 bits are SIGNED

				// read directly
				file.ReadBytes( (u8*)data, length*2 );

			}
            
			sample_complete = true;
            
        } else if( chunkID[0] == 's' && chunkID[1] == 'm' && chunkID[2] == 'p' && chunkID[3] == 'l' ) {
			// sampler chunk

			file.Read32(); // manufacturer
			file.Read32(); // prudoct
			file.Read32(); //  period
			file.Read32(); // midi note
			file.Read32(); // midi pitch frac
			file.Read32(); //  smpte format
			file.Read32(); //  smpte offset
			int nloops = file.Read32(); // num loops
			file.Read32(); // sampler data size

			if(nloops) {
				// use first loop
				file.Read32(); // 
				int type = file.Read32();
				int start = file.Read32();
				int end = file.Read32();
				int fraction = file.Read32();
				file.Read32(); // play count

				if( type == 0 ) {
					// forward loop
					SetLoopRange( start, end-start );
					EnableLoop(true);
					
				} else {
					// unsupported :(
					// todo: bidi unroll, reverse unroll
				}
				
			}
		}

		file.Seek( file_position+chunksize );
	}
     
	return sample_complete;
	
}

//-------------------------------------------------------------------------------------------------

}

