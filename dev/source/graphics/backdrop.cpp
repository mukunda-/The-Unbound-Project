//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#include "stdafx.h"

#if 0

namespace backdrop {
	 
Video::VertexBuffer buffer[2];

enum {
	BUFFER_BACKDROP,
	BUFFER_SUNMOON
};

Video::point_vertex_2d screen[4];
Video::generic_vertex sunmoon[8];
graphics::element ebackdrop;
graphics::element esunmoon;

//bs_vertex screen[4];
//obj_vertex sun[8];
//GLuint vboID ;
//GLuint vboSun[2];

Bitmap imgSunlight;
stb_image *imgFog;

void init( ) {
	
	//glGenBuffersARB( 1, &vboID );
	//glGenBuffersARB( 2, vboSun );

	buffer[BUFFER_BACKDROP].Create( Video::VF_POINT2D );
	buffer[BUFFER_SUNMOON].Create( Video::VF_GENERIC );

	screen[0].x = 0;
	screen[0].y = 0;
	screen[1].x = 0;
	screen[1].y = 1;
	screen[2].x = 1;
	screen[2].y = 1;
	screen[3].x = 1;
	screen[3].y = 0;

	buffer[BUFFER_BACKDROP].BufferData( (void*)screen, 4 * sizeof(Video::point_vertex_2d), GL_STATIC_DRAW_ARB );
	 
	memset(&ebackdrop,0, sizeof(graphics::element));
	memset(&esunmoon,0, sizeof(graphics::element));

	//graphics::setup_element( ebackdrop, &buffer[BUFFER_BACKDROP], Video::BLEND_OPAQUE, Video::SHADER_BACKDROP, 0, 0, 0, 4, GL_QUADS );
	graphics::setup_element( esunmoon, &buffer[BUFFER_SUNMOON], Video::BLEND_OPAQUE, Video::Shaders::Find( "objects" ), 0, 0, 0, 4, GL_QUADS );  
	
	imgSunlight.load( "texture\\skycolors.bmp" );
	 
	imgFog = new stb_image("texture\\sky.png" );
}

void create_sun_sprite( float angle, int index, float lum ) {
	if(lum<0)lum=0;
	cml::vector3f cam = Video::GetCamera();
	//cml::vector3f sunprojection;

	cml::vector3f verts[4];

	for( int i = 0; i < 4; i++ ) {
		//outer_verts[i] = cml::vector3f( tablecos(i * 256 / 24)*0.01, 1.0f, tablesin(i*256/24)*0.01 );
		float x,z;
		switch(i){
		case 0:
			x=1.0;
			z=1.0;
			break;
		case 1:
			x=-1.0;
			z=1.0;
			break;
		case 2:
			x=-1.0;
			z=-1.0;
			break;
		case 3:
			x=1.0;
			z=-1.0;
			break;
		}
		verts[i] =  cml::vector3f(x*0.2f , 1.0f, z*0.2f );
		verts[i] *= 100.0;
		verts[i] =  cml::rotate_vector( verts[i], cml::vector3f( 0.0,0.0,1.0), angle  );
		verts[i] += cam;
		
	}
	
	const float tu[] = {0.0,0.0,1.0,1.0};
	const float tv[] = {0.0,1.0,1.0,0.0};
	
	for( int i = 0; i < 4; i++ ) {
		
		//add_vertex( outer_verts[i][0], outer_verts[i][1], outer_verts[i][2], u[i], v[i], 1.0, false );
		//add_vertex( outer_verts[i2][0], outer_verts[i2][1], outer_verts[i2][2], u[i2], v[i2], 1.0, false );
		//add_vertex( vert_center[0], vert_center[1], vert_center[2], 0.8, 0.8, 1.0, false );

		sunmoon[i+index].x = verts[i][0];
		sunmoon[i+index].y = verts[i][1];
		sunmoon[i+index].z = verts[i][2];
		sunmoon[i+index].r = (u8)(lum*255);
		sunmoon[i+index].g = (u8)(lum*255);
		sunmoon[i+index].b = (u8)(lum*255);
		sunmoon[i+index].a = 255;
		sunmoon[i+index].u = tu[i];
		sunmoon[i+index].v = tv[i];
		
	}
	


}


float SKYU;

void set_time( float tod ) {
	tod = tod - floor(tod);
	
	SKYU = tod;

	BitmapPixel p = imgSunlight.sample( (int)floor(tod * 512), 0 );
	worldrender::set_sunlight_color( (float)p.r / 255.0f, (float)p.g / 255.0f, (float)p.b / 255.0f );
	worldrender::set_sunlight_intensity( imgSunlight.sample( (int)floor(tod * 512), 8 ).r / 255.0f );

	clouds::set_color( (float)p.r / 255.0f, (float)p.g / 255.0f, (float)p.b / 255.0f );
	clouds::set_light( imgSunlight.sample( (int)floor(tod * 512), 8 ).r / 255.0f );
	//worldrender::set_fog_color( imgFog.data[

	 p = imgSunlight.sample( (int)floor(tod * 512), 20 );
	worldrender::set_fog_color( (float)p.r / 255.0f, (float)p.g / 255.0f, (float)p.b / 255.0f );
	//worldrender::set_sunlight_position( tod+0.25 * 3.14159265*2.0 );

	create_sun_sprite( tod * 3.14159265f*2.0f, 0, 1.0f );

	float moontime  = tod+0.5f;
	moontime -= floor(moontime);
	float moonlum = moontime;
	if( moonlum > 0.5f ) moonlum = moonlum-1.0f;
	moonlum = abs(moonlum)*3.0f;
	
	moonlum = 1.0f - moonlum;
	moonlum = moonlum*moonlum;
	create_sun_sprite( (tod+0.5f) * 3.14159265f*2.0f, 4 , moonlum );

	buffer[BUFFER_SUNMOON].Bind();
	buffer[BUFFER_SUNMOON].BufferData( sunmoon, 8 * sizeof(Video::generic_vertex), GL_STREAM_DRAW_ARB );

}

void render() {

	Video::Shaders::BackdropShader *shader = Video::Shaders::Find( "backdrop" );
	shader->Use();
	shader->SetSkyU( SKYU );
	shader->SetSampler( 0 );

//	Video::SetShader( Video::SHADER_BACKDROP );
	
//	Video::SetBackdropShaderSkyU( SKYU );
//	Video::SetBackdropShaderSampler( 0 );

	cml::vector3f facing = (Video::GetFarPlane()[2]+Video::GetFarPlane()[0])/2.0 - (Video::GetNearPlane()[2]+Video::GetNearPlane()[0])/2.0;
	cml::vector3f facing_h = facing;
	facing[1] = 0;

	float angle = (float)cml::signed_angle( cml::vector3f( 1.0f, 0.0f, 0.0f ), facing_h, cml::vector3f( 0.0f, 1.0f, 0.0f ) );
	float pitch = (float)cml::unsigned_angle( facing_h, facing  );
	if( facing_h[1] > 0 ) pitch=-pitch;
	// todo/note: rotation is removed from the shader
	shader->SetRotation( angle, pitch );
//	Video::SetBackdropShaderRotation( angle, pitch );
	 
	shader->SetShaderPlanes( Video::GetCamera(), Video::GetFarPlane() );
//	Video::SetupBackdropShaderPlanes();
	 
	Video::SetDepthBufferMode( Video::ZBUFFER_DISABLED );
	 
	
	Textures::Bind( Textures::SKY );


	buffer[BUFFER_BACKDROP].Bind();
	
	shader->SetVertexAttributePointers();
//	Video::SetShaderVertexAttributePointers(0);
	Video::DrawQuads( 0, 4 );

	// debug:commented out
	//graphics::render_element( &ebackdrop );
	//graphics::render_element( &esunmoon );
	Video::SetDepthBufferMode( Video::ZBUFFER_ENABLED );

}

void render_sun( bool moon ) {
	Video::SetDepthBufferMode( Video::ZBUFFER_DISABLED );
	
	Video::SetBlendMode( Video::BLEND_ADD );

	Video::Shaders::ObjectShader *shader = Video::Shaders::Find( "objects" );

	shader->SetCameraMatrix( Video::GetXPMatrix()->data() );
	shader->SetSampler( 0 );
	shader->SetSkylight( 1.0f, 1.0f, 1.0f, 0.0 );
	shader->SetTranslation( 0, 0, 0 );

	//Video::SetShader( Video::SHADER_OBJECTS );
	//Video::SetObjectShaderCamera();
	//Video::SetObjectShaderSampler( 0 );
	//Video::SetObjectShaderSunlight( 255,255,255,0);
	//Video::SetObjectShaderTranslation( 0,0,0 );
	
	Textures::Bind( Textures::SUN );
	buffer[BUFFER_SUNMOON].Bind();
	shader->SetVertexAttributePointers();
//	Video::SetShaderVertexAttributePointers(0);
	Video::DrawQuads( 0, 4 );
	
	if( moon ) {
		
	}

	Video::SetDepthBufferMode( Video::ZBUFFER_ENABLED );
}

void render_sunflare() {
	Video::SetDepthBufferMode( Video::ZBUFFER_DISABLED );
	
	Video::SetBlendMode( Video::BLEND_ADD );

	Video::Shaders::ObjectShader *shader = Video::Shaders::Find( "objects" );

	shader->SetCameraMatrix( Video::GetXPMatrix()->data() );
	shader->SetSampler( 0 );
	shader->SetSkylight( 1.0f, 1.0f, 1.0f, 0.0 );
	shader->SetTranslation( 0, 0, 0 );

//	Video::SetShader( Video::SHADER_OBJECTS );
//	Video::SetObjectShaderCamera();
//	Video::SetObjectShaderSampler( 0 );
//	Video::SetObjectShaderSunlight( 255,255,255,0);
//	Video::SetObjectShaderTranslation( 0,0,0 );
	
	Video::SetActiveTextureSlot(1);
	Textures::Bind( Textures::NOISE );
	Video::SetActiveTextureSlot(0);
	Textures::Bind( Textures::SUNFLARE );

	buffer[BUFFER_SUNMOON].Bind();
	shader->SetVertexAttributePointers();
	//Video::SetShaderVertexAttributePointers(0);
	Video::DrawQuads( 0, 4 );

	Textures::Bind( Textures::MOON );

		Video::DrawQuads(4,4);

	Video::SetDepthBufferMode( Video::ZBUFFER_ENABLED );
}


}

#endif
