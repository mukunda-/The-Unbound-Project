//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"



namespace World {

namespace Disk {

// cell data offsets:
enum {
	CD_CDATA_TOTALSIZE = 72,
	CD_ITEMLIST_TOTALSIZE = 76,
	CD_ENTLIST_TOTALSIZE = 80,
	CD_HEADER_END = 84,
};

enum {
	MAX_COMPRESSED_CUBES_SIZE = 4096*2
};

	/*
namespace Codec {

// todo: optimization:
//    have a termination byte at the end of input arrays, to eliminate input_size tests within the loop

//------------------------------------------------------------------------------------------------------------------------------------
int PackRLE1( boost::uint8_t *input, boost::uint8_t *output, int input_size, bool *error ) {
	if( error ) *error = true;

	int read = 0;
	boost::uint8_t *pout = output;

	while( read < input_size ) {
		int byte = input[read];
		int count;
		for( count = 1; count < 128; count++ ) {
			if( read+count >= input_size ) break;
			if( input[read+count] != byte ) break;
		}

		if( count == 1 ) {
			if( byte & 128 ) *pout++ = 128;
			*pout++ = byte;

			// (todo: debug insert overflow checks)
		} else {
			*pout++ = 128 + count-1;
			*pout++ = byte;

			// (todo: debug insert overflow checks)
		}
		read += count;
	}
	if( error ) *error = false;
		
	return (int)(pout - output);
}

//------------------------------------------------------------------------------------------------------------------------------------
int PackRLE2( boost::uint16_t *input, boost::uint8_t *output, int input_size, bool *error ) {

	if( error ) *error = true;

	int read = 0;
	boost::uint8_t *pout = output;

	while( read < input_size ) {
		int word = input[read];
		int count;
		for( count = 1; count < 128; count++ ) {
			if( read+count >= input_size ) break;
			if( input[read+count] != word ) break;
		}

		if( count == 1 ) {
			*pout++ = word>>8; // bit15 of word is unused and should be 0 (can mask above to make sure)
			*pout++ = word&255;

			// (todo: debug insert overflow checks)
		} else {
			*pout++ = 128 + count-1;
			*pout++ = word>>8; // big endian for concurrency with above
			*pout++ = word&255;

			// (todo: debug insert overflow checks)
		}
		read += count;
	}

	if( error ) *error = false;

	return (int)(pout - output);
}

//------------------------------------------------------------------------------------------------------------------------------------
int UnpackRLE1( boost::uint8_t *input, boost::uint8_t *output, int input_size, int max_output_size, bool *error ) {

	if( error ) *error = true;
	int read = 0;
	int write = 0;

	while( read != input_size ) {
		int byte = input[read++];
		if( byte & 128 ) {
			if( read == input_size ) return 0; // error
			int word = input[read++];
				
			byte = (byte & 127) + 1;
			for( int i = 0; i < byte; i++ ) {
				if( write == max_output_size ) return 0;
				output[write++] = word;
			}
		} else {
			if( write == max_output_size ) return 0;
			output[write++] = byte;
			
		}
	}

	if( error ) *error = false;

	return write;
}

//------------------------------------------------------------------------------------------------------------------------------------
int UnpackRLE2( boost::uint8_t *input, boost::uint16_t *output, int input_size, int max_output_size, bool *error ) {
		
	if( error ) *error = true;
	int read = 0;
	int write = 0;

	while( read != input_size ) {
		int byte = input[read++];
		if( byte & 128 ) {
			if( read == input_size ) return 0; // error/underflow
			int word = input[read++] << 8;
			if( read == input_size ) return 0; // error/underflow
			word |= input[read++];
			byte = (byte & 127) + 1;
			for( int i = 0; i < byte; i++ ) {
				if( write == max_output_size ) return 0; // error/overflow
				output[write++] = word;
			}
		} else {
			int word = byte<<8;
			if( read == input_size ) return 0; // error/underflow
			word |= input[read++];
			if( write == max_output_size ) return 0; // error/overflow
			output[write++] = byte;
		}
	}

	if( error ) *error = false;
	return write;
}

}*/

//------------------------------------------------------------------------------------------------------------------------------------
void PushItem( Context *ct, int index, ItemHeader *data ) {
	//todo
}

//------------------------------------------------------------------------------------------------------------------------------------
void PushItems( Context *ct, int index, ItemHeader *first, ItemHeader *last ) {
	//todo
}

//------------------------------------------------------------------------------------------------------------------------------------
void PopItems( Context *ct, int index, ItemHeader **first, ItemHeader **last ) {
	//todo
}

//------------------------------------------------------------------------------------------------------------------------------------
void ProcessItems( Context *ct, int index, ProcessItemsFunction func ) {
	//todo
}

//------------------------------------------------------------------------------------------------------------------------------------
void PushEntity( Context *ct, int index, EntityHeader *ent ) {
	//todo
}

//------------------------------------------------------------------------------------------------------------------------------------
void PushEntities( Context *ct, int index, EntityHeader *first, EntityHeader *last ) {
	//todo
}

//------------------------------------------------------------------------------------------------------------------------------------
void PopEntities( Context *ct, int index, EntityHeader **first, EntityHeader **last ) {
	//todo
}

//------------------------------------------------------------------------------------------------------------------------------------
void ProcessEntities( Context *ct, int index, void *data, ProcessEntitiesFunction func ) {
	//todo
}

//------------------------------------------------------------------------------------------------------------------------------------
bool LoadCubeData( Cell *cell, BinaryFile &f, int data_size ) {
	boost::uint8_t compressed_data[16*16*16*2];

	if( data_size > 16*16*16*2 ) {
		// crazy data
		return false;
	}

	f.ReadBytes( compressed_data, data_size );
	
	bool error;
	CellCodec::UnpackRLE2( compressed_data, data_size, cell->cubes, 16*16*16*2, &error );
	
	return !error;
}

//------------------------------------------------------------------------------------------------------------------------------------
Cell *ReadCellData( Context *ct, int index ) {

	boost::uint64_t region_index;
	int region_chunk_index;
	std::string filename;
	ct->GetRegionAddress( index, &region_index, &region_chunk_index );
	ct->GetRegionFilename( region_index, filename );
	//ct->GetChunkFilename( index, filename );

	// acquire file lock
	// seek to chunk
	// allocate chunk if existing isnt available
	// read masked data

	RegionFileLock lock( ct, region_index );
	//ct->AcquireFileLock( region_index );

	BinaryFile f( filename.c_str(), BinaryFile::MODE_READ );
	if( !f.IsOpen() ) {
		// file doesnt exist
		return 0;
	}
	f.Seek( region_chunk_index*4 );

	boost::uint32_t offset = f.Read32();
	if( offset == 0 ) {
		// chunk is not on disk
		return 0;
	}
	f.Seek( offset );

	Cell *cell = (Cell*)Memory::AllocMem( sizeof(Cell) );
	InitializeCellHeader(cell);
	cell->ct = ct;
		
	cell->owner = f.Read32();
	cell->group = f.Read32();
		
	int cube_data_size = f.Read32();

	for( int i = 0; i < COMP_TOTAL; i++ ) {
		cell->comp_sizes[i] = f.Read32();
	}
	
	/*
	int paint_data_size_total = 0;
	for( int i = 0; i < 6; i++ )
		paint_data_size_total += paint_data_size[i];
	int decal_data_size_total = 0;
	for( int i = 0; i < 6; i++ )
		decal_data_size_total += decal_data_size[i];
	*/
	int comp_data_size = f.Read32();
	int item_data_size = f.Read32();
	int ent_data_size = f.Read32();

	// load cube data
	if( !LoadCubeData( cell, f, cube_data_size ) ) {
		// an error occurred
		//todo
	}

	cell->compressed = (boost::uint8_t*)Memory::AllocMem( comp_data_size );
	f.ReadBytes( cell->compressed, comp_data_size );
		/*
		if( damage ) {
			comp_data_size += damage_data_size;
		} else {
			if( cell->data_loaded[CDATA_DAMAGE] ) {
				comp_data_size += chunk->comp_sizes[COMP_DAMAGE];
			}
		}
		if( paint ) {
			comp_data_size += paint_data_size_total;
		} else {
			if( cell->data_loaded[CDATA_PAINT] ) {
				for( int i = 0; i < 6; i++ ) {
					comp_data_size += chunk->comp_sizes[COMP_PAINT1+i];
				}
			}
		}
		if( decal ) {
			comp_data_size += decal_data_size_total;
		} else {
			if( chunk->data_loaded[CDATA_DECAL] ) {
				for( int i = 0; i < 6; i++ ) {
					comp_data_size += chunk->comp_sizes[COMP_DECAL1+i];
				}
			}
		}

		boost::uint8_t *cdata = (boost::uint8_t*)Memory::AllocMem( comp_data_size );
		boost::uint8_t *cdata_write = cdata;
		boost::uint8_t *existing_compressed_read = chunk->compressed;

		if( damage ) {
			// load damage data
			if( damage_data_size != 0 )
				f.ReadBytes( cdata_write, damage_data_size );

			cdata_write += damage_data_size;

			// skip existing data
			if( chunk->data_loaded[CDATA_DAMAGE] ) {
				existing_compressed_read += chunk->comp_sizes[COMP_DAMAGE];
			}

			chunk->comp_sizes[COMP_DAMAGE] = damage_data_size;
			chunk->data_loaded[CDATA_DAMAGE] = 1;
		} else {
			if( damage_data_size )
				f.SeekCur( damage_data_size );

			// copy existing damage data
			if( chunk->data_loaded[CDATA_DAMAGE] ) {
				if( chunk->comp_sizes[COMP_DAMAGE] != 0 ) {
					memcpy( cdata_write, existing_compressed_read, chunk->comp_sizes[COMP_DAMAGE] );
					cdata_write += chunk->comp_sizes[COMP_DAMAGE];
					existing_compressed_read += chunk->comp_sizes[COMP_DAMAGE];
				}
			}
		}

		if( paint ) {
			// load paint data
			if( paint_data_size_total != 0 )
				f.ReadBytes( cdata_write, paint_data_size_total );
			cdata_write += paint_data_size_total;

			// skip existing data
			if( chunk->data_loaded[CDATA_PAINT] ) {
				for( int i = 0; i < 6; i++ )
					existing_compressed_read += chunk->comp_sizes[COMP_PAINT1+i];
			}

			chunk->data_loaded[CDATA_PAINT] = 1;
			for( int i = 0; i < 6; i++ )
				chunk->comp_sizes[COMP_PAINT1+i] = paint_data_size[i];
				
		} else {
			if( paint_data_size_total != 0 )
				f.SeekCur( paint_data_size_total );
				
			if( chunk->data_loaded[CDATA_PAINT] ) {
				int existing_paint_data_total = 0;
				for( int i = 0; i < 6; i++ )
					existing_paint_data_total += chunk->comp_sizes[COMP_PAINT1+i];
				if( existing_paint_data_total != 0 )
					memcpy( cdata_write, existing_compressed_read, existing_paint_data_total );

				cdata_write += existing_paint_data_total;
				existing_compressed_read += existing_paint_data_total;
			}
		}

		if( decal ) {
			// load decal data
			if( decal_data_size_total != 0 )
				f.ReadBytes( cdata_write, decal_data_size_total );
			cdata_write += decal_data_size_total;

			if( chunk->data_loaded[CDATA_DECAL] ) {
				for( int i = 0; i < 6; i++ )
					existing_compressed_read += chunk->comp_sizes[COMP_DECAL1+i];
			}

			chunk->data_loaded[CDATA_DECAL] = 1;
			for( int i = 0; i < 6; i++ ) 
				chunk->comp_sizes[COMP_DECAL1+i] = decal_data_size[i];

				
		} else {
			
			if( decal_data_size_total != 0 )
				f.SeekCur( decal_data_size_total );

			if( chunk->data_loaded[CDATA_DECAL] ) {
				int existing_decal_data_total = 0;
				for( int i = 0; i < 6; i++ )
					existing_decal_data_total += chunk->comp_sizes[COMP_DECAL1+i];
				if( existing_decal_data_total != 0 )
					memcpy( cdata_write, existing_compressed_read, chunk->comp_sizes[COMP_DAMAGE] );
				cdata_write += existing_decal_data_total;
				existing_compressed_read += existing_decal_data_total;
			}
		}

		if( chunk->compressed ) Memory::Free( chunk->compressed );
		chunk->compressed = cdata;
	}
	*/
	f.Close();

	return cell;
}

//------------------------------------------------------------------------------------------------------------------------------------
void OutputCellData( BinaryFile &f_out, Cell *data, int region_chunk_index, int items_totalsize, int ents_totalsize ) {
	boost::uint8_t compressed_cubes[MAX_COMPRESSED_CUBES_SIZE];
	bool codec_error;

	int cube_data_size = CellCodec::PackRLE2( data->cubes,  16*16*16*2, compressed_cubes );

//	if( codec_error ) {
		// an error happened :(
		// (todo)
//		assert( !codec_error );
//	}

	int total_comp_size = 0;
	total_comp_size += cube_data_size;
	
	for( int i = 0; i < COMP_TOTAL; i++ ) {
		total_comp_size += data->comp_sizes[i];
	}

	f_out.Write32( CD_HEADER_END + total_comp_size + items_totalsize + ents_totalsize );
	f_out.Write32( region_chunk_index );
	f_out.Write32( data->owner );
	f_out.Write32( data->group );
	f_out.Write32( cube_data_size );
	for( int i = 0; i < COMP_TOTAL; i++ ) {
		f_out.Write32( data->comp_sizes[i] );
	}
	 
	f_out.Write32( total_comp_size );
	f_out.Write32( items_totalsize );
	f_out.Write32( ents_totalsize );

	f_out.WriteBytes( compressed_cubes, cube_data_size );
	f_out.WriteBytes( data->compressed, total_comp_size );

}

//------------------------------------------------------------------------------------------------------------------------------------
void SaveCellData( Context *ct, int index, Cell *data ) {
	
	boost::uint64_t region_index;
	int region_chunk_index;
	std::string filename;
	std::string filename2;
	ct->GetRegionAddress( index, &region_index, &region_chunk_index );
	ct->GetRegionFilename( index, filename );
 
	filename2 = filename;
	filename2[filename2.length()-1] = 't'; // .rgt

	RegionFileLock lock( ct, region_index );

	bool new_file = false;
	
	BinaryFile f_in( filename.c_str(), BinaryFile::MODE_READ );
	if( !f_in.IsOpen() ) {
		new_file = true;
	}
	BinaryFile f_out( filename2.c_str(), BinaryFile::MODE_WRITE );

	boost::uint32_t offset_table[4*4*4];

	if( new_file) {
		
		for( int i = 0; i < 4*4*4; i++ ) {
			if( i == region_chunk_index ) {
				f_out.Write32( 4*4*4 *4 );
			} else {
				f_out.Write32( 0 );
			}
		}

		OutputCellData( f_out, data, region_chunk_index, 0, 0 );

		f_out.Close();
		rename( filename2.c_str(), filename.c_str() );

	} else {

	
		for( int i = 0; i < 4*4; i++ ) {
			offset_table[i] = 0;
			f_out.Write32( 0 );
		}

		boost::uint32_t offset = 0;
		boost::uint32_t our_offset = 0;
		f_in.Seek( 4*4*4 * 4 );

		boost::uint32_t size, d_index;

		bool new_data_outputted = false;
	
		while( !f_in.Eof() ) {
			size = f_in.Read32();
			d_index = f_in.Read32();

			if( d_index != region_chunk_index ) {
				// copy to output
				offset_table[d_index] = f_out.Tell();
				f_out.Write32( size );
				f_out.Write32( d_index );
				f_out.Stream( f_in, size - 8 );
			} else {
				// output our data

				offset_table[region_chunk_index] = f_out.Tell();
				f_in.SeekCur( CD_CDATA_TOTALSIZE - 8 );
				int cdata_totalsize = f_in.Read32();
				int items_totalsize = f_in.Read32();
				int ents_totalsize = f_in.Read32();
				f_in.SeekCur( cdata_totalsize );
				// f_in is at itemlist&entitylist position

				OutputCellData( f_out, data, region_chunk_index, items_totalsize, ents_totalsize );

				// stream existing data entries
				f_out.Stream( f_in, items_totalsize );
				f_out.Stream( f_in, ents_totalsize );

				new_data_outputted = true;
			}

		}

		if( !new_data_outputted ) {
			// this chunk has no existing data
			offset_table[region_chunk_index] = f_out.Tell();
			OutputCellData( f_out, data, region_chunk_index, 0, 0 );
		}
	
		f_out.Seek(0);
		for( int i = 0; i < 4*4*4; i++ ) {
			f_out.Write32( offset_table[i] );
		}

		f_out.Close();
		remove( filename.c_str() );
		rename( filename2.c_str(), filename.c_str() );
	}

	
}

}
}

