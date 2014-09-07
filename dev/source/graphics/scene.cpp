//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

#if 0 ///bypass

	extern Models::Model poody; //////DEBUG

namespace Scene {

//Video::Surface sfBackdrop;
//Video::Surface sfWorldObjects;
//Video::Surface sfWater;

Video::Surface sfLayer1; // backdrop,terrain,etc
Video::Surface sfLayer2; // water

Video::Surface sfPostSS;

Video::Surface sfBloomH[4];
Video::Surface sfBloomV[4];

RasterFonts::Font test_font;


#define PBO_COUNT 2
GLuint pboID[2];
int pbo_index_read;
int pbo_index_write;
bool pbo_init;

//int image_intensity ;
double image_intensity;

typedef struct t_ss_vertex {
	
	float x;
	float y;
} ss_vertex;

ss_vertex screen[4];
//GLuint screen_vbo ;
Video::VertexBuffer vb_screen;


void setup_pbo() {
	glGenBuffersARB( PBO_COUNT, pboID );
	pbo_init=true;
	pbo_index_read = 0;
	pbo_index_write = 1;

	int pbo_size = Video::ScreenWidth() * Video::ScreenHeight() * 4;

	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboID[0]);
    glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, pbo_size, 0, GL_STREAM_READ_ARB);
    glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboID[1]);
    glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, pbo_size, 0, GL_STREAM_READ_ARB);
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
	image_intensity = 1.0;
}

void processPixels( GLubyte *pixels ) {
	
	
	int x, y,w,h;

	double result = 0.0;

	u32 *p32 = (u32*)pixels;

	w = Video::ScreenWidth();
	h = Video::ScreenHeight();
	for( y = 0; y < h; y+=16 ) {
		for( x = 0; x < w; x+=16 ) {
			float r = (float)pixels[(x+y*w)*4];
			float g = (float)pixels[(x+y*w)*4+1];
			float b = (float)pixels[(x+y*w)*4+2];
			r /= 255.0f;
			g /= 255.0f;
			b /= 255.0f;
			r *= 0.2989f;
			g *= 0.5870f;
			b *= 0.1140f;
			double i = r+g+b;
			result += i;
		}
	}
	result /= (double)(w * h) / 256.0;
	//if( image_intensity < result ) image_intensity += 0.01;
	//if( image_intensity > result ) image_intensity -= 0.01;
	//if( image_intensity <= 0 ) image_intensity = 0.01;
	image_intensity = image_intensity * 0.990 + result * 0.01;
}

void swap_pbo() {

	// "index" is used to read pixels from framebuffer to a PBO
	// "nextIndex" is used to update pixels in the other PBO
	pbo_index_read = (pbo_index_read + 1) % PBO_COUNT;
	pbo_index_write = (pbo_index_write + 1) % PBO_COUNT;

	// set the target framebuffer to read
	glReadBuffer(GL_FRONT);

	// read pixels from framebuffer to PBO
	// glReadPixels() should return immediately.
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboID[pbo_index_write]);
	glReadPixels(0, 0, Video::ScreenWidth(), Video::ScreenHeight(), GL_BGRA, GL_UNSIGNED_BYTE, 0);

	// map the PBO to process its data by CPU

	if( !pbo_init ) {
		glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pboID[pbo_index_read]);
		GLubyte* ptr = (GLubyte*)glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB,GL_READ_ONLY_ARB);
		if(ptr)
		{
			processPixels(ptr);
			glUnmapBufferARB(GL_PIXEL_PACK_BUFFER_ARB);
		}
	} else {
		pbo_init = false;
	}

	// back to conventional pixel operation
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
}

// 0x10 = horizontal wrap
// 0x01 = vertical wrap
u8 wrapping_modes[] = {
	0x11,0x11,0x11,0x10,  0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,  
	0x01,0x00,0x11,0x00,  0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,  
	0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,  
	0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,  

};

void copy_tile( int dx, int dy, int sx, int sy, u8 *dest, const u8 *source, int samples ) {
	/*
	for( int x = -1; x < 129; x++ ) {
		for( int y = -1; y < 129; y++ ) {
			if( x >= 0 && x <= 
		}
	}*/
}

void downsample_32bit_half( u8 *dest, u8 *source, int newsize ) {
	int oldsize = newsize*2;
	u8 *newdata = new u8[newsize*newsize*4];

	for( int x = 0; x < newsize; x++ ) {
		for( int y = 0; y < newsize; y++ ) {
			int pixel[4];

			for( int i = 0; i < 4; i++ ) {
				pixel[i] = source[(x*2+y*2*oldsize)*4+i];
				pixel[i] += source[(x*2+1+y*2*oldsize)*4+i];
				pixel[i] += source[(x*2+(y*2+1)*oldsize)*4+i];
				pixel[i] += source[(x*2+1+(y*2+1)*oldsize)*4+i];
				pixel[i] /= 4;
				
				newdata[(x+y*newsize)*4 + i] = (u8)pixel[i];
			}

		}
	}

	for( int i = 0; i < newsize*newsize*4; i++ ) {
		dest[i] = newdata[i];
	}

	delete[] newdata;
}

void LoadWorldTexture() {
	
	world_texture::create();


}

void Init() {

	
	LoadWorldTexture() ;
	RasterFonts::Init();

	//test_font.load( "C:\\windows\\fonts\\TrajanPro-Bold.otf", 26 );
	//test_font.LoadFace( "C:\\windows\\fonts\\homizio.ttf", 20,2  );
	//test_font.LoadFace( "C:\\windows\\fonts\\homizio.ttf", 24,0 );

	test_font.LoadFace( "SourceCodePro-Light.ttf", 20,2  );
	test_font.LoadFace( "SourceCodePro-Light.ttf", 24,0 );
	//test_font.DebugDump();
	

	Textures::LoadFontTexture( Textures::FONT1, &test_font );
	hud::use_font( &test_font );

	clouds::init();
	backdrop::init();
	worldrender::init();
	//objects::init();
	graphics::init();
	hud::init();
	screenshader::init();

	particles::init();

	//sfBackdrop.Create( GL_FALSE, GL_FALSE, 1 );
	//sfWorldObjects.Create( GL_TRUE, GL_TRUE, 1 );
	//sfWater.Create( GL_FALSE, GL_FALSE, 1 );

	sfLayer1.Create( GL_TRUE, GL_TRUE, 1 );
	sfLayer2.Create( GL_TRUE, GL_FALSE, 1 );
	sfLayer2.LinkDepthBuffer( &sfLayer1 );

	sfPostSS.Create( GL_TRUE, GL_FALSE, 1 );
	//sfPostSS.LinkDepthBuffer( &sfWorldObjects );

	sfBloomH[0].Create( GL_FALSE, GL_FALSE, 1 );
	sfBloomH[1].Create( GL_TRUE, GL_FALSE, 2 );
	sfBloomH[2].Create( GL_TRUE, GL_FALSE, 4 );
	sfBloomH[3].Create( GL_TRUE, GL_FALSE, 8 );

	sfBloomV[0].Create( GL_FALSE, GL_FALSE, 1 );
	sfBloomV[1].Create( GL_TRUE, GL_FALSE, 2 );
	sfBloomV[2].Create( GL_TRUE, GL_FALSE, 4 );
	sfBloomV[3].Create( GL_TRUE, GL_FALSE, 8 );

	//glGenBuffersARB( 1, &screen_vbo );
	vb_screen.Create( Video::VF_POINT2D );

	screen[0].x = -1;
	screen[0].y = 1;
	screen[1].x = -1;
	screen[1].y = -1;
	screen[2].x = 1;
	screen[2].y = -1;
	screen[3].x = 1;
	screen[3].y = 1;

	vb_screen.BufferData( screen, 4*sizeof(ss_vertex) , GL_STATIC_DRAW_ARB );

	// copy data to buffer
	//glBindBufferARB( GL_ARRAY_BUFFER_ARB, screen_vbo );
	//glBufferDataARB( GL_ARRAY_BUFFER_ARB, 4 * sizeof(ss_vertex), screen, GL_STATIC_DRAW_ARB );

	setup_pbo();
}

void run_sm( float alpha ) {

	Video::Shaders::SurfaceMixer *shader = Video::Shaders::Find( "surfacemixer" );
	assert( !shader );

	shader->Use();
	shader->SetSamplers( 0, 1 );
	shader->SetAlpha( alpha );
	//Video::SetShader( Video::SHADER_SURFACEMIXER );
	//Video::SetSurfaceMixerSamplers( 0, 1 );
	//Video::SetSurfaceMixerAlpha( alpha );

	Video::SetDepthBufferMode( Video::ZBUFFER_DISABLED );

	vb_screen.Bind();
	shader->SetVertexAttributePointers();
//	Video::SetShaderVertexAttributePointers();
	Video::DrawQuads( 0, 4 );
	
	Video::SetDepthBufferMode( Video::ZBUFFER_ENABLED );
	 
}
/*
void run_bloom() {
	
	Video::SetBlendMode( Video::BLEND_OPAQUE );
	Video::SetDepthBufferMode( Video::ZBUFFER_DISABLED );

/////////////////////////// DOWNSAMPLE /////////////////////////
	Video::SetShader( Video::SHADER_BLOOMDOWNSAMPLER );
	//Shaders::BL1.Use();

	Video::SetBloomDownsamplerSampler( 0 );
	//glUniform1i( Shaders::BL1.sampler, 0 );
	
	//glEnableVertexAttribArray( Shaders::BL1.position );

	sfPostSS.BindTexture();

	vb_screen.Bind();
	Video::SetShaderVertexAttributePointers();
	//glBindBufferARB(GL_ARRAY_BUFFER_ARB, screen_vbo );
 	//glVertexAttribPointer( Shaders::BL1.position, 2, GL_FLOAT, GL_FALSE, 8, (void*)0 );
	
	sfBloomH[0].Use();
	Video::DrawQuads(0,4);
	//glDrawArrays(GL_QUADS, 0, 4 );

	sfBloomH[1].Use();
	Video::DrawQuads(0,4);
	//glDrawArrays(GL_QUADS, 0, 4 );

	sfBloomH[2].Use();
	Video::DrawQuads(0,4);
	//glDrawArrays(GL_QUADS, 0, 4 );

	sfBloomH[3].Use();
	Video::DrawQuads(0,4);
	//glDrawArrays(GL_QUADS, 0, 4 );

	//glDisableVertexAttribArray( Shaders::BL1.position );

//////////////////////////// FILTER
	
	Video::SetShader( Video::SHADER_BLOOMFILTER );
	//Shaders::BL2.Use();
	GLfloat kernel[] = {0.006f,0.061f,0.242f,0.383f,0.242f,0.061f,0.006f};
	//GLfloat kernel[] = {0.14285714285714285714285714285714,0.14285714285714285714285714285714,0.14285714285714285714285714285714,0.14285714285714285714285714285714,0.14285714285714285714285714285714,0.14285714285714285714285714285714,0.14285714285714285714285714285714};

	Video::SetBloomFilterKernel( kernel );
	Video::SetBloomFilterSampler( 0 );
	//glUniform1i( Shaders::BL2.sampler, 0 );
	//glUniform1fv( Shaders::BL2.kernel, 7, kernel );
	
	
	//glUniform1f( Shaders::BL2.pixelDistanceY, 0.0 );
		
	//glEnableVertexAttribArray( Shaders::BL2.position );

	vb_screen.Bind();
	Video::SetShaderVertexAttributePointers();
	//glBindBufferARB(GL_ARRAY_BUFFER_ARB, screen_vbo );
 	//glVertexAttribPointer( Shaders::BL2.position, 2, GL_FLOAT, GL_FALSE, 8, (void*)0 );
	
	//glUniform1f( Shaders::BL2.pixelDistanceX, 1.0 / ((float)Video::SCREEN_WIDTH*2.0) );
	Video::SetBloomFilterPixelVector( 1.0f / ((float)Video::SCREEN_WIDTH*2.0f), 0.0f );
	sfBloomH[0].BindTexture();
	sfBloomV[0].Use();
	Video::DrawQuads( 0, 4 );
	//glDrawArrays(GL_QUADS, 0, 4 );

	Video::SetBloomFilterPixelVector( 1.0f / ((float)Video::SCREEN_WIDTH*1.0f), 0.0f );
	sfBloomH[1].BindTexture();
	sfBloomV[1].Use();
	Video::DrawQuads( 0, 4 );
	//glDrawArrays(GL_QUADS, 0, 4 );

	Video::SetBloomFilterPixelVector( 1.0f / ((float)Video::SCREEN_WIDTH/2.0f), 0.0f );
	sfBloomH[2].BindTexture();
	sfBloomV[2].Use();
	Video::DrawQuads( 0, 4 );
	//glDrawArrays(GL_QUADS, 0, 4 );

	Video::SetBloomFilterPixelVector( 1.0f / ((float)Video::SCREEN_WIDTH/4.0f), 0.0f );
	sfBloomH[3].BindTexture();
	sfBloomV[3].Use();
	Video::DrawQuads( 0, 4 );
	//glDrawArrays(GL_QUADS, 0, 4 );


	//glUniform1f( Shaders::BL2.pixelDistanceX, 0.0 );

	//glUniform1f( Shaders::BL2.pixelDistanceY, 1.0 / ((float)Video::SCREEN_HEIGHT*2.0) );
	Video::SetBloomFilterPixelVector( 0.0, 1.0f / ((float)Video::SCREEN_HEIGHT*2.0f) );
	sfBloomH[0].Use();
	sfBloomV[0].BindTexture();
	Video::DrawQuads( 0, 4 );
	//glDrawArrays(GL_QUADS, 0, 4 );

	Video::SetBloomFilterPixelVector( 0.0, 1.0f / ((float)Video::SCREEN_HEIGHT*1.0f) );
	sfBloomH[1].Use();
	sfBloomV[1].BindTexture();
	Video::DrawQuads( 0, 4 );
	//glDrawArrays(GL_QUADS, 0, 4 );

	Video::SetBloomFilterPixelVector( 0.0, 1.0f / ((float)Video::SCREEN_HEIGHT/2.0f) );
	sfBloomH[2].Use();
	sfBloomV[2].BindTexture();
	Video::DrawQuads( 0, 4 );
	//glDrawArrays(GL_QUADS, 0, 4 );

	Video::SetBloomFilterPixelVector( 0.0, 1.0f / ((float)Video::SCREEN_HEIGHT/4.0f) );
	sfBloomH[3].Use();
	sfBloomV[3].BindTexture();
	Video::DrawQuads( 0, 4 );
	//glDrawArrays(GL_QUADS, 0, 4 );
	
	//glDisableVertexAttribArray( Shaders::BL2.position );
	
//////////////////////// RESULT

	Video::SetShader( Video::SHADER_BLOOMRESULT );
	//aders::BL3.Use();

	Video::SetBloomResultSamplers( 0,1,2,3,4 );
	//Uniform1i( Shaders::BL3.sampler1, 0 );
	//Uniform1i( Shaders::BL3.sampler2, 1 );
	//Uniform1i( Shaders::BL3.sampler3, 2 );
	//Uniform1i( Shaders::BL3.sampler4, 3 );
	//Uniform1i( Shaders::BL3.sampler5, 4 );

	float sc1 = (float)(1.0-image_intensity);
	sc1 = (1.0f - sc1*sc1);

	float sc = 0.8f - sc1 * 1.0f;
	
	
	if( sc < 0.05f ) sc = 0.05f;

	Video::SetBloomResultScale( sc );
	//glUniform1f( Shaders::BL3.scale, sc );

	glActiveTexture( GL_TEXTURE1 );
	sfBloomH[0].BindTexture();
	glActiveTexture( GL_TEXTURE2 );
	sfBloomH[1].BindTexture();
	glActiveTexture( GL_TEXTURE3 );
	sfBloomH[2].BindTexture();
	glActiveTexture( GL_TEXTURE4 );
	sfBloomH[3].BindTexture();
	glActiveTexture( GL_TEXTURE0 );
	sfPostSS.BindTexture();
	Video::UseGlobalSurface();


	//glEnableVertexAttribArray( Shaders::BL3.position );

	vb_screen.Bind();
	Video::SetShaderVertexAttributePointers();
	//glBindBufferARB(GL_ARRAY_BUFFER_ARB, screen_vbo );
 //	glVertexAttribPointer( Shaders::BL3.position, 2, GL_FLOAT, GL_FALSE, 8, (void*)0 );
	Video::DrawQuads( 0, 4 );
	//glDrawArrays(GL_QUADS, 0, 4 );

	//glDisableVertexAttribArray( Shaders::BL3.position );

	Video::SetDepthBufferMode( Video::ZBUFFER_ENABLED );
	//glEnable( GL_DEPTH_TEST );
	//glDepthFunc( GL_LEQUAL );

	//glEnable(GL_BLEND);

	
}
*/
float tod=0.0;

bool debug_switch_1=false;

void Render() {
	
	cml::vector3f vpoodycam=Video::GetRelativeCameraPoint(0.5,0.5,0.5); // DEBUG/TEST
 
	//graphics::draw_sprite( vpoodycam, 5,5,0,0,1,1,Video::BLEND_OPAQUE, 255,255,255,255,0,false); // DEBUG/TEST
	graphics::finalize_data();
	
	// def=0.0001f
	tod += 0.0001f * (float)gametime::frames_passed() ;//3.14159*2.0/360.0 * 1.0;
	
	//objects::create_sun_sprite( tod );
	backdrop::set_time(tod/(3.14159f*2.0f));
	
	if( planet::get_cube( (int)(Video::GetCamera()[0]), (int)(Video::GetCamera()[1]), (int)(Video::GetCamera()[2]) ) == planet::CELL_WATER ) {
		
		screenshader::setPreset_Underwater(0.0);
	} else {
		screenshader::setPreset_Normal();
	}
	
	sfLayer1.Use();

	Video::Clear();
	// rendering cycles
	// layer1
	//  - backdrop
	//  - terrain
	//  - solid graphics
	// layer2
	//  - water
	// <mix> layer1 <- layer2 (water alpha)
	// layer1
	//  - blended graphics
	// global
	//  - full screen shader
	
	Video::SetBlendMode( Video::BLEND_OPAQUE );

	backdrop::render();
	backdrop::render_sunflare();
	//clouds::set_density(0.25);
	clouds::set_center( Video::GetCamera()[0], Video::GetCamera()[2] );
	clouds::render();
	backdrop::render_sun(true);
	// debug: press t for wireframe terrain
	//######################DEBUG##############################{
	if( Input::KeyPressed(DIK_T) ) {

		
		//Textures::SetTextureAnisotropy( Textures::WORLD, 16.0f );
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		
		//Textures::SetTextureAnisotropy( Textures::WORLD, 0.0f );
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	//######################DEBUG##############################}

	worldrender::render();
	
	
	//######################DEBUG##############################{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//######################DEBUG##############################}
		
		
	//poody.DrawInstanceTEST(vpoodycam[0],vpoodycam[1],vpoodycam[2]); // DEBUG/TEST
	graphics::render_solid_graphics();
	

	sfLayer2.Use();

	
	Video::SetBlendMode( Video::BLEND_OPAQUE );
	glClear( GL_COLOR_BUFFER_BIT );
	worldrender::render_water();

	sfLayer1.Use();
	glActiveTexture(GL_TEXTURE1);	// source
	sfLayer2.BindTexture();
	glActiveTexture(GL_TEXTURE0);	// dest
	sfLayer1.BindTexture();
	Video::SetBlendMode( Video::BLEND_OPAQUE );

	run_sm(0.7f);

	graphics::render_blended_graphics();

	sfLayer1.BindTexture();
	
	sfPostSS.Use();
	//Video::UseGlobalSurface();
	
	Video::SetBlendMode( Video::BLEND_OPAQUE );
	screenshader::render();

	

	//run_bloom();

	swap_pbo();

	hud::render();


}




}

#endif
