//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#include "stdafx.h"

#if 0

namespace Ingame{
extern Players::Player test;
}

namespace clouds {

Video::VertexBuffer vb_clouds;
float mu;

#define datasize 256
#define SEED_MASK 16383

float cloud_plane = 800.0;

u8 perlin_wip[datasize*datasize];

int writex, writey;
int gen_offset;
int next_texture_slot;

float cloud_density;
float cloud_color[4];
float centerx, centerz;

cml::vector2f wind;
cml::vector2f cloud_offset;

int current_texture_slot;

u8 random_data[16384];

//-------------------------------------------------------------------------------------------------
double sample_2d_cosine_hq( int x, int y, int mask, int offset ) {
//-------------------------------------------------------------------------------------------------
	x &= mask;
	y &= mask;
    int x2 = (x+65536) & mask;
	int y2 = (y+65536) & mask;

	double a = random_data[(offset+(x>>16)+(y>>16)*191) & SEED_MASK];
	double b = random_data[(offset+(x2>>16)+(y>>16)*191) & SEED_MASK];
	double c = random_data[(offset+(x>>16)+(y2>>16)*191) & SEED_MASK];
	double d = random_data[(offset+(x2>>16)+(y2>>16)*191) & SEED_MASK];

	x = (32768 - cos_table[(x&65535)>>6]) >> 1;
	y = (32768 - cos_table[(y&65535)>>6]) >> 1;
	double dx = (double)(x) / 32768.0;
	double dy = (double)(y) / 32768.0;
	
	a += ((b-a)*dx);
	c += ((d-c)*dx);
	return (a + ((c-a)*dy));
}

//-------------------------------------------------------------------------------------------------
double perlin( int x, int y, int offset ) {
//-------------------------------------------------------------------------------------------------
	double a = 0;

	int scale1 = 32;

	int freq = 1 * 8192/32 * 4 * 1024/datasize;
	double amp = 32.0;
	double normal = 0.0;

	int iterations = 10;
	
	for( int i = 0; i < iterations; i++ ) {
		a += sample_2d_cosine_hq( x * freq, y * freq, datasize*freq-1, offset ) * amp;
		normal += amp;
		freq *= 2;
		amp *= 0.5; // persistence

	}

	// normalize
	a *= 1.0 / ((normal)*256.0);

	return a;
}

//-------------------------------------------------------------------------------------------------
void generate_perlin( int count ) {
//-------------------------------------------------------------------------------------------------
	if( count < 0 ) {
		count = datasize*datasize;	

	}

	if( writey >= datasize ) return;

	while( count != 0 ) {
		double a = perlin( writex, writey, gen_offset );
		int b = (int)floor(a*255);
		perlin_wip[writex+writey*datasize] = (u8)b;

		writex++;
		if( writex >= datasize ) {
			writex =0;
			writey++;
			if( writey >= datasize ) return;
		}

		count--;
	}
}

//-------------------------------------------------------------------------------------------------
void send_texture() {
//-------------------------------------------------------------------------------------------------
	Textures::UpdateTextureFromMemoryRED( Textures::CLOUDS1+next_texture_slot, perlin_wip, datasize, datasize );
	next_texture_slot++;
	if( next_texture_slot > 2 ) next_texture_slot = 0;
	writex=0;
	writey=0;
	gen_offset = rnd::next();
}

//-------------------------------------------------------------------------------------------------
void prime() {
//-------------------------------------------------------------------------------------------------
	generate_perlin(-1);
	send_texture();
	generate_perlin(-1);
	send_texture();
}

//-------------------------------------------------------------------------------------------------
void set_density( float d ) {
//-------------------------------------------------------------------------------------------------
	cloud_density = d;
}

//-------------------------------------------------------------------------------------------------
void set_color( float r, float g, float b ) {
//-------------------------------------------------------------------------------------------------
	cloud_color[0] = r;
	cloud_color[1] = g;
	cloud_color[2] = b;
}

//-------------------------------------------------------------------------------------------------
void set_light( float light ) {
//-------------------------------------------------------------------------------------------------
	cloud_color[3] = light;
}

//-------------------------------------------------------------------------------------------------
void set_wind( float x, float y ) {
//------------------------------------------------------------------------------------------------
	wind[0] = x;
	wind[1] = y;
}

//-------------------------------------------------------------------------------------------------
void init() {
//-------------------------------------------------------------------------------------------------
	vb_clouds.Create( Video::VF_GENERIC );
	
	Video::point_vertex_2d verts[4]; // MOVE THIS BACK 88 DEBUG

	verts[0].x = -1.0;
	verts[0].y = 1.0;
	verts[1].x = -1.0;
	verts[1].y = -1.0;
	verts[2].x = 1.0;
	verts[2].y = -1.0;
	verts[3].x = 1.0;
	verts[3].y = 1.0;
	/*
	verts[0].x = -5000;
	verts[0].z = -5000;
	verts[1].x = -5000;
	verts[1].z = 5000;
	verts[2].x = 5000;
	verts[2].z = 5000;
	verts[3].x = 5000;
	verts[3].z = -5000;

	verts[0].y = verts[1].y = verts[2].y = verts[3].y = 0;
	
	verts[0].r = verts[0].g = verts[0].b = verts[0].a = 255;
	verts[1].r = verts[1].g = verts[1].b = verts[1].a = 255;
	verts[2].r = verts[2].g = verts[2].b = verts[2].a = 255;
	verts[3].r = verts[3].g = verts[3].b = verts[3].a = 255;

	verts[0].u = 0.0;
	verts[0].v = 0.0;
	
	verts[1].u = 0.0;
	verts[1].v = 8.0;

	verts[2].u = 8.0;
	verts[2].v = 8.0;
	
	verts[3].u = 8.0;
	verts[3].v = 0.0;
	*/
	vb_clouds.BufferData( verts, 4 * sizeof(Video::point_vertex_2d), GL_STATIC_DRAW_ARB );

	mu = 0;

	cloud_offset = cml::vector2f(0.0f,0.0f);

	writex=0;
	writey=0;
	next_texture_slot=0;
	current_texture_slot=0;

	

	set_density(0);
	set_color(1,1,1);
	set_wind(0.0001f, 0.00005f);
	//set_wind(0.001, 0.0005); // fast

	for( u32 i = 0; i < 16384; i++ ) {
		random_data[i] = (u8)rnd::next();
	}

	
	prime();

	Textures::SetTextureFiltering( Textures::CLOUDS1, true );
	Textures::SetTextureFiltering( Textures::CLOUDS2, true );
	Textures::SetTextureFiltering( Textures::CLOUDS3, true );

	Textures::SetTextureWrapping( Textures::CLOUDS1, true );
	Textures::SetTextureWrapping( Textures::CLOUDS2, true );
	Textures::SetTextureWrapping( Textures::CLOUDS3, true );

}

float debug1=0;
//-------------------------------------------------------------------------------------------------
void update() {
//-------------------------------------------------------------------------------------------------
	int frames = gametime::frames_passed();

	cloud_offset += wind * (float)frames;

	generate_perlin(frames*100000);
	if( writey >= datasize ) {
		if( current_texture_slot != next_texture_slot ) {
			send_texture();
		}
	}

	debug1 += 0.01f;
	set_density((sin(debug1)+1.0f)/4.0f);
	set_density((float)fmod(abs(Ingame::test.angle/8.0), 1.0));
	//set_density(0.3);
	//set_density(0.1);

	if( frames == 0 ) return;
	float t =(float)frames;
	mu += t * 0.0001f;
	//mu += t * 0.01f;
	if( mu >= 1.0f ) {
		mu = 1.0f;
		if( current_texture_slot == next_texture_slot ) {
			mu = 0.0f;
			current_texture_slot++;
			if( current_texture_slot > 2 ) current_texture_slot = 0;

		}
	}
}

//-------------------------------------------------------------------------------------------------
void set_center( float x, float z ) {
//-------------------------------------------------------------------------------------------------
	centerx=x;
	centerz=z;
}

//-------------------------------------------------------------------------------------------------
void render() {
//-------------------------------------------------------------------------------------------------
	Video::Shaders::CloudShader *shader = Video::Shaders::Find( "clouds" );
	shader->Use();
	shader->SetPlanes( Video::GetCamera(), Video::GetFarPlane() );
	shader->SetSamplers( 0, 1 );
	shader->SetMu( (float)cosinterp[(int)(mu*256.0f)] / 256.0f );
	shader->SetColor( cloud_color[0] * cloud_color[3], cloud_color[1] * cloud_color[3], cloud_color[2] * cloud_color[3] );
	shader->SetDensity( cloud_density );
	shader->SetTextureOffset( cloud_offset[0] + centerx*0.001f, cloud_offset[1]+centerz*0.001f );
//	Video::SetShader( Video::SHADER_CLOUDS );

	//Video::SetCloudShaderProjection();
	
	//Video::SetCloudShaderSamplers( 0, 1 );
	//Video::SetCloudShaderMu( (float)cosinterp[(int)(mu*256.0f)] / 256.0f );// mu );
	
	//Video::SetCloudShaderTint( cloud_color[0] * cloud_color[3], cloud_color[1] * cloud_color[3], cloud_color[2] * cloud_color[3] );
	//Video::SetCloudShaderDensity( cloud_density );
	//Video::SetCloudShaderTextureOffset( cloud_offset[0] + centerx*0.001f, cloud_offset[1]+centerz*0.001f );

	//Video::SetCloudShaderTranslate( centerx, cloud_plane,centerz );

	Video::SetActiveTextureSlot(1);
	Textures::Bind( Textures::CLOUDS1 + ((current_texture_slot+1)%3 ) );
	Video::SetActiveTextureSlot(0);
	Textures::Bind( Textures::CLOUDS1 + current_texture_slot );
	
	vb_clouds.Bind();

	shader->SetVertexAttributePointers();
//	Video::SetShaderVertexAttributePointers();

	Video::SetDepthBufferMode( Video::ZBUFFER_DISABLED );
	Video::SetBlendMode( Video::BLEND_ALPHA );

	
	glDisable( GL_CULL_FACE );
	Video::DrawQuads( 0, 4 ) ;
	glEnable( GL_CULL_FACE );

	Video::SetDepthBufferMode( Video::ZBUFFER_ENABLED );
	
}
	
}

#endif
