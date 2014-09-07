//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#include "stdafx.h"

#if 0

namespace particles {

#define MAX_PARTS 8192
#define MAX_PARTS_MASK 8191

// NOTICE:
// TO ADD NEW KERNEL PARAMETER
// 1) ADD ENTRY IN PARTICLE KERNEL
// 2) ADD ENTRY IN PARAM ENUM
// 3) ADD ENTRY IN TRANSLATION TABLE
// 4) ADD CODE IN KERNEL INITIALIZATION
// 5) ADD CODE IN XML ATTRIBUTE SWITCH
// 6) ADD CODE IN PARTICLE ROUTINE


typedef struct t_particle_kernel {
	float u, v, w, h;

	float emitter_x,emitter_y,emitter_z;

	float velx_min;		// velocity ranges of particle (meters/256 units)
	float velx_max;
	float vely_min;
	float vely_max;
	float velz_min;
	float velz_max;

	float velar;
	float rotar;

	float gravity;			// floating point now xxxxxxxxxxxxxvel.y += gravity each frame (meter/256 units)
	
	float angle_min;	// DEGREES
	float angle_max;	// DEGREES
	float rot_min;		
	float rot_max;

	u16 size_min;		// size of particle (meters/1024 units)
	u16	size_max;		//

	s16 sizeot_min;		// size over time meters/1024 units
	s16 sizeot_max;

	u8 animation_speed_min;	// frame/128 units
	u8 animation_speed_max;

	u16 lifetime_min;		// in 1000/60 millisecond units
	u16 lifetime_max;		//

	


	u16 alpha_min;		// particle starting alpha/lum 1.0/32768 units
	u16 alpha_max;		//
	s16 alphafade;		// alpha/lum fade setting

	u8 blend;			// ~0=alpha, 0=lum

	u8 static_frame_min;		// starting frame (for static mode)
	u8 static_frame_max;

	
	u8 animation_length;		// end frame
	u8 animation_wrap;			// texture wrap point (u + (w*frame % wrap)), (v + (w*frame / wrap))

	u8 r, g, b;
	
	u8 geometry; // vertical

	float height;
} particle_kernel;

particle_kernel *kernels;

typedef struct t_particle {
	float pos_x, pos_y, pos_z;
	float vel_x,vel_y, vel_z;
	float angle, rot;
	//int vel_x, vel_y, vel_z; // meters/65536
	u16 size;
	s16 sizeot;
	u16 animation;	// frame in 1/128 units
	u16 life;
	u8 animation_speed;
 
	u16 alpha; // 32768 = 1.0, >32768 = 1.0 (clamp!)
	u32 light;
	u8 kernel;
	
} particle;

struct translation {
	int index;
	int param1;
	const char *name;
};

struct parameters {
	
	std::vector<std::string> params;
};

enum {
	PARAM_NULL,
	PARAM_VEL,PARAM_GRAVITY,PARAM_ANGLE,PARAM_ROT,PARAM_SIZE,PARAM_SIZEOT,
	PARAM_HEIGHT,PARAM_BLEND,PARAM_ALPHA,PARAM_ALPHAOT,PARAM_STATICFRAME,
	PARAM_ANISPEED,PARAM_ANILENGTH,PARAM_ANIWRAP,PARAM_LIFE,PARAM_TEXTURE,
	PARAM_COLOR,PARAM_VERTICAL,PARAM_CONVERT,PARAM_BOUNCE,PARAM_VELAR,
	PARAM_ROTAR,PARAM_EMIT
};

translation parameter_name_translations[] = { 
	{PARAM_NULL,0,""},
	{PARAM_VEL, 0, "vel"},
	{PARAM_GRAVITY, 0, "gravity"},
	{PARAM_ANGLE, 0, "angle"},
	{PARAM_ROT,0,"rot"},
	{PARAM_SIZE,0,"size"},
	{PARAM_SIZEOT,0,"sizeot"},
	{PARAM_HEIGHT,0,"height"},
	{PARAM_BLEND,0,"blend"},
	{PARAM_ALPHA,0,"alpha"},
	{PARAM_ALPHAOT,0,"alphaot"},
	{PARAM_STATICFRAME,0,"staticframe"},
	{PARAM_ANISPEED,0,"anispeed"},
	{PARAM_ANILENGTH,0,"anilength"},
	{PARAM_ANIWRAP,0,"aniwrap"},
	{PARAM_LIFE,0,"life"},
	{PARAM_TEXTURE,0,"texture"},
	{PARAM_COLOR,0,"color"},
	{PARAM_VERTICAL,0,"vertical"},
	{PARAM_CONVERT,0,"convert"},
	{PARAM_BOUNCE,0,"bounce"},
	{PARAM_VELAR,0,"velar"},
	{PARAM_ROTAR,0,"rotar"},
	{PARAM_EMIT,0,"emit"},
	{-1,0,""}
};

translation blend_mode_translations[] = {
	{Video::BLEND_OPAQUE,0,"solid"},
	{Video::BLEND_OPAQUE,0,"opaque"},
	{Video::BLEND_ALPHA,0,"alpha"},
	{Video::BLEND_ADD,0,"add"},
	{Video::BLEND_SUB,0,"sub"},
	{-1,0,""}
};

translation boolean_translation[] = {
	{0,0,"false"},
	{0,0,"no"},
	
	{1,0,"true"},
	{1,0,"yes"},

	{0,0,"off"},
	{1,0,"on"},

	{0,0,"negative"},
	{1,0,"positive"},

	{-1,0,""}
};

translation * translate( const char * str, translation *table ) {
	for( int i = 0; table[i].index != -1; i++ ) {
		if( strcmp(table[i].name,str) == 0 ) {
			return &table[i];
		}
	}
	return 0;
}

int translation_simple( const char *str, translation *table ) {
	translation *t = translate(str,table);
	if( t ) return t->index;
	else return 0;
}

float decimal_from_string( const char *s ) {
	return (float)atof(s);

	/*
	// s = trimmed string "123.456"
	float negative = 1.0;
	if( s[0] == '-' ) {
		negative = -1.0;
		s++;
	}
	float f = 0.0;

	float e = -1.0;

	float d = 0.1f;
	int i = 0;
	for( ; s[i]; i++ ) {
		if( s[i] == '.' ) {
			if( e >= 0 ) return 0.0f; // error 
			e = i;
			continue;
		}

		if( s[i] < '0' || s[i] > '9' ) return 0.0f; // error

		float digit = (float)s[i] - '0';
		f = f + digit * d;
		d = d * 0.1;

	}
	if( e < 0.0 ) {
		e = i;
	}

	f = f * pow(10,e) * negative;

	return f;
	*/
}

int tofixed( float value, int fraction, int clamp ) {
	int v = (int)floor(value * (float)fraction + 0.5);
	if( v < -clamp ) v = -clamp;
	if( v > clamp ) v = clamp;
	return v;
}

int tofixedf( float value, float fraction, int clamp ) {
	int v = (int)floor(value * fraction + 0.5);
	if( v < -clamp ) v = -clamp;
	if( v > clamp ) v = clamp;
	return v;
}

int tofixeduf( float value, float fraction, int clamp ) {
	int v = (int)floor(value * fraction + 0.5);
	if( v < 0 ) v = 0;
	if( v > clamp ) v = clamp;
	return v;
}

int tofixedu( float value, int fraction, int clamp ) {
	int v = (int)floor(value * (float)fraction + 0.5);
	if( v < 0 ) v = 0;
	if( v > clamp ) v = clamp;
	return v;
}

void parse_params( parameters &result, const char *text ) {

	char copy[200];
	for( int i = 0; i < 200; i++ ) {
		copy[i] = text[i];
		if( text[i] == 0 ) break;
	}
	copy[199] = 0;

	result.params.clear();
	char *a;
	a = strtok(copy," ,/|");
	while(1) {
		result.params.push_back(a);
		a = strtok(0," ,/|");
		if( !a ) break;
	}
	
}

int hex2num(char c) {
	if( c >= '0' && c <= '9' ) return c - '0';
	else if( c >= 'a' && c <= 'f' ) return c - 'a' + 10;
	else if( c >= 'A' && c <= 'F' ) return c - 'A' + 10;
	else return 0;
}

void parse_hexcode( const char *hex, u8 &r, u8 &g, u8 &b ) {
	u8 result[3];
	for( int i = 0; i < 3; i++ ) {
		int a,b;
		a = hex2num(hex[i*2+0]);
		b = hex2num(hex[i*2+1]);
		result[i] = a*16+b;
	}
	r = result[0];
	g = result[1];
	b = result[2];
}

void copy_kernel( particle_kernel *dest, particle_kernel *source ) {
	memcpy( dest, source, sizeof( particle_kernel ) );
}

/*

void parse_param_table_input( parameters &result, const char *text ) {
	result.params.clear();

}*/

//std::vector<translation> translations;

/*
void add_translation( int index, const char *name ) {

}

void setup_translations() {
	translations.clear();
	add_translation(
}
*/


int first=0, count=0;
particle parts[MAX_PARTS];

void initialize( particle_kernel &k ) {
	k.alphafade = 0;
	k.alpha_max = k.alpha_min = 32768;
	k.angle_max = k.angle_min = 0;
	k.animation_length = 1;
	k.animation_speed_max = k.animation_speed_min = 0;
	k.animation_wrap = 1;
	k.blend = Video::BLEND_OPAQUE;
	k.emitter_x = k.emitter_y = k.emitter_z = 0;
	k.geometry = 0;
	k.gravity = 0;
	k.height = 1.0;
	k.lifetime_max = k.lifetime_min = 300;
	k.r = k.g = k.b = 255;
	k.rot_max = k.rot_min = 0;
	k.rotar=1.0f;
	k.sizeot_max = k.sizeot_min = 0;
	k.size_max = k.size_min = 1024;
	k.static_frame_max = k.static_frame_min = 0;
	k.u = k.v = 0; k.w = k.h = 1;
	k.velar = 1.0f;
	k.velx_min = k.velx_max = k.vely_min = k.vely_max = k.velz_min = k.velz_max = 0;
	
}



void load_kernels() {
	TiXmlDocument doc("particles.xml");
	doc.LoadFile();

	TiXmlNode* kl = doc.FirstChild("kernels");
	TiXmlNode* node;
	std::vector<particle_kernel> list;

	float texture_scale = 1.0f/1024.0f;
	
	for( node = kl->FirstChild("kernel"); node; node = node->NextSibling("kernel") ) {
		
		particle_kernel k;
		initialize(k);
		TiXmlElement *e;
		e = node->ToElement();
		for( TiXmlAttribute *attr = e->FirstAttribute(); attr; attr = attr->Next() ) {
			translation *t = translate(attr->Name(), parameter_name_translations );
			if( t == 0 ) continue;

			parameters params;
			parse_params( params, attr->Value() );

			

			switch( t->index ) {
			case PARAM_VEL:
				switch( params.params.size() ) {
				case 1: 
					{float a = decimal_from_string(params.params[0].c_str());
				 
					k.velx_min = k.vely_min = k.velz_min = -a;
					k.velx_max = k.vely_max = k.velz_max = a;
					} break;
				case 2: 
					{float a = decimal_from_string(params.params[0].c_str());
					
					float b = decimal_from_string(params.params[1].c_str());
					
					k.velx_min = k.vely_min = k.velz_min = a;
					k.velx_max = k.vely_max = k.velz_max = b;
					}break;
				case 3:
					{float a = decimal_from_string(params.params[0].c_str());
					
					float b = decimal_from_string(params.params[1].c_str());
					
					float c = decimal_from_string(params.params[2].c_str());
					
					k.velx_min = -a; k.velx_max = a;
					k.vely_min = -b; k.vely_max = b;
					k.velz_min = -c; k.velz_max = c;
					}break;
				case 6:
					{float a = (decimal_from_string(params.params[0].c_str()));
					float b = (decimal_from_string(params.params[1].c_str()));
					float c = (decimal_from_string(params.params[2].c_str()));
					float d = (decimal_from_string(params.params[3].c_str()));
					float e = (decimal_from_string(params.params[4].c_str()));
					float f = (decimal_from_string(params.params[5].c_str()));
 					k.velx_min = a; k.velx_max = b; k.vely_min = c; k.vely_max = d; k.velz_min = e; k.velz_max = f;
					}break;
				default:
					// error
					break;
				}
				break;
			case PARAM_GRAVITY:
				
				k.gravity = (decimal_from_string(params.params[0].c_str()));
				break;

			case PARAM_ANGLE:
				if( params.params.size() == 1 ) {
					k.angle_min = decimal_from_string(params.params[0].c_str());
					k.angle_max = k.angle_min;
				} else if( params.params.size() == 2 ) {
					k.angle_min = decimal_from_string(params.params[0].c_str());
					k.angle_max =decimal_from_string(params.params[1].c_str()) ;
				} else {
					// error
				}
				break;
			case PARAM_ROT:
				if( params.params.size() == 1 ) {
					k.rot_max = ( decimal_from_string(params.params[0].c_str()) );
					k.rot_min = -k.rot_max;
				} else if( params.params.size() == 2 ) {
					k.rot_min = ( decimal_from_string(params.params[0].c_str()) );
					k.rot_max = ( decimal_from_string(params.params[1].c_str()) );
				} else {
					// error
				}
				break;
			case PARAM_SIZE:
				
				if( params.params.size() == 1 ) {
					k.size_max = tofixedu( decimal_from_string(params.params[0].c_str()),1024,65535);
					k.size_min = k.size_max;
				} else if( params.params.size() == 2 ) {
					k.size_min = tofixedu( decimal_from_string(params.params[0].c_str()),1024,65535);
					k.size_max = tofixedu( decimal_from_string(params.params[1].c_str()),1024,65535);
				} else {
					// error
				}
				break;
			case PARAM_SIZEOT:
				if( params.params.size() == 1 ) {
					k.sizeot_max = tofixed( decimal_from_string(params.params[0].c_str()),1024,32767);
					k.sizeot_min = k.sizeot_max;
				} else if( params.params.size() == 2 ) {
					k.sizeot_min = tofixed( decimal_from_string(params.params[0].c_str()),1024,32767);
					k.sizeot_max = tofixed( decimal_from_string(params.params[1].c_str()),1024,32767);
				} else {
				}
				break;
			case PARAM_HEIGHT:
				k.height = decimal_from_string(params.params[0].c_str());
				break;
			case PARAM_BLEND:
				{translation *t2 = translate( params.params[0].c_str(), blend_mode_translations );
				 k.blend = t2->index;
				}
				break;
			case PARAM_ALPHA:
				if( params.params.size() == 1 ) {
					k.alpha_min = k.alpha_max = tofixedu(decimal_from_string(params.params[0].c_str()),32768, 65535);
				} else if( params.params.size() == 2 ) {
					k.alpha_min = tofixedu(decimal_from_string(params.params[0].c_str()),32768,65535);
					k.alpha_max = tofixedu(decimal_from_string(params.params[1].c_str()),32768,65535);
				} else {
					// error
				}
				break;
			case PARAM_ALPHAOT:
				k.alphafade = tofixed(decimal_from_string(params.params[0].c_str()),32768, 32767);
				break;
			case PARAM_STATICFRAME:
				if( params.params.size() == 1 ) {
					k.static_frame_min = k.static_frame_max = (int)decimal_from_string(params.params[0].c_str());
				} else if( params.params.size() == 2 ) {
					k.static_frame_min = (int)decimal_from_string(params.params[0].c_str());
					k.static_frame_max = (int)decimal_from_string(params.params[1].c_str());
				} else {}
				
				break;
			case PARAM_ANISPEED:
				if( params.params.size() == 1 ) {
					k.animation_speed_min = k.animation_speed_max = tofixedu(decimal_from_string(params.params[0].c_str()),128,255);
				} else {
					k.animation_speed_min = tofixedu(decimal_from_string(params.params[0].c_str()),128,255);
					k.animation_speed_max = tofixedu(decimal_from_string(params.params[1].c_str()),128,255);
				}
				break;
			case PARAM_ANILENGTH:
				k.animation_length = tofixedu( decimal_from_string(params.params[0].c_str()),1,255);
				break;
			case PARAM_ANIWRAP:
				k.animation_wrap = tofixedu( decimal_from_string(params.params[0].c_str()),1,255);
				break;
			case PARAM_LIFE:
				if( params.params.size() == 1 ) {
					k.lifetime_min = k.lifetime_max = tofixeduf(decimal_from_string(params.params[0].c_str()),60,65535);
				} else if( params.params.size() == 2 ) {
					k.lifetime_min = tofixeduf(decimal_from_string(params.params[0].c_str()),60,65535);
					k.lifetime_max = tofixeduf(decimal_from_string(params.params[1].c_str()),60,65535);
				}
				break;
			case PARAM_TEXTURE:
				if( params.params.size() == 3 || params.params.size() == 4 ) {
					k.u = decimal_from_string(params.params[0].c_str()) * texture_scale;
					k.v = decimal_from_string(params.params[1].c_str()) * texture_scale;
					k.w = decimal_from_string(params.params[2].c_str());
					k.h = k.w * texture_scale;
					k.w = k.w * texture_scale; // done afterward in case texture becomes nonsquare
				}
				if( params.params.size() == 4 ) {
					k.h = decimal_from_string(params.params[3].c_str()) * texture_scale;
				
				}
				break;
			case PARAM_COLOR:
				if( params.params.size() == 1 ) {
					parse_hexcode( params.params[0].c_str(), k.r, k.g, k.b );
				} else if( params.params.size() == 3 ) {
					k.r = tofixeduf( decimal_from_string(params.params[0].c_str()), 255.0f, 255);
					k.g = tofixeduf( decimal_from_string(params.params[1].c_str()), 255.0f, 255);
					k.b = tofixeduf( decimal_from_string(params.params[2].c_str()), 255.0f, 255);
				} else {
					// error
				} 
				break;
			case PARAM_VERTICAL:
				k.geometry = translation_simple( params.params[0].c_str(), boolean_translation );
				break;
			case PARAM_CONVERT:
				// nyi
				break;
			case PARAM_BOUNCE:
				// nyi
				break;
			case PARAM_VELAR:
				// nyi
				k.velar = 1.0f-decimal_from_string( params.params[0].c_str() ) ;
				break;
			case PARAM_ROTAR:
				k.rotar = 1.0f-decimal_from_string( params.params[0].c_str() );
				// nyi
				break;
			case PARAM_EMIT:
				if( params.params.size() == 1) {
					k.emitter_x = k.emitter_y = k.emitter_z = decimal_from_string(params.params[0].c_str()) ;
					
				} else if( params.params.size() == 3 ) {
					k.emitter_x = decimal_from_string(params.params[0].c_str()) ;
					k.emitter_y = decimal_from_string(params.params[1].c_str()) ;
					k.emitter_z = decimal_from_string(params.params[2].c_str()) ;
				}
				break;
			}
		}

		list.push_back(k);
	}

	kernels = new particle_kernel[list.size()];

	for( u32 i = 0; i < list.size(); i++ ) {
		copy_kernel( kernels+i, &list[i] );
	}
}

void reset() {
	first=0;
	count=0;
	// blahbalh
}



void init() {
	load_kernels();
	reset();
}

int random_lerp( int low, int high ) {
	return low + (((high-low) * planet::random_number())>>8);
}

float random_lerpf( float low, float high ) {
	return low + (high-low) * (float)planet::random_number() / 256.0f;
}

void add( u8 kernel, cml::vector3f pos, cml::vector3f velocity ) {
	
	particle *p = parts + ((first+count)&MAX_PARTS_MASK);
	if( count == MAX_PARTS ) {
		first = (first+1) & MAX_PARTS_MASK;
	} else {
		count++;
	}
	p->pos_x = (pos[0]);
	p->pos_y = (pos[1]);
	p->pos_z = (pos[2]);

	particle_kernel *k = &kernels[kernel];

	p->pos_x += random_lerpf( -k->emitter_x, k->emitter_x );
	p->pos_y += random_lerpf( -k->emitter_y, k->emitter_y );
	p->pos_z += random_lerpf( -k->emitter_z, k->emitter_z );

	p->vel_x = random_lerpf( k->velx_min, k->velx_max ) + velocity[0];
	p->vel_y = random_lerpf( k->vely_min, k->vely_max ) + velocity[1];
	p->vel_z = random_lerpf( k->velz_min, k->velz_max ) + velocity[2];

	p->rot = random_lerpf( k->rot_min, k->rot_max );
	p->angle = random_lerpf(k->angle_min,k->angle_max);

	p->size = random_lerp( k->size_min, k->size_max );
	p->sizeot = random_lerp( k->sizeot_min, k->sizeot_max );
	p->animation = random_lerp( k->static_frame_min, k->static_frame_max );
	p->animation_speed = random_lerp( k->animation_speed_min, k->animation_speed_max );
	p->life = random_lerp( k->lifetime_min, k->lifetime_max );
	
	p->alpha = random_lerp( k->alpha_min, k->alpha_max );
	p->kernel = kernel;
	p->light = k->blend ? 15 : 15;


}

void add( u8 kernel, cml::vector3f pos ) {
	add( kernel, pos, cml::vector3f(0,0,0) );
}

void update_particle( particle *p, int time ) {
	float ftime = float(time);
	if( p->life ) {
		
		particle_kernel *k = kernels+p->kernel;

		int newlife = p->life - time;
		if( newlife < 0 ) {
			p->life = 0;
			return;
		}
		p->life = newlife;

		p->angle += p->rot * time;
		int s = p->size + p->sizeot * time;
		
		if( p->size < 0 ) {
			p->life=0;
			
			return;
		}
		p->size = s;
		int a = p->alpha + k->alphafade * time;
		if( a < 0 ) {
			p->life=0;
			return;
		}
		p->alpha = a > 65535 ? 65535:a;

		p->animation += p->animation_speed;
		if( (p->animation>>7) >= k->animation_length ) {
			p->life=0;
			return;
		}

		p->vel_y += k->gravity*time ;

		
		p->vel_x = p->vel_x * k->velar;
		p->vel_y = p->vel_y * k->velar;
		p->vel_z = p->vel_z * k->velar;
	
		p->rot *= k->rotar;

		p->pos_y +=  p->vel_y * ftime ;
		if( planet::collision( (int)floor(p->pos_x), (int)floor(p->pos_y), (int)floor(p->pos_z) ) ) {
			p->life=0; // bypass! (todo: switchable)

			p->pos_y = floor(p->pos_y);
			
			if( p->vel_y <= 0 ) {
				p->pos_y += 1.0078125;
			} else {
				p->pos_y -= 0.01171875;
			}
			p->vel_y = -p->vel_y / 2.0f ;
			p->vel_x /= 2.0f;
			p->vel_y /= 2.0f;
			p->vel_z /= 2.0f;
			
			p->rot /= 2.0f;
			 
		}
		
		if( p->vel_x != 0.0f ) {
			p->pos_x += p->vel_x * time;
			
			if( planet::collision( (int)floor(p->pos_x), (int)floor(p->pos_y), (int)floor(p->pos_z) ) ) {
				p->life=0;

				p->pos_x = floor(p->pos_x);
				
				// adjust
				if( p->vel_x <= 0 ) {
					p->pos_x += 1.0078125;
				} else {
					p->pos_x -= 0.01171875;
				}
				p->vel_x = -p->vel_x / 2.0f;
				
			}
		}
		
		if( p->vel_z ) {
			p->pos_z += p->vel_z * time;
			
			if( planet::collision( (int)floor(p->pos_x), (int)floor(p->pos_y), (int)floor(p->pos_z) ) ) {
				p->life=0;

				p->pos_z = floor(p->pos_z);

				if( p->vel_z <= 0 ) {
					p->pos_z += 1.0078125;
				} else {
					p->pos_z -= 0.01171875;
				}

				 
				p->vel_z = -p->vel_z / 2.0f;
				
			}
		}
		
		p->light = worldrender::compute_light_7bit( floor(p->pos_x), floor(p->pos_y), floor(p->pos_z) );
		//if( k->blend == BLEND_ALPHA || k->blend == 
		/* TODO:LIGHTING
		if( !k->blend ) {
			int cx,cy,cz;
			cx = p->pos_x>>8;
			cy = p->pos_y>>8;
			cz = p->pos_z>>8;
			planet::chunk *ch = planet::get_chunk_direct( cx>>4, cy>>4, cz>>4 );
			if( valid_chunk_addr(ch) ) {
				if( ch->data[(cx&15)+(cy&15)*256+(cz&15)*16] == planet::CELL_AIR ) {
					if(ch->instance!= 0 ) {
						u32 light = worldrender::get_light_value(ch->instance,(cx&15,cy&15,cz&15));
						
						int a = (ch->light_dim[(cx&15)+(cy&15)*256+(cz&15)*16]>>4) + (ch->light_sun[(cx&15)+(cy&15)*256+(cz&15)*16]>>4); // todo: compute proper light
						a=15;
						p->light = a;
					}
					
				}
			}
		}*/

	}
}

void update() {
	int time = (int)gametime::frames_passed();


	for( int i = first; i < first+count; i++ ) {
		update_particle( parts + (i&MAX_PARTS_MASK), time );
	}

	// cleanup list
	while( count != 0 ) {
		if( parts[first].life != 0 ) {
			break;
		}
		first = (first+1) & MAX_PARTS_MASK;
		count--;
	}
}

void draw_particle( particle *p ) {
	if( p->life ) {
		particle_kernel *k = kernels+p->kernel;
		float frame_x, frame_y;
		frame_x = (float)((p->animation >> 7) % k->animation_wrap);
		frame_y = (float)((p->animation >> 7) / k->animation_wrap);
		frame_x = frame_x*k->w + k->u;
		frame_y = frame_y*k->w + k->v;
		int alpha = p->alpha;
		if( alpha > 32767 ) alpha = 32767;
		u8 a = k->blend ? (alpha>>8)  :  255 ;


		

		float width = (float)p->size / 1024.0f;
		float height = width * k->height;

		int r,g,b;
		r = p->light & 0xFF;
		g = (p->light>>8) & 0xFF;

		b = (p->light>>16) & 0xFF;

		//r = (r*256)/12;
		//g = (g*256)/12;
		//b = (b*256)/12;

		//worldrender::apply_sunlight( r,g,b, p->light>>24 );
		//if( r > 256 ) r = 256;
		//if( g > 256 ) g = 256;
		//if( b > 256 ) b = 256;

		if( k->blend == Video::BLEND_ALPHA ) {
			
			r = (r * k->r) >> 7;
			g = (g * k->g) >> 7;
			b = (b * k->b) >> 7;
			if( r > 255 ) r =255;
			if( g > 255) g = 255;
			if( b > 255) b = 255;
			
			graphics::draw_sprite( cml::vector3f( p->pos_x, p->pos_y, p->pos_z), 
				width, height, 
				frame_x , frame_y , frame_x+k->w, frame_y+k->w, k->blend,
				r,g,b,a,
				(int)(p->angle*182.04444444444), !!k->geometry );
		} else {
			graphics::draw_sprite( cml::vector3f( p->pos_x ,  p->pos_y ,  p->pos_z ), 
				width,height,
				frame_x , frame_y , frame_x+k->w, frame_y+k->w, k->blend,
				(a*k->r) >>8,(a*k->g)>>8,(a*k->b)>>8,255, 
				(int)(p->angle*182.04444444444), !!k->geometry );
		}
	}
}

void draw() {
	for( int i = first; i < first+count; i++ ) {
		draw_particle( parts + (i&MAX_PARTS_MASK) );
	}
	
}

}

#endif
