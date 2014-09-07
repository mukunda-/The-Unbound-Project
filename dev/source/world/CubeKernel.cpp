//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#include "stdafx.h"

//-------------------------------------------------------------------------------------------------
namespace World {

//	STEAM_1:0:16483256 

namespace {

	enum {
		INS_DESCRIPTION,
		INS_COST,
		INS_CLUTTER,
		INS_LIGHT,
		INS_BLASTRESIST,
		INS_FLAMMABILITY,
		INS_MATERIAL,
		INS_PHYSICS,
		INS_EMITTER,
		INS_PASSABLE,
		INS_COLLISION,
		INS_ITEM,
		INS_EXPLOSIVE,
		INS_CROP_CONDITIONS,
		INS_CROP_YIELD,
		INS_CROP_RESIST_PESTS,
		INS_CROP_RESIST_DISEASE,
		INS_CROP_WATER_COMFORT,
		INS_CROP_HUNGER,
		INS_SIDES_SPRING,
		INS_SIDES_FRICTION,
		INS_SIDES_SURFACETYPE,
		INS_SIDES_DAMAGE,
		INS_SIDES_HOLY,
		INS_SIDES_TEXTURE,
		INS_SIDES_SHIELDED,
		INS_SIDES_MOVEBONUS,
		INS_SIDES_LIGHTABSORB,
		INS_SIDES_PAINTABLE
	};

	Util::StringTable translation_table;

	bool translation_table_setup = false;

	
	void SetupTranslationTable() {
		if( translation_table_setup ) return;
	
		translation_table.Add( "description", (void*)INS_DESCRIPTION );
		translation_table.Add( "cost", (void*)INS_COST );
		translation_table.Add( "clutter", (void*)INS_CLUTTER );
		translation_table.Add( "light", (void*)INS_LIGHT );
		translation_table.Add( "blastresist", (void*)INS_BLASTRESIST );
		translation_table.Add( "flammability", (void*)INS_FLAMMABILITY );
		translation_table.Add( "material", (void*)INS_MATERIAL );
		translation_table.Add( "physics", (void*)INS_PHYSICS );
		translation_table.Add( "emitter", (void*)INS_EMITTER );
		translation_table.Add( "passable", (void*)INS_PASSABLE );
		translation_table.Add( "collision", (void*)INS_COLLISION );
		translation_table.Add( "item", (void*)INS_ITEM );
		translation_table.Add( "explosive", (void*)INS_EXPLOSIVE );
		translation_table.Add( "crop_conditions", (void*)INS_CROP_CONDITIONS );
		translation_table.Add( "crop_yield", (void*)INS_CROP_YIELD );
		translation_table.Add( "crop_resist_pests", (void*)INS_CROP_RESIST_PESTS );
		translation_table.Add( "crop_resist_disease", (void*)INS_CROP_RESIST_DISEASE );
		translation_table.Add( "crop_water_comfort", (void*)INS_CROP_WATER_COMFORT );
		translation_table.Add( "crop_hunger", (void*)INS_CROP_HUNGER );
		translation_table.Add( "spring", (void*)INS_SIDES_SPRING );
		translation_table.Add( "friction", (void*)INS_SIDES_FRICTION );
		translation_table.Add( "surfacetype", (void*)INS_SIDES_SURFACETYPE );
		translation_table.Add( "damage", (void*)INS_SIDES_DAMAGE );
		translation_table.Add( "holy", (void*)INS_SIDES_HOLY );
		translation_table.Add( "texture", (void*)INS_SIDES_TEXTURE );
		translation_table.Add( "shielded", (void*)INS_SIDES_SHIELDED );
		translation_table.Add( "movebonus", (void*)INS_SIDES_MOVEBONUS );
		translation_table.Add( "lightabsorb", (void*)INS_SIDES_LIGHTABSORB );
		translation_table.Add( "paintable", (void*)INS_SIDES_PAINTABLE );

		translation_table_setup = true;
	}

	int TranslateInstruction( const char *instruction ) {
		SetupTranslationTable();
		return (int)translation_table.Query( instruction );
	}

	bool CheckDuplicate( CubeKernelParserData *ud, int hash ) {
		for( int i = 0; i < ud->classes.size(); i++ ) {
			if( ud->classes[i].namehash == hash ) {
				return true;
			}
		}

		for( int i = 0; i < ud->instances.size(); i++ ) {
			if( ud->instances[i].namehash == hash ) {
				return true;
			}
		}
		return false;
	}

#define FIRE_SCRIPT_ERROR( desc ) {printf( "script error, line %d: %s\n", data->script_line, desc ); return false;}
#define EXPECTED_ARGS( count ) if( data->argc<(count+1) ) {printf( "script error, line %d: instruction expects at least %d arguments\n", data->script_line, count ); return false; }

	//-------------------------------------------------------------------------------------------------
	bool ApplyLightInstruction( const Util::ScriptParserData *data, void *user ) {
		EXPECTED_ARGS(1);
		return true;
	}

	//-------------------------------------------------------------------------------------------------
	bool ScriptParserFunc( const Util::ScriptParserData *data, void *user ) {
		CubeKernelParserData *ud = (CubeKernelParserData*)user;

		if( data->type == Util::SCRIPT_SPACE ) {
			return true;
		}

		if( data->type == Util::SCRIPT_ERROR ) {
			// todo: print error
			if( data->errdata >= ' ' && data->errdata <= 126 ) {

				printf( "script parsing error, line %d, col %d, '%c'\n", data->script_line, data->errcol, data->errdata );
			} else {
				printf( "script parsing error, line %d, col %d\n", data->script_line, data->errcol );
			}
			return false;
		}
		 

		if( data->type == Util::SCRIPT_LABEL ) { 
			bool error = false;
			if( data->args[0] == 0 ) {
				error = true;
			}
			if( data->args[0][0] == 0 ) {
				error = true;
			}
			if( error ) {
				FIRE_SCRIPT_ERROR( "error parsing label" );
				//printf( "script error parsing label, line %d\n", data->script_line );
				//return false;
			}

			bool isclass = false;

			if( data->args[0][0] == '.' ) {
				isclass = true;
			}

			CubeProperties cpnew;
			strcpy_s<sizeof(cpnew.name)>( cpnew.name, data->args[0] );
			cpnew.namehash = Util::HashString( cpnew.name, false );
			if( CheckDuplicate( ud, cpnew.namehash ) ) {
				return false;
			} 

			if(isclass ){
			
				ud->classes.push_back(cpnew);
				ud->current = &ud->classes.back();
			} else {

				ud->instances.push_back(cpnew);
				ud->current = &ud->instances.back();
			
			}
		

		} else if( data->type == Util::SCRIPT_INSTRUCTION ) {
			if( !ud->current ) {
				FIRE_SCRIPT_ERROR( "not expecting an instruction" );
				//printf( "(%d) script error: not expecting an instruction\n", data->script_line  );
				//return false;
			}

			if( data->args[0] == 0 ) {
				FIRE_SCRIPT_ERROR( "unknown error (instruction)" );
				//printf( "(%d) script error ?\n", data->script_line );
				//return false;
			}

			int instruction_index = TranslateInstruction( data->args[0] );

			if( instruction_index == -1 ) {
				FIRE_SCRIPT_ERROR( "unknown instruction" );
				//printf( "(%d) script error: unknown instruction\n", data->script_line  );
				return false;
			}

			switch( instruction_index ) {
			case INS_DESCRIPTION:
				EXPECTED_ARGS(1);
				strcpy_s( ud->current->description, data->args[1] );
				break;
			case INS_COST:
				EXPECTED_ARGS(1);
				ud->current->cost = Util::StringToInt( data->args[1] );
				break;
			case INS_CLUTTER:
				EXPECTED_ARGS(1);
				ud->current->clutter = Util::StringToBool( data->args[1] );
				break;
			case INS_LIGHT:
				return ApplyLightInstruction( data, &ud );

			case INS_BLASTRESIST:
			case INS_FLAMMABILITY:
			case INS_MATERIAL:
			case INS_PHYSICS:
			case INS_EMITTER:
			case INS_PASSABLE:
			case INS_COLLISION:
			case INS_ITEM:
			case INS_EXPLOSIVE:
			case INS_CROP_CONDITIONS:
			case INS_CROP_RESIST_PESTS:
			case INS_CROP_RESIST_DISEASE:
			case INS_CROP_WATER_COMFORT:
			case INS_CROP_HUNGER:
			case INS_SIDES_SPRING:
			case INS_SIDES_FRICTION:
			case INS_SIDES_SURFACETYPE:
			case INS_SIDES_DAMAGE:
			case INS_SIDES_HOLY:
			case INS_SIDES_TEXTURE:
			case INS_SIDES_SHIELDED:
			case INS_SIDES_MOVEBONUS:
			case INS_SIDES_LIGHTABSORB:
			case INS_SIDES_PAINTABLE:
				printf( "notice, line %d: this instruction isnt supported yet\n", data->script_line );
				return true;
			}
		}
	
		return true;
	}

}

//-------------------------------------------------------------------------------------------------
CubeKernel::CubeKernel( const char *p_file ) {
	// todo: parse file
	
	CubeKernelParserData parser_data;
	parser_data.current = 0;
	parser_data.parent = this;
	
	Util::ScriptParser parser( p_file, ScriptParserFunc, &parser_data );

	// bake data
	props = new CubeProperties[parser_data.instances.size()];
	for( int i = 0; i < parser_data.instances.size(); i++ ) {
		props[i] = parser_data.instances[i];
		st_props.Add( props[i].name, &props[i] );
	}
}

//-------------------------------------------------------------------------------------------------
CubeKernel::~CubeKernel() {
	if( props ) {
		delete[] props;
	}
}

//-------------------------------------------------------------------------------------------------
const CubeProperties *CubeKernel::GetData( int index ) const {
	return &props[index];
}

//-------------------------------------------------------------------------------------------------
int CubeKernel::FindDataIndex( const char *name ) const {
	int hash = Util::HashString( name );
	for( int i = 1; i < num_types; i++ ) {
		if( props[i].namehash == hash ) {
			return i; 
		}
	}
	return -1;
}
//icedance
//Rory Gallagher feat. Sean Ryan 	Remember Me

}
