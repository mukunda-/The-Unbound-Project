//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include "world/ChunkManager.h"
#include "world/JobManager.h"
#include "world/CubeKernel.h"
#include <string>
#include <vector>
#include <cml/cml.h>
#include "world/Cell.h"
#include "world/Context.h"

//------------------------------------------------------------------------------------------------------------------------
namespace World { 

typedef void (*explosion_hook)( int x, int y, int z, int radius, boost::uint16_t *damage_map );

//------------------------------------------------------------------------------------------------------------------------
class Engine {
	CellPool cell_pool;
	JobManager jobs;
	CellCodec cell_codec;

	std::vector<CubeKernel*> cube_kernels;

	cml::vector3f *explosion_vector_list;
	int explosion_vector_count;

	std::string root_directory;

	enum {
		INITIAL_EXPLOSION_POWER = 250*256
	};

	void ComputeExplosionVectors();
	void LoadCubeKernels();
	int AddCubeKernel( const boost::filesystem::path &path );

public:
	void Initialize( const char *p_root_directory );
	
	void HookExplosionCallback( explosion_hook function );

	CellPool *GetCellPool();
	JobManager *GetJobs();
	CellCodec *GetCodec();

	int ExplosionVectorCount();
	cml::vector3f *ExplosionVector( int index );
	int InitialExplosionPower();


	//void PushJob_CellSave( Cell *ch );
};


}
