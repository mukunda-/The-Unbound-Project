//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

namespace World {

enum {
	DAMAGE_MAX=255
};

//-------------------------------------------------------------------------------------------------
void Context::AddActiveCommand( Command *cmd ) {

	cmd->ac_next = 0;


	boost::lock_guard<boost::mutex> lock(active_command_mutex);
	active_commands_total++;

	if( !last_active_command ) {
		first_active_command = last_active_command = cmd;
		cmd->ac_prev = 0;
	} else {
		last_active_command->ac_next = cmd;
		cmd->ac_prev = last_active_command;
		last_active_command = cmd;
	}
}

//-------------------------------------------------------------------------------------------------
void Context::RemoveActiveCommand( Command *cmd ) {

	boost::lock_guard<boost::mutex> lock(active_command_mutex);

	active_commands_total--;

	if( cmd->ac_next )
		cmd->ac_next->ac_prev = cmd->ac_prev;
	else
		last_active_command = cmd->ac_prev;

	if( cmd->ac_prev )
		cmd->ac_prev->ac_next = cmd->ac_next;
	else
		first_active_command = cmd->ac_next;


}

//-------------------------------------------------------------------------------------------------
bool BoundingBoxTest( BoundingBox &a, BoundingBox &b ) {
	int d;
	for( int i = 0; i < 3; i++ ) {
		d = b.position[i] - a.position[i];
		if( d < 0 ) d = -d;


		if( d >= (a.size[i] + b.size[i]) ) {
			return false;
		}
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
bool Context::CommandBoxTest( Command &a, Command &b ) {
	
	if( BoundingBoxTest( a.area.box, b.area.box ) ) return true;
	if( b.area.use_box2 ) {
		if( BoundingBoxTest( a.area.box, b.area.box2 ) ) return true;
	}
	if( a.area.use_box2 ) {
		if( BoundingBoxTest( a.area.box2, b.area.box ) ) return true;
		if( b.area.use_box2 ) {
			if( BoundingBoxTest( a.area.box2, b.area.box2 ) ) return true;
		}
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
void Context::CommandDispatcherThread() {
	// fill command buffer

	while( true ) {

		ew_dispatcher.Wait();

		while( active_commands_total < active_commands_quota ) {
			Command *cmd = commands.Pop();
			if( !cmd ) break;

			AddActiveCommand( cmd );
		}

		{
			boost::lock_guard<boost::mutex> lock(active_command_mutex);
			for( Command *cmd = first_active_command; cmd; cmd = cmd->ac_next ) {
				if( cmd->executing ) continue;
			
				bool execute = true;
				for( Command *c2 = cmd->ac_prev; c2; c2 = c2->ac_prev ) {
					if( CommandBoxTest( *cmd, *c2 ) ) {
						execute = false;
						break;
					}
				}

				if( execute ) {
					cmd->executing = 1;
					command_execute_list.Push( cmd );
				}
			}
		}

		if( shutting_down_command_execution ) {
			break;
		}

	}

}

//-------------------------------------------------------------------------------------------------
void Context::CommandExecutionThread() {

	while( true ) {
		command_execute_list.WaitForCommand();
		
		Command *cmd = command_execute_list.Pop();
		if( cmd ) {
			ProcessCommand( cmd );
		} else {
			if( shutting_down_command_execution ) {
				break;
			}
		}
	}

}

//-------------------------------------------------------------------------------------------------
void Context::StartCommandExecutionThreads( int count ) {
	num_command_execution_threads = count;
	
	command_execution_threads = new boost::thread[count];

	InterlockedExchange( &shutting_down_command_execution, 0 );

	for( int i = 0; i < count; i++ ) {
		command_execution_threads[i] = boost::thread( boost::bind( &Context::CommandExecutionThread, this ) );
	}
}

//-------------------------------------------------------------------------------------------------
void Context::StopCommandExecutionThreads() {
	InterlockedExchange( &shutting_down_command_execution, 1 );

	//new_commands_added.notify_all();
	command_execute_list.DisableWaits();

	for( int i = 0; i < num_command_execution_threads; i++ ) {
		command_execution_threads[i].join();
	}

}

//-------------------------------------------------------------------------------------------------
//chunk_state Context::GetChunkState( int x, int y, int z ) {
//	return chunks.GetState( x, y, z );
//}

//-------------------------------------------------------------------------------------------------
//c//hunk_state Context::GetChunkState( int index ) {
//	return chunks.GetState( index );
//}

//-------------------------------------------------------------------------------------------------
//Chunk *Context::GetChunk( int x, int y, int z ) {
//	return chunks.GetAddress( x, y, z );
//}

//-------------------------------------------------------------------------------------------------
//Chunk *Context::GetChunk( int index ) {
//	return chunks.GetAddress( index );
//}

//-------------------------------------------------------------------------------------------------
//int Context::GetWorldIndex( int x, int y, int z ) {
//	return chunks.IndexFromCoords( x, y, z );
//}

//void Context::GetWorldIndexCoords( int world_index, int &x, int &y, int &z ) {
//	x = world_index & (world_width-1);
//	z = (world_index / world_width) & (world_length-1);
//	y = world_index / (world_width*world_length);
//}

//-------------------------------------------------------------------------------------------------
void Context::GetRegionAddress( boost::uint64_t world_index, boost::uint64_t *region_index, int *index_within_region ) {
	int x,y,z;
	UnpackCoords( world_index, x, y, z );
	//GetWorldIndexCoords( world_index, x, y, z );

	// note: hardcoded values here

	if( index_within_region ) {
		*index_within_region = (x&3) + (y&3)*16 + (z&3)*4;
	}

	if( region_index ) {
		*region_index = 
			((boost::uint64_t)(x>>2))+
			((boost::uint64_t)(y>>2)<<32)+
			((boost::uint64_t)(z>>2)<<16);
	}
}

//-------------------------------------------------------------------------------------------------
char hexchar( int a ) {
	if( a < 10 ) {
		return '0' + a;
	} else {
		return 'a' + a;
	}
}

//-------------------------------------------------------------------------------------------------
void Context::GetRegionFilename( boost::uint64_t world_index, std::string &output ) {

	int x, y, z;
	UnpackCoords( world_index, x, y, z );
	// parse coordinates
	
	//GetWorldIndexCoords( world_index, x, y, z );
	x >>= 2;
	y >>= 2;
	z >>= 2;

	char digits_x[8];
	char digits_y[8];
	char digits_z[8];

	for( int i = 0; i < 7; i++ ) {	// max 16384x16384x1024 regions
		digits_x[i] = '0' + (x & 3);
		digits_z[i] = '0' + (z & 3);
		digits_y[i] = '0' + (y & 3);
		x >>= 2;
		y >>= 2;
		z >>= 2;
	}
	
	output = datapath;
	for( int i = 6; i >= 0; i-- ) {
		output += "/";
		output += digits_x[i];
		output += digits_z[i];
		output += digits_y[i];
	}
	output += ".rgn";
}

/*
//-------------------------------------------------------------------------------------------------
void Context::GetChunkFilename( int world_index, std::string &output ) {

	int cx,cy,cz;
	GetWorldIndexCoords( world_index, cx, cy, cz );

	char digits_x[8];
	char digits_y[8];
	char digits_z[8];

	for( int i = 0; i < 5; i++ ) {	// max 1024x1024x1024
		digits_x[i] = 'a' + (cx & 3);
		digits_y[i] = 'a' + (cy & 3);
		digits_z[i] = 'a' + (cz & 3);
		cx >>= 2;
		cy >>= 2;
		cz >>= 2;
	}
	
	output = datapath;
	for( int i = 4; i >= 0; i-- ) {
		output += "/";
		output += digits_x[i];
		output += digits_y[i];
		output += digits_z[i];
	}
	output += ".chk";
}
*/

//-------------------------------------------------------------------------------------------------
float Context::GetSeaLevel() const {
	return (float)sea_level;
}

/*
//-------------------------------------------------------------------------------------------------
void Context::RefreshChunk( boost::uint64_t world_index ) {
	Chunk *ch;
	int state;

	Chunk *ch = GetChunk( world_index );
	if( ch ) {
		engine->GetChunkManager()->Bump( ch );
	}
}
*/
//-------------------------------------------------------------------------------------------------
int Context::GetWidth() const {
	return world_width;
}

//-------------------------------------------------------------------------------------------------
int Context::GetDepth() const {
	return world_depth;
}

//-------------------------------------------------------------------------------------------------
int Context::GetLength() const {
	return world_length;
}

//-------------------------------------------------------------------------------------------------
bool Context::CoordsInChunkRange( int x, int y, int z ) {
	return (x >= 0 && y >= 0 && z >= 0 && x < world_width && y < world_depth && z < world_length );
}

//-------------------------------------------------------------------------------------------------
bool Context::CoordsInCubeRange( int x, int y, int z ) {
	return (x >= 0 && y >= 0 && z >= 0 && x < world_width*16 && y < world_depth*16 && z < world_length*16 );
}

//-------------------------------------------------------------------------------------------------
int Context::GetCube( int x, int y, int z ) {
	if( x < 0 || y < 0 || z < 0 || x >= world_width*16 || y >= world_depth*16 || z >= world_length*16 ) return 0;

	Cell *ch;
	int result, state;
	result = cell_tree->Acquire( x, y, z, &ch, &state, CellTree::FLAG_KNOWNONLY );

	//result = chunks->WaitAcquireActiveShared( x, y, z, &ch );
	if( result == CellTree::RESULT_SUCCESS ) {
		int cube;
		
		if( state & CELLSTATE_EMPTY ) {
			cube = 0;
		} else {
			cube = ch->cubes[ (x&15) + ((z&15)<<4) + ((y&15)<<8) ];
		}
		cell_tree->Release( x, y, z );
		//chunks->ReleaseShared( x, y, z );
		return cube;
	}
	return -1;
}
/*
//-------------------------------------------------------------------------------------------------
job *Context::NewJob() {
	job *j = Memory::Alloc<job>();
	j->ct = this;
	return j;
}
*/
/*
//-------------------------------------------------------------------------------------------------
void Context::PushGenerationJob( int world_index ) {
	job *j = NewJob();
	j->type = JOB_GENERATE;
	j->index = world_index;
	engine->GetJobs()->Push(j);
}
*/
/*
//-------------------------------------------------------------------------------------------------
void Context::PushDiskReadJob( int world_index ) {
	job *j = NewJob();
	j->type = JOB_LOAD;
	j->index = world_index;
	engine->GetJobs()->Push(j);
}
*/



//-------------------------------------------------------------------------------------------------
void Context::PushCreationJob( boost::uint64_t world_index, int unknown_state ) {
	Job *j = JobManager::CreateJob();
	j->type = JOB_CREATE;
	j->world_index = world_index;
	j->data[0] = unknown_state;
	engine->GetJobs()->Push(j);
}

//-------------------------------------------------------------------------------------------------
bool Context::Request( boost::uint64_t world_index ) {
	
	// quick check to see if request is redundant
	int original_state;
	int result = cell_tree->AcquireCreation( world_index, original_state );

	if( result == CellTree::RESULT_CREATION_ACTIVE ) {
		return false; // job in progress
	} else if( result == CellTree::RESULT_KNOWN ) {
		return true; // chunk is known
	}
	
	assert( result == CellTree::RESULT_SUCCESS ); // catch unknown error

	PushCreationJob( world_index, original_state );
	return false;
	/*
	switch( original_state ) {
	case CHUNKSTATE_UNKNOWN:
		// todo: for clients this is a request-from-server job
		// (JOB_REQUEST)
		PushCreationJob( world_index, original_state );
		break;
	case CHUNKSTATE_DORMANT:
		PushCreationJob( world_index, original_state );
		break;
	case CHUNKSTATE_DISK:
		PushCreationJob( world_index, original_state );
		break;
	}
	return false;

	int state = chunks->QueryState( world_index );
	if( state == CHUNKSTATE_READY ) return false;
	if( state == CHUNKSTATE_EMPTY ) return false;
	if( state == CHUNKSTATE_LOCKED ) return false;
	if( state == CHUNKSTATE_SHARED ) return false;
	
	if( !chunks->TryAcquireInactive( world_index, state ) ) {
		return false;
	}

	// state will only be inactive states with acquire
	switch( state ) {
	
	case CHUNKSTATE_UNKNOWN:
		// todo : for client this is a request-from-server job
		//(JOB_REQUEST)
		// for server this function must check the disk to see if this is a DORMANT or DISK chunk

		return true;
	case CHUNKSTATE_DORMANT:
		
		// add a generation job
		// client or server

		PushGenerationJob( world_index );

		return true;
	case CHUNKSTATE_DISK:
		// load from disk (todo)
		// this is a server side only job
		
		PushDiskReadJob( world_index );

		return true;
	}
	*/
	//return true;

	
}

//-------------------------------------------------------------------------------------------------
bool Context::Request( int x, int y, int z ) {
	return Request( PackCoords(x, y, z) );
}

//-------------------------------------------------------------------------------------------------
void Context::RequestWait( boost::uint64_t world_index ) {
	if( Request( world_index ) ) {
		return;
	}
	cell_tree->WaitCreation( world_index );
}

//-------------------------------------------------------------------------------------------------
void Context::RequestWait( int x, int y, int z ) {
	RequestWait( PackCoords( x, y, z ) );
}


//-------------------------------------------------------------------------------------------------
// function to rearrange box coordinates so that the first index is always lesser or equal to the second
static void arrange_box_coordinates( int &x1, int &y1, int &z1, int &x2, int &y2, int &z2 ) {
	if( x1 < x2 ) { int a = x2; x2 = x1; x1 = a; }
	if( y1 < y2 ) { int a = y2; y2 = y1; y1 = a; }
	if( z1 < z2 ) { int a = z2; z2 = z1; z1 = a; }
}

//-------------------------------------------------------------------------------------------------
void Context::RequestCubeBoxWaitEach( int x1, int y1, int z1, int x2, int y2, int z2 ) {
	arrange_box_coordinates( x1, y1, z1, x2, y2, z2 );
	cell_tree->WaitCreationArea( x1>>4, y1>>4, z1>>4, x2>>4, y2>>4, z2>>4 );
}

//-------------------------------------------------------------------------------------------------
void Context::RequestCubeBoxWaitEach( int *pos1, int *pos2 ) {
	arrange_box_coordinates( pos1[0], pos1[1], pos1[2], pos2[0], pos2[1], pos2[2] );
	cell_tree->WaitCreationArea( pos1, pos2 );
}

//-------------------------------------------------------------------------------------------------
bool Context::RequestCubeBox( int x1, int y1, int z1, int x2, int y2, int z2 ) {
	arrange_box_coordinates( x1, y1, z1, x2, y2, z2 );

	x1 >>= 4;
	y1 >>= 4;
	z1 >>= 4;
	x2 >>= 4;
	y2 >>= 4;
	z2 >>= 4;

	bool requested = false;

	for( int y = y1; y <= y2; y++ ) {
		for( int z = z1; z <= z2; z++ ) {
			for( int x = x1; x <= x2; x++ ) {
				if( CoordsInChunkRange( x, y, z ) ) {
					requested = requested || Request( x, y, z );
				}
			}
		}
	}

	return requested;
}

//-------------------------------------------------------------------------------------------------
bool Context::IsAreaLoaded( int x1, int y1, int z1, int x2, int y2, int z2 ) {
	arrange_box_coordinates( x1, y1, z1, x2, y2, z2 );

	return cell_tree->IsAreaCreated( x1, y1, z1, x2, y2, z2 );

	/*
	x1 >>= 4;
	y1 >>= 4;
	z1 >>= 4;
	x2 >>= 4;
	y2 >>= 4;
	z2 >>= 4;
	for( int y = y1; y <= y2; y++ ) {
		for( int z = z1; z <= z2; z++ ) {
			for( int x = x1; x <= x2; x++ ) {
				if( CoordsInChunkRange( x, y, z ) ) {
					int s = cell_tree->GetState( x, y, z, 0 );
					if( !(s & CHUNKSTATE_READY) ) return false;
				}
			}
		}
	}
	return true;*/
}

//-------------------------------------------------------------------------------------------------
CellTree *Context::GetCells() {
	return cell_tree;
}

//-------------------------------------------------------------------------------------------------
Engine *Context::GetEngine() {
	return engine;
}

//-------------------------------------------------------------------------------------------------
void Context::CubeOpRemove( Cell &cell, int index, int mode ) {
	if( cell.cubes[index] == 0 ) return;
	int original = cell.cubes[index];

	cell.changed = 1;
	cell.render_flags |= CRF_CUBES_REMOVED;

	cell.cubes[index] = 0;
	cell.codec_cache->damage[index] = 0;
	for( int i = 0; i < 6; i++ ) {
		cell.codec_cache->paint[index*6+i] = 0;
		cell.codec_cache->decal[index*6+i] = 0;
	}
}

//-------------------------------------------------------------------------------------------------
void Context::CubeOpAdd( Cell &cell, int index, int cube, int mode ) {
	if( cell.cubes[index] != 0 ) return;
	
	cell.cubes[index] = cube;
	cell.render_flags |= CRF_CUBES_ADDED;
}

//-------------------------------------------------------------------------------------------------
void Context::CubeOpChange( Cell &cell, int index, int cube, int mode ) {
	if( cell.cubes[index] == cube ) return;
	cell.cubes[index] = cube;
	
	cell.codec_cache->damage[index] = 0;
	for( int i = 0; i < 6; i++ ) {
		cell.codec_cache->paint[index*6+i] = 0;
		cell.codec_cache->decal[index*6+i] = 0;
	}
}

//-------------------------------------------------------------------------------------------------
void Context::CubeOpDamage( Cell &cell, int index, int amount, int mode ) {
	assert( cell.cubes[index] != 0 );

	int a = cell.codec_cache->damage[index];
	a += amount;
	if( a < 0 ) a = 0;

	if( a >= DAMAGE_MAX ) {
		return CubeOpRemove( cell, index, mode );
		
	}

	cell.codec_cache->damage[index] = a;
}

//-------------------------------------------------------------------------------------------------
void Context::CubeOpDamageSet( Cell &cell, int index, int amount, int mode ) {
	assert( cell.cubes[index] != 0 );

	if( amount >= DAMAGE_MAX ) {
		return CubeOpRemove( cell, index, mode );
	}

	assert( !(amount<0));
	cell.codec_cache->damage[index] = amount;
}

//-------------------------------------------------------------------------------------------------
void Context::CubeOpPaint( Cell &cell, int index, int side, int paint, int mode ) {
	assert( cell.cubes[index] != 0 );
	if( cell.codec_cache->paint[index*6+side] == paint ) return;

	cell.codec_cache->paint[index*6+side] = paint;
	cell.changed = 1;
	cell.render_flags |= CRF_PAINT_CHANGED;
}

//-------------------------------------------------------------------------------------------------
void Context::CubeOpDecal( Cell &cell, int index, int side, int decal, int mode ) {
	assert( cell.cubes[index] != 0 );
	if( cell.codec_cache->decal[index*6+side] == decal ) return;

	cell.codec_cache->decal[index*6+side] = decal;
	cell.changed=1;
	cell.render_flags |= CRF_DECAL_CHANGED;
}



//-------------------------------------------------------------------------------------------------
void set_cbp( copybox_parameters &p, int x1, int y1, int z1, int x2, int y2, int z2, boost::uint16_t *buffer ) {
	p.x1 = x1;
	p.x2 = x2;
	p.y1 = y1;
	p.y2 = y2;
	p.z1 = z1;
	p.z2 = z2;
	p.width = x2-x1 + 1;
	p.height = y2-y1 + 1;
	p.length = z2-z1 + 1;
	p.buffer = buffer;
}

//-------------------------------------------------------------------------------------------------
bool cbp_clip( int &x1, int &y1, int &z1, int &x2, int &y2, int &z2, copybox_parameters &p ) {
	if( x1 < p.x1 ) x1 = p.x1;
	if( x1 > p.x2 ) return false;
	if( y1 < p.y1 ) y1 = p.y1;
	if( y1 > p.y2 ) return false;
	if( z1 < p.z1 ) z1 = p.z1;
	if( z1 > p.z2 ) return false;
	if( x2 < p.x1 ) return false;
	if( x2 > p.x2 ) x2 = p.x2;
	if( y2 < p.y1 ) return false;
	if( y2 > p.y2 ) y2 = p.y2;
	if( z2 < p.z1 ) return false;
	if( z2 > p.z2 ) z2 = p.z2;
	return true;
}

//-------------------------------------------------------------------------------------------------
void Context::CopyBoxChunkData( Context &ct, CellShadow *shadow, int x, int y, int z, copybox_parameters &p ) {

	// shadow unused

	int x1 = x*16, y1 = y*16, z1 = z*16;
	int x2 = x1+15, y2 = y1+15, z2 = z1+15;

	// clip
	if( !cbp_clip( x1, y1, z1, x2, y2, z2, p ) ) return;

	int state;
	Cell *c;
	
	ct.GetCells()->GetCellState( x, y, z, &state, &c );
	//int state = ct.GetCells()->GetState( x, y, z, &c );
	//
	// if chunk state isn't ready (c!=0) or empty (state==empty), break function
	//
	if( !CellStateReady(state) ) {
		// assert here?
		return;

	}

	boost::uint16_t *write;
	boost::uint16_t *read;

	if( CellStateEmpty(state) ) {
		for( int cy = y1; cy <= y2; cy++ ) {
			for( int cz = z1; cz <= z2; cz++ ) {
				
				write = p.buffer + ((x1 - p.x1) + (cy - p.y1)*p.width*p.length + (cz - p.z1)*p.width);
				
				for( int cx = x1; cx <= x2; cx++ ) {
					*write++ = 0;
				}
			}
		}
	} else {
		
		for( int cy = y1; cy <= y2; cy++ ) {
			for( int cz = z1; cz <= z2; cz++ ) {
			
				write = p.buffer + ((x1 - p.x1) + (cy - p.y1)*p.width*p.length + (cz - p.z1)*p.width);
				read = c->cubes + ((x1 - x*16) + (cy - y*16)*256 + (cz - z*16)*16);
				for( int cx = x1; cx <= x2; cx++ ) {
					*write++ = *read++;
				}
			}
		}
	}

	
}

//-------------------------------------------------------------------------------------------------
void Context::SetBoxChunkData( Context &ct, CellShadow *shadow, int x, int y, int z, copybox_parameters &p ) {

	int x1 = x*16, y1 = y*16, z1 = z*16;
	int x2 = x1+15, y2 = y1+15, z2 = z1+15;

	// clip
	if( !cbp_clip( x1, y1, z1, x2, y2, z2, p ) ) return;

	Cell *c;
	int state;

	c = shadow->GetCell( x, y, z );

	//ct.GetCells()->GetCellState( x, y, z, &state, &c );
	
	//if( !CellStateReady(state) ) return; // assert?
	/*
	if( !c ) {
		if( state & CELLSTATE_EMPTY ) {
		//if( ct.GetChunks()->GetChunkState( x, y, z ) == CHUNK_EMPTY ) {
			c = ct.UpgradeEmptyChunk( x, y, z );
		} else {
			return;
		}
	}
	*/

	c->empty = false;
	c->changed = 1;
	c->render_flags |= CRF_CUBES_REMOVED | CRF_CUBES_ADDED;

	boost::uint16_t *read;
	//boost::uint16_t *write;
	int write;
	for( int cy = y1; cy <= y2; cy++ ) {
		for( int cz = z1; cz <= z2; cz++ ) {
			
			read = p.buffer + ((x1 - p.x1) + (cy - p.y1)*p.width*p.length + (cz - p.z1)*p.width);
			write = ((x1 - x*16) + (cy - y*16)*256 + (cz - z*16)*16);
			
			for( int cx = x1; cx <= x2; cx++ ) {
				
				ct.CubeOpChange( *c, write++, *read++, CUBEOP_SYSTEM_LARGE_AREA );
				//*write++ = *read++;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
void Context::ClearBoxChunkData( Context &ct, CellShadow *shadow, int x, int y, int z, copybox_parameters &p ) {


	Cell *c = shadow->GetCell( x, y, z );
	//if( !c ) return; // (chunk is empty or something wrong occurred at a higher level)

	
	c->changed = 1;
	c->render_flags |= CRF_CUBES_REMOVED;

	int x1 = x*16, y1 = y*16, z1 = z*16;
	int x2 = x1+15, y2 = y1+15, z2 = z1+15;

	// clip
	if( !cbp_clip( x1, y1, z1, x2, y2, z2, p ) ) return;

	
	boost::uint16_t *read;
	//boost::uint16_t *write;
	int write;
	if( !p.buffer ) {
		
		for( int cy = y1; cy <= y2; cy++ ) {
			for( int cz = z1; cz <= z2; cz++ ) {
		
				//write = c->cubes + ((x1 - x*16) + (cy - y*16)*256 + (cz - z*16)*16);
				write = (x1-x*16) + (cy-y*16)*256 + (cz-z*16)*16;

				for( int cx = x1; cx <= x2; cx++ ) {
					ct.CubeOpRemove( *c, write++, CUBEOP_SYSTEM_LARGE_AREA ); 
					//*write++ = 0;
				}
			}
		}
	} else {
		
		for( int cy = y1; cy <= y2; cy++ ) {
			for( int cz = z1; cz <= z2; cz++ ) {
		
				//write = c->cubes + ((x1 - x*16) + (cy - y*16)*256 + (cz - z*16)*16);
				write = ((x1 - x*16) + (cy - y*16)*256 + (cz - z*16)*16);
				read = p.buffer + ((x1 - p.x1) + (cy - p.y1)*p.width*p.length + (cz - p.z1)*p.width);
				for( int cx = x1; cx <= x2; cx++ ) {
					if( !(*read++) ) {
						ct.CubeOpRemove( *c, write++, CUBEOP_SYSTEM_LARGE_AREA );
						//*write++ = 0;
					}
				}
			}
		}
	}
}


//-------------------------------------------------------------------------------------------------
void Context::DamageBoxChunkData( Context &ct, CellShadow *shadow, int x, int y, int z, copybox_parameters &p ) {


	Cell *c = shadow->GetCell( x, y, z );
	//if( !c ) return; // (chunk is empty or something wrong occurred at a higher level)

	
	c->changed = 1; 

	int x1 = x*16, y1 = y*16, z1 = z*16;
	int x2 = x1+15, y2 = y1+15, z2 = z1+15;

	// clip
	if( !cbp_clip( x1, y1, z1, x2, y2, z2, p ) ) return;

	
	boost::uint16_t *read;
	//boost::uint16_t *write;
	int write;

	assert( p.buffer ); // damagebox requires buffer
	
	for( int cy = y1; cy <= y2; cy++ ) {
		for( int cz = z1; cz <= z2; cz++ ) {
		
			//write = c->cubes + ((x1 - x*16) + (cy - y*16)*256 + (cz - z*16)*16);
			write = ((x1 - x*16) + (cy - y*16)*256 + (cz - z*16)*16);
			read = p.buffer + ((x1 - p.x1) + (cy - p.y1)*p.width*p.length + (cz - p.z1)*p.width);
			for( int cx = x1; cx <= x2; cx++ ) {
				if( !(*read++) ) {
					ct.CubeOpDamage( *c, write++, *read++, CUBEOP_SYSTEM_LARGE_AREA );
					//*write++ = 0;
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
template <typename F> 
void Context::BoxFunction( int x1, int y1, int z1, int x2, int y2, int z2, boost::uint16_t *buffer, CellShadow *shadow, F func ) {
	arrange_box_coordinates( x1, y1, z1, x2, y2, z2 );

	copybox_parameters p;
	set_cbp( p, x1, y1, z1, x2, y2, z2, buffer );

	int cx1 = x1 >> 4;
	int cy1 = y1 >> 4;
	int cz1 = z1 >> 4;
	int cx2 = x2 >> 4;
	int cy2 = y2 >> 4;
	int cz2 = z2 >> 4;

	for( int y = cy1; y <= cy2; y++ ) {
		for( int z = cz1; z <= cz2; z++)  {
			for( int x = cx1; x <= cx2; x++ ) {
				(*func)( *this, shadow, x, y, z, p );
			}
		}
	}
}
 

//-------------------------------------------------------------------------------------------------
void Context::InternalCopyBox( int x1, int y1, int z1, int x2, int y2, int z2, boost::uint16_t *buffer ) {
	 
	BoxFunction( x1, y1, z1, x2, y2, z2, buffer, 0, &Context::CopyBoxChunkData );
}

//-------------------------------------------------------------------------------------------------
void Context::InternalSetBox( CellShadow &shadow, int x1, int y1, int z1, int x2, int y2, int z2, boost::uint16_t *buffer ) {

	BoxFunction( x1, y1, z1, x2, y2, z2, buffer, &shadow, &Context::SetBoxChunkData );

}

//-------------------------------------------------------------------------------------------------
void Context::InternalClearBoxMapped( CellShadow &shadow, int x1, int y1, int z1, int x2, int y2, int z2, boost::uint16_t *buffer ) {

	BoxFunction( x1, y1, z1, x2, y2, z2, buffer, &shadow, &Context::ClearBoxChunkData );

}

//-------------------------------------------------------------------------------------------------
void Context::InternalClearBox( CellShadow &shadow, int x1, int y1, int z1, int x2, int y2, int z2 ) {
	
	BoxFunction( x1, y1, z1, x2, y2, z2, 0, &shadow, &Context::ClearBoxChunkData );
}

//-------------------------------------------------------------------------------------------------
void Context::InternalDamageBox( CellShadow &shadow, int x1, int y1, int z1, int x2, int y2, int z2, boost::uint16_t *buffer ) {
	BoxFunction( x1, y1, z1, x2, y2, z2, buffer, &shadow, &Context::DamageBoxChunkData );
}

//-------------------------------------------------------------------------------------------------
bool Context::Collision( int x, int y, int z ) {
	
	int sample = cell_tree->Sample( x, y, z );
	
	if( sample != -1 ) {
		// todo: solid map
		return sample > 0;
	} else {
		return true;
	}
}

//-------------------------------------------------------------------------------------------------
bool Context::Collision3f( cml::vector3f &pos ) {
	return Collision( (int)floor(pos[0]), (int)floor(pos[1]), (int)floor(pos[2]) );
}

//-------------------------------------------------------------------------------------------------
bool Context::Collision3d( cml::vector3d &pos ) {
	return Collision( (int)floor(pos[0]), (int)floor(pos[1]), (int)floor(pos[2]) );
}

//-------------------------------------------------------------------------------------------------
void Context::SetCommandArea( Command &c, int x, int y, int z, int size_x, int size_y, int size_z, bool secondary ) {
	BoundingBox &target = secondary ? c.area.box : c.area.box2;
	if( secondary ) {
		c.area.use_box2 = true;
	}
	target.position[0] = x;
	target.position[1] = y;
	target.position[2] = z;
	target.size[0] = size_x + 2; // safety padding!
	target.size[1] = size_y + 2;
	target.size[2] = size_z + 2;
}

//-------------------------------------------------------------------------------------------------
void Context::SetCommandArea( Command &c, int x, int y, int z, int size, bool secondary ) {
	SetCommandArea( c, x, y, z, size, size, size, secondary );
}

//-------------------------------------------------------------------------------------------------
Command *Context::NewCommand() {
	Command *c = Memory::Alloc<Command>();
	c->finished = false;
	c->executing = false;
	c->id = command_counter;
	command_counter++;
	c->ct = this;
	c->area.use_box2 = false;
	return c;
}

//-------------------------------------------------------------------------------------------------
void Context::DestroyPowered( int x, int y, int z, int power ) {

	// prefetch chunk(s)
	// max affected range is x,y,z +- power (power is measured in max affected cubes length)
	RequestCubeBox( x - power, y - power, z - power, x + power, y + power, z + power );

	Command *c = NewCommand(); 
	 
	c->id = CMD_CLEAR_EXPLOSION;
	c->data[0] = x;
	c->data[1] = y;
	c->data[2] = z;
	c->data[3] = power; 

	SetCommandArea( *c, x, y, z, power  );
	commands.Push(c);
	
}

//-------------------------------------------------------------------------------------------------
void Context::SetCube( int x, int y, int z, int index ) {
	if( !CoordsInCubeRange( x, y, z ) ) return;
	Request( x>>4, y>>4, z>>4 );

	Command *c = NewCommand();

	c->id		= CMD_SET_SINGLE;
	c->data[0]	= x;
	c->data[1]	= y;
	c->data[2]	= z;
	c->data[3]	= index;

	SetCommandArea( *c, x, y, z, 1 );
	commands.Push(c);
}

//-------------------------------------------------------------------------------------------------
void Context::ClearCube( int x, int y, int z ) {
	if( !CoordsInCubeRange( x, y, z ) ) return;
	Request( x>>4, y>>4, z>>4 );

	Command *c	= NewCommand(); 
	c->id		= CMD_CLEAR_SINGLE;
	c->data[0]	= x;
	c->data[1]	= y;
	c->data[2]	= z; 

	SetCommandArea( *c, x, y, z, 1 );
	commands.Push(c);
}

//-------------------------------------------------------------------------------------------------
void Context::SetBox( int x, int y, int z, int width, int height, int length, boost::uint16_t *data ) {
	RequestCubeBox( x, y, z, x + width, y + height, z + length );

	Command *c	= NewCommand(); 
	c->id		= CMD_SET_BOX;
	c->data[0]	= x;
	c->data[1]	= y;
	c->data[2]	= z;
	c->data[3]	= width;
	c->data[4]	= height;
	c->data[5]	= length;
	c->data[6]	= (int)data; 

	SetCommandArea( *c, x + (width>>1), y + (height>>1), z + (length>>1), (width>>1), (height>>1), (length>>1) );

	commands.Push(c);
}

//-------------------------------------------------------------------------------------------------
void Context::ClearBox( int x, int y, int z, int width, int height, int length ) {
	RequestCubeBox( x, y, z, x + width, y + height, z + length );

	Command *c	= NewCommand();
 
	c->id		= CMD_CLEAR_BOX;
	c->data[0]	= x;
	c->data[1]	= y;
	c->data[2]	= z;
	c->data[3]	= width;
	c->data[4]	= height;
	c->data[5]	= length;

	SetCommandArea( *c, x + (width>>1), y+(height>>1), z+(length>>1), (width>>1), (height>>1), (length>>1) );
	commands.Push(c);
}

//-------------------------------------------------------------------------------------------------
void Context::SetCubePaint( int x, int y, int z, int side, int paint ) {
	Request( x>>4, y>>4, z>>4 );

	Command *c	= NewCommand();
	
	c->id		= CMD_PAINT;
	c->data[0]	= x;
	c->data[1]	= y;
	c->data[2]	= z;
	c->data[3]	= paint;
	c->data[4]	= side ;
	
	SetCommandArea( *c, x, y, z, 1 );
	commands.Push(c);
}

//-------------------------------------------------------------------------------------------------
enum {
	PARAMS_EXPLOSION_X,
	PARAMS_EXPLOSION_Y,
	PARAMS_EXPLOSION_Z,
	PARAMS_EXPLOSION_POWER
};

//-------------------------------------------------------------------------------------------------
void Context::ProcessCommandExplosion( Command *c ) {
	
	// load params
	int x, y, z, p;
	x = c->data[PARAMS_EXPLOSION_X];
	y = c->data[PARAMS_EXPLOSION_Y];
	z = c->data[PARAMS_EXPLOSION_Z];
	p = c->data[PARAMS_EXPLOSION_POWER];

	RequestCubeBoxWaitEach( x-p, y-p, z-p, x+p, y+p, z+p );

	CellShadow shadow( *this, x-p, y-p, z-p, x+p, y+p, z+p );

	//ShadowCubeBox( x-p, y-p, z-p, x+p, y+p, z+p );
	
	int diameter = p * 2 + 1;
	int diameter2 = diameter * diameter;

	// create buffers
	boost::uint16_t *cube_buffer;
	cube_buffer = (boost::uint16_t*)Memory::AllocMem( diameter * diameter * diameter * sizeof(*cube_buffer) );



	boost::uint16_t *destruction_map;
	destruction_map = (boost::uint16_t*)Memory::AllocMem( diameter * diameter * diameter * sizeof(*destruction_map) );
	for( int i = 0; i < diameter*diameter*diameter; i++ ) {
		destruction_map[i] = 0;
	}

	// fill cube buffer
	InternalCopyBox( x-p, y-p, z-p, x+p, y+p, z+p, cube_buffer );
	   
	// cast rays
	for( int i = 0; i < engine->ExplosionVectorCount(); i++ ) {
		cml::vector3f direction = *engine->ExplosionVector(i);
		cml::vector3f raypos( (float)diameter/2.0f, (float)diameter/2.0f, (float)diameter/2.0f );

		int power = engine->InitialExplosionPower();// INITIAL_EXPLOSION_POWER;
		float length = (float)p;
		const float timestep = 0.3f;
		int previndex = -1;

		int powerstep = (int)floor((1.0 - (0.3 / length)) * 16384.0);
		
		for( float time = 0.0f; time < length; time += timestep ) {
			cml::vector3i newpos( (int)floor(raypos[0]), (int)floor(raypos[1]), (int)floor(raypos[2]));
			int newindex = newpos[0] + newpos[1]*diameter2 + newpos[2]*diameter;
			if( newindex != previndex ) {

				int ci = MaskCubeIndex(cube_buffer[newindex]);
				if( ci > 0 ) {
					const CubeProperties *cp = kernel->GetData( ci );
					boost::uint16_t *dm_entry = &destruction_map[newindex];

					if( power > cp->blast_resistance ) {
						power -= cp->blast_resistance;
						(*dm_entry) = 256;
					} else {
						int dmg = (power * cp->blast_resistance_r) >> 16;
						if( dmg > (*dm_entry) ) (*dm_entry) = dmg;
						break;
					}
				}
				 
			}
			power = power * powerstep;
			power >>= 14;
			if( power <= 0 ) break;

			raypos += direction * 0.3f;
		}

	}

	InternalDamageBox( shadow, x-p, y-p, z-p, x+p, y+p, z+p, destruction_map );

	/*
	// apply destruction
	for( int i = 0; i < diameter*diameter*diameter; i++ ) {
		if( destruction_map[i] > 0 && MaskCubeIndex(cube_buffer[i]) > 0 ) {
			CubeOpDamage( destruction_map[i] );
		}
		if( destruction_map[i] >= kernel->GetData(cube_buffer[i])->blast_resistance ) {// absorption_map[cube_buffer[i]] ) {
			cube_buffer[i] = 0;
		}
	}*/
	
	// save damage
	//ClearBoxMappedI( x-p, y-p, z-p, x+p, y+p,z+p, cube_buffer );

	// callback user with explosion data
	// ie callback( x,y,z,destruction_map )
	// TODO
	// and remove the deletes so the user can view them
	//   2/14/13 : what?

	shadow.Apply();

	Memory::FreeMem( cube_buffer );
	Memory::FreeMem( destruction_map );
	
	InterlockedExchange( &c->finished, 1 );
	RemoveActiveCommand( c );
	Memory::FreeMem( c );
}

//-------------------------------------------------------------------------------------------------
void Context::ProcessCommandClearBox( Command *c ) {

	int x,y,z,w,h,l;
	x = c->data[0]; y = c->data[1]; z = c->data[2];
	w = c->data[3]; h = c->data[4]; l = c->data[5];

	RequestCubeBoxWaitEach( x, y, z, x+w-1, y+h-1, z+l-1 );
	CellShadow shadow( *this, x, y, z, x+w-1, y+h-1, z+l-1 );
	//ShadowCubeBox( x, y, z, x+w-1, y+h-1, z+l-1 );
	InternalClearBox( shadow, c->data[0], c->data[1], c->data[2], c->data[0] + c->data[3] + 1, c->data[1] + c->data[4] + 1, c->data[2] + c->data[5] );

	shadow.Apply();

	InterlockedExchange( &c->finished, 1 );
	RemoveActiveCommand( c );
	Memory::FreeMem( c );
}

//-------------------------------------------------------------------------------------------------
void Context::ProcessCommandClearSingle( Command *c ) {

	int cx, cy, cz;
	cx = c->data[0]>>4;
	cy = c->data[1]>>4;
	cz = c->data[2]>>4;

	cell_tree->Reserve( cx, cy, cz );

	//if( Request( c->data[0]>>4, c->data[1]>>4, c->data[2]>>4 ) ) return;
	RequestWait( cx, cy, cz );

	int state;
	cell_tree->GetCellState( cx, cy, cz, &state, 0 );
	if( state & CELLSTATE_EMPTY ) {
		
		cell_tree->Release( cx, cy, cz );

	} else {
		CellShadowSingle shadow( *this, cx, cy, cz );
		Cell *cell = shadow.GetCell();
		int x, y, z;
		x = c->data[0] & 15;
		y = c->data[1] & 15;
		z = c->data[2] & 15;
		cell->cubes[ x + y * 256 + z * 16 ] = 0;
		cell->render_flags |= CRF_CUBES_REMOVED;

		cell_tree->Upgrade( cx, cy, cz );
		shadow.Apply();
		cell_tree->Release( cx, cy, cz );
	}
	

	/*
	if( !chunk ) {
		return; // chunk is an 'empty' chunk
	} else {
		chunk->cubes[ (c->data[0] & 15) + (c->data[1]&15)*256 + (c->data[2]&15)*16 ] = 0;
		chunk->changed = 1;
		chunk->mod_flags |= MF_CUBES_CLEARED;
	}*/
	
	InterlockedExchange( &c->finished, 1 )
	;RemoveActiveCommand( c );
	Memory::FreeMem( c );
}

//-------------------------------------------------------------------------------------------------
void Context::ProcessCommandSetBox( Command *c ) {

	int p1[3], p2[3];
	p1[0] = c->data[0]; p1[1] = c->data[1]; p1[2] = c->data[2]; 
	p2[0] = p1[0] + c->data[3] - 1; p2[1] = p1[1] + c->data[4] - 1; p2[2] = p1[2] + c->data[5] - 1;
	cell_tree->ReserveArea( p1, p2 );

	RequestCubeBoxWaitEach( p1, p2 );

	CellShadow shadow( *this, p1, p2 );
	
	InternalSetBox( shadow, c->data[0], c->data[1], c->data[2], c->data[0] + c->data[3] - 1, c->data[1] + c->data[4] - 1, c->data[2] + c->data[5] - 1, (boost::uint16_t*)c->data[6] );

	cell_tree->UpgradeArea( p1, p2 );
	shadow.Apply();
	cell_tree->ReleaseArea( p1, p2 );

	InterlockedExchange( &c->finished, 1 );
	RemoveActiveCommand( c );
	Memory::FreeMem( c );
}

//-------------------------------------------------------------------------------------------------
void Context::ProcessCommandSetSingle( Command *c ) {

	cell_tree->Reserve( c->world_index );
	
	Cell *cell;//, int *state;

	RequestWait( c->world_index );
	CellShadowSingle shadow( *this, c->world_index );

//	Upgrade( c->data[0] );
	//if( Request( c->data[0] >> 4, c->data[1] >> 4, c->data[2] >> 4 ) ) return;

	cell = shadow.GetCell();

	//Chunk *chunk = GetChunk( c->data[0] >> 4, c->data[1] >> 4, c->data[2] >> 4 );
	//if( !chunk ) {
	//	chunk = UpgradeEmptyChunk( c->data[0] >> 4, c->data[1] >> 4, c->data[2] >> 4 );
	//}

	cell->cubes[ c->data[0] ] = c->data[1];

	//chunk->cubes[ (c->data[0]&15) + (c->data[1]&15)*256 + (c->data[2]&15)*16 ] = c->data[3];
	cell->changed = 1;
	
	cell->render_flags |= CRF_CUBES_ADDED;
	cell->empty = 0;

	cell_tree->Upgrade( c->world_index );
	shadow.Apply();
	cell_tree->Release( c->world_index );

	InterlockedExchange( &c->finished, 1 );
	RemoveActiveCommand( c );
	Memory::FreeMem( c );
}

//-------------------------------------------------------------------------------------------------
void Context::ProcessCommandPaint( Command *c ) {
	// todo

	//chunk->changed = 1;
	//chunk->mod_flags |= MF_PAINT_CHANGED;
	InterlockedExchange( &c->finished, 1 );
	RemoveActiveCommand( c );
	Memory::FreeMem( c );
}

//-------------------------------------------------------------------------------------------------
void Context::ProcessCommand( Command *c ) {
	if( !c ) return;
	if( c->finished ) return;
	// parse id

	switch( c->id & 0xFF00 ) {
		case CMDH_BLOCK_MODS:	// block mod commands
			switch( c->id & 0xFF ) {
			case CMDL_CLEAR_EXPLOSION:
				ProcessCommandExplosion( c );
				break;
			case CMDL_CLEAR_BOX:
				ProcessCommandClearBox( c );
				break;
			case CMDL_CLEAR_SINGLE:
				ProcessCommandClearSingle( c );
				break;
			case CMDL_SET_BOX:
				ProcessCommandSetBox( c );
				break;
			case CMDL_SET_SINGLE:
				ProcessCommandSetSingle( c );
				break;
			case CMDL_PAINT:
				ProcessCommandPaint( c );
				break;
			}
			break;
	}

}
/*
//-------------------------------------------------------------------------------------------------
void Context::ProcessCommands() {
	for(;;) {
		command *c = commands.Get();
		if( c ) {
			if( ProcessCommand(c) ) {
				commands.Pop();
			} else {
				break;
			}
		} else {
			break;
		}
	}
}*/

//-------------------------------------------------------------------------------------------------
bool Context::Pick( const cml::vector3d &start, const cml::vector3d &end, 
					cml::vector3d *intersection, cml::vector3i *target, int *normal ) {
	
	cml::vector3d inc = end-start;
	cml::vector3d vec = start;

	

	inc.normalize();

	double length = cml::length(end-start);

	if( inc[0] == 0.0 && inc[1] == 0.0 && inc[2] == 0.0 ) return false;

	double dx, dy, dz;
	dx = inc[0] != 0.0 ? (1.0f / inc[0]) : 0;
	dy = inc[1] != 0.0 ? (1.0f / inc[1]) : 0;
	dz = inc[2] != 0.0 ? (1.0f / inc[2]) : 0;

	cml::vector3d planes;
	planes = vec;
	planes[0] = floor(planes[0]);
	planes[1] = floor(planes[1]);
	planes[2] = floor(planes[2]);

	cml::vector3f planesteps(1.0,1.0,1.0);
	for( int i = 0; i < 3; i++ )
		if( inc[i] < 0 ) planesteps[i] = -planesteps[i]; 

	double total = 0.0f;

	CellReader reader( *this );
	
	for( ; total < length; ) {
		cml::vector3d dist;

		dist = planes;

		if( inc[0] >= 0.0f ) dist[0] += 1.0;
		if( inc[1] >= 0.0f ) dist[1] += 1.0;
		if( inc[2] >= 0.0f ) dist[2] += 1.0;

		dist -= vec;
		dist[0] *= dx;
		dist[1] *= dy;
		dist[2] *= dz;

		int dir=0;
		double shortest = dist[0];

		if( dist[1] < shortest && inc[1] != 0.0 ) { shortest = dist[1]; dir = 1; }
		if( dist[2] < shortest && inc[2] != 0.0 ) { shortest = dist[2]; dir = 2; }
		
		vec += inc * shortest;
		if( dir == 0 ) {
			planes[0] += planesteps[0];
		} else if( dir == 1 ) {
			planes[1] += planesteps[1];
		} else if( dir == 2 ) {
			planes[2] += planesteps[2];
		}

		total += shortest;
		if( total > length ) return false;
		

		cml::vector3i cube_index((int)planes[0], (int)planes[1], (int)planes[2]);
		Cell *cell = reader.GetCell( cube_index[0] >> 4, cube_index[1] >> 4, cube_index[2] >> 4 );
		//Chunk *ch = GetChunk( cube_index[0], cube_index[1], cube_index[2] );
		if( cell ) {
			if( cell->cubes[(cube_index[0]&15) + (cube_index[1]&15)*256 + (cube_index[2]&15)*16] != 0 ) {
				// collision
				
				// todo: collision filters

				if( intersection ) {
					(*intersection) = vec;

				}

				if( target ) {
					(*target) = cube_index;
				}

				if( normal ) {
					switch( dir ) {
					case 0:
						if( inc[0] >= 0 ) 
							*normal = NORMAL_LEFT;
						else
							*normal = NORMAL_RIGHT;
						break;
					case 1:
						if( inc[1] >= 0 )
							*normal = NORMAL_DOWN;
						else
							*normal = NORMAL_UP;
						break;
					case 2:
						if( inc[2] >= 0 )
							*normal = NORMAL_BACK;
						else
							*normal = NORMAL_FORWARD;
						break;
					}
				}

				return true;
			}
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
void Context::AcquireFileLock( boost::uint64_t region_index ) {
	file_lock_table.Acquire( region_index );
}

//-------------------------------------------------------------------------------------------------
void Context::ReleaseFileLock( boost::uint64_t region_index ) {
	file_lock_table.Release( region_index );
}

//-------------------------------------------------------------------------------------------------
Context::Context( const char *mapfile, const char *directory_path, int num_cmdex_threads ) {
	datapath = directory_path;
	
	map.Load( mapfile );
	world_width = map.width;
	world_depth = map.depth;
	world_length = map.length;

	sea_level = map.sea_level;

	cell_tree = new(Memory::Alloc<CellTree>()) CellTree;
	
	command_dispatcher_thread = boost::thread( boost::bind( &Context::CommandDispatcherThread, this ) );

	StartCommandExecutionThreads( num_cmdex_threads );

}

//-------------------------------------------------------------------------------------------------
Context::~Context() {

	StopCommandExecutionThreads();
}

}
