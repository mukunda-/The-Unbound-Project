//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

namespace World {

int Engine::InitialExplosionPower() {
	return 9000;
}

//-------------------------------------------------------------------------------------------------
void Engine::ComputeExplosionVectors() {
	if( explosion_vector_list ) return;
	const int eds = 12; // explosion distribution slices (number of vectors for one edge)
	int nvectors = eds*eds * 2; // top+bottom
	nvectors += eds*(eds-2) * 2; // front+back
	nvectors += (eds-2)*(eds-2) * 2; // left+right

	explosion_vector_list = new cml::vector3f[eds];

	explosion_vector_count = eds;

	cml::vector3f *vec = explosion_vector_list;
	// distribute points on box
	// top/bottom: (y=+-1)
	for( int x = 0; x < eds; x++ ) {
		for( int z = 0; z < eds; z++ ) {
			(*vec)[0] = (float)x / 11.0f * 2.0f - 1.0f;
			(*vec)[1] = 1.0f;
			(*vec)[2] = (float)z / 11.0f * 2.0f - 1.0f;
			vec++;
			(*vec)[0] = (float)x / 11.0f * 2.0f - 1.0f;
			(*vec)[1] = -1.0f;
			(*vec)[2] = (float)z / 11.0f * 2.0f - 1.0f;
			vec++;
		}
	}

	// front/back (z=+-1)
	for( int x = 0; x < eds; x++ ) {
		for( int y = 1; y < eds-1; y++ ) {
			(*vec)[0] = (float)x / 11.0f * 2.0f - 1.0f;
			(*vec)[1] = (float)y / 11.0f * 2.0f - 1.0f;
			(*vec)[2] = 1.0f;
			vec++;
			(*vec)[0] = (float)x / 11.0f * 2.0f - 1.0f;
			(*vec)[1] = (float)y / 11.0f * 2.0f - 1.0f;
			(*vec)[2] = -1.0f;
			vec++;
		}
	}
	
	// left/right (x=+-1)
	for( int z = 1; z < eds-1; z++ ) {
		for( int y = 1; y < eds-1; y++ ) {
			(*vec)[0] = 1.0f;
			(*vec)[1] = (float)y / 11.0f * 2.0f - 1.0f;
			(*vec)[2] = (float)z / 11.0f * 2.0f - 1.0f;
			vec++;
			(*vec)[0] = -1.0f;
			(*vec)[1] = (float)y / 11.0f * 2.0f - 1.0f;
			(*vec)[2] = (float)z / 11.0f * 2.0f - 1.0f;
			vec++;
		}
	}
	
	for( int i = 0; i < nvectors; i++ ) {
		explosion_vector_list[i].normalize();
	}
}

//-------------------------------------------------------------------------------------------------
cml::vector3f *Engine::ExplosionVector( int index ) {
	return &explosion_vector_list[index];
}

int Engine::ExplosionVectorCount() {
	return explosion_vector_count;
}

#if 0
namespace CKTranslations {
	struct translation {
		int value;
		const char *text;
	};

	enum {
		CUBE_NAME =1,
		CUBE_BUYABLE,
		CUBE_COST,
		CUBE_DESCRIPTION,
		CUBE_CLUTTER,
		CUBE_GEOMETRY,
		CUBE_EMITTANCE,
		CUBE_BLASTRESIST,
		CUBE_FLAMMABILITY,
		CUBE_MATERIAL,
		CUBE_PHYSICS,
		CUBE_LIQUID,
		CUBE_EMITTER,
		CUBE_SOLID,
		CUBE_ITEM,
		CUBE_EXPLOSIVE,
	};

	enum {
		SIDE_SPRING =1,
		SIDE_FRICTION,
		SIDE_SURFACE,
		SIDE_DAMAGE,
		SIDE_IMPALE,
		SIDE_HOLY,
		SIDE_TEXTURE,
		SIDE_SHIELD,
		SIDE_MOVEMENT,
		SIDE_ABSORBLIGHT,
		SIDE_PAINTABLE
	};

	enum {
		SIDEINDEX_FRONT =1,
		SIDEINDEX_BACK,
		SIDEINDEX_RIGHT,
		SIDEINDEX_LEFT,
		SIDEINDEX_TOP,
		SIDEINDEX_BOTTOM
	};

	translation translations_main[] = {
		{CUBE_NAME,"name"},
		{CUBE_BUYABLE,"buyable"},
		{CUBE_COST,"cost"},
		{CUBE_DESCRIPTION,"description"},
		{CUBE_CLUTTER,"clutter"},
		{CUBE_GEOMETRY,"geometry"},
		{CUBE_EMITTANCE,"emittance"},
		{CUBE_BLASTRESIST,"blastresist"},
		{CUBE_FLAMMABILITY,"flammability"},
		{CUBE_MATERIAL,"material"},
		{CUBE_PHYSICS,"physics"},
		{CUBE_LIQUID,"liquid"},
		{CUBE_EMITTER,"emitter"},
		{CUBE_SOLID,"solid"},
		{CUBE_ITEM,"item"},
		{CUBE_EXPLOSIVE,"explosive"},
		//{KE_GROWABLE,"growable"}, TODO
		{-1,"poopies"},
	};

	translation translations_side[] = {
		{SIDE_SPRING,"spring"},
		{SIDE_FRICTION,"friction"},
		{SIDE_SURFACE,"surface"},
		{SIDE_DAMAGE,"damage"},
		{SIDE_IMPALE,"impale"},
		{SIDE_HOLY,"holy"},
		{SIDE_TEXTURE,"texture"},
		{SIDE_SHIELD,"shield"},
		{SIDE_MOVEMENT,"movement"},
		{SIDE_ABSORBLIGHT,"absorblight"},
		{SIDE_PAINTABLE,"paintable"},
		{-1,"poopies"}
	};

	translation translations_sideindexes[] = {
		{SIDEINDEX_FRONT,"front"},
		{SIDEINDEX_BACK,"back"},
		{SIDEINDEX_RIGHT,"right"},
		{SIDEINDEX_LEFT,"left"},
		{SIDEINDEX_TOP,"top"},
		{SIDEINDEX_BOTTOM,"bottom"},
		{-1,"popies"}
	};

	int Translate( const char *text, const translation *table ) {
		for( int i = 0; table[i].value != -1; i++ ) {
			if( strcmp( text, table[i].text ) == 0 ) {
				return table[i].value;
			}
		}
		return -1;
	}

	int Translate_Main( const char *text ) {
		return Translate( text, translations_main );
	}

	int Translate_Side( const char *text ) {
		return Translate( text, translations_side );
	}

	int Translate_SideIndex( const char *text ) {
		return Translate( text, translations_sideindexes );
	}
}

//-------------------------------------------------------------------------------------------------
int Engine::AddCubeKernel( const boost::filesystem::path &path ) {
	TiXmlDocument doc(path.string().c_str());

	doc.LoadFile();

	TiXmlNode *kl = doc.FirstChild("definitions");
	TiXmlNode *node;
	std::vector<CubeProperties> props;

	for( node = kl->FirstChild("cube"); node; node = node->NextSibling("cube") ) {
		CubeProperties p;
		memset( &p, 0, sizeof(p) );
		TiXmlElement *e;
		e = node->ToElement();
		for( TiXmlAttribute *attr = e->FirstAttribute(); attr; attr = attr->Next() ) {
			int index = CKTranslations::Translate_Main( attr->Name() );

			float value = (float)atof(attr->Value());
		
			switch( index ) {
				// todo
			case CKTranslations::CUBE_BLASTRESIST:
				p.blast_resistance = (int)floor((value * 256.0f) / 100.0f + 0.5f);
				break;
			}
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
void Engine::LoadCubeKernels() {
	// search /kernels folder and load cube kernel files

	std::string path;
	path = root_directory;
	path += "kernels/";

	path = root_directory;
	boost::filesystem::directory_iterator iter(path);
	while( iter != boost::filesystem::directory_iterator() ) {
		if( boost::filesystem::is_regular_file(*iter) ) {
			// load cube kernel
			
			AddCubeKernel( *iter );
		}
		iter++;
	}
}
#endif
/*
//-------------------------------------------------------------------------------------------------
void Engine::PushJob_CellSave( Chunk *ch ) {

}*/

//-------------------------------------------------------------------------------------------------
JobManager *Engine::GetJobs() {
	return &jobs;
}

//-------------------------------------------------------------------------------------------------
CellPool *Engine::GetCellPool() {
	return &cell_pool;
}

//-------------------------------------------------------------------------------------------------
CellCodec *Engine::GetCodec() {
	return &cell_codec;
}

//-------------------------------------------------------------------------------------------------
void Engine::Initialize( const char *p_root_directory ) {
	root_directory = p_root_directory;
	ComputeExplosionVectors();
//	LoadCubeKernels(); TODO
}

}

