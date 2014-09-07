//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

#if 0

namespace ScreenEffects {

float sine_offset;
float sine_frequency;
float sine_depth;
float screen_zoom;

float color_overlay_r;
float color_overlay_g;
float color_overlay_b;
float color_overlay_a;

float screen_distort;

float horiz_blur;

float color_multiply = 1.0;
float screen_flash;

Video::VertexBuffer vb_screen;
Video::point_vertex_2d screen[4];

//ss_vertex screen[4];
//GLuint vboID ;

void init() {

	vb_screen.Create( Video::VF_POINT2D );
	
	//glGenBuffersARB( 1, &vboID );

	screen[0].x = -1;
	screen[0].y = 1;
	screen[1].x = -1;
	screen[1].y = -1;
	screen[2].x = 1;
	screen[2].y = -1;
	screen[3].x = 1;
	screen[3].y = 1;

	vb_screen.BufferData( screen, sizeof( Video::point_vertex_2d ) * 4, GL_STATIC_DRAW_ARB );
}

void setSineEffect( float offset, float frequency, float depth ) {
	sine_offset = offset;
	sine_frequency = frequency;
	sine_depth = depth;
}

void setZoomEffect( float zoom ) {
	screen_zoom = zoom;
}

void setColorOverlay( float r, float g, float b, float a ) {
	color_overlay_r = r;
	color_overlay_g = g;
	color_overlay_b = b;
	color_overlay_a = a;
}

void setHorizontalBlur( float s ) {
	horiz_blur = s;
}

void setDistort( float d ) {
	screen_distort = d;
}

void setPreset_Underwater( float time ) {
	setColorOverlay( 0.0546875f, 0.25f, 0.609375f, 0.3f );
	setHorizontalBlur( (float)Video::ScreenWidth() / 960000.0f / 8 );
	//setHorizontalBlur( 0.05 );
	setZoomEffect( 0.75 );
	setSineEffect( time, 10.0f, 0.01f );
	setDistort( 1.0 );
	
	Video::SetViewDistance( 256.0 );
	Video::SetFogLength( 0.3f );
	Video::SetBackgroundColor( 197/256.0f, 197/256.0f, 197/256.0f);
	//Video::set_bg_color( 0.0546875, 0.25, 0.609375 );
}
float foo=0;
void setPreset_Normal() {
	setColorOverlay( 0.0,0.0,0.0,0.0 );
	setHorizontalBlur( 0);//0.08 );
	setZoomEffect( 1.0 );
	setSineEffect( 0.0,0.0,0.0 );
	setDistort(0.0);
	
	Video::SetViewDistance( 256.0 );
	Video::SetFogLength( 0.2f );
	Video::SetBackgroundColor( 199,199,199);//197/256.0f, 197/256.0f, 197/256.0f);
	//Video::set_bg_color( 0/256.0f, 0/256.0f, 8/256.0f);
	//planet::set_sunlight( 0xf );

	
	foo += 0.1f;
	
	//worldrender::set_sunlight_color(1.0,1.0,1.0 );
	//worldrender::set_sunlight_color( 40/256.0f, 40/256.0f, 120/256.0f);

}

void SetScreenShaderColorMultiply( float a ) {
	color_multiply = a;
}

void SetScreenFlash( float intensity ) {
	screen_flash += intensity;
	if( screen_flash > 32.0 ) screen_flash = 32.0;
}

void UpdateScreenFlash() {
	screen_flash *= pow( 0.65f, (float)gametime::frames_passed());

}

void render() {

	// todo: how to type cast this shit
	Video::Shaders::ScreenShader *shader = Video::Shaders::Find( "screenfx" );
	assert( shader != 0 );

	shader->Use();
	shader->SetSamplers( 0, 1 );
	shader->SetSineEffect( sine_frequency, sine_depth, sine_offset );
	
	float sf_white = screen_flash / 32.0f;
	shader->SetColorOverlay( color_overlay_r + sf_white, color_overlay_g + sf_white, color_overlay_b + sf_white, color_overlay_a );
	shader->SetHorizontalBlur( horiz_blur );
	shader->SetZoom( screen_zoom );
	shader->SetDistortion( screen_distort );
	shader->SetTime( (float)gametime::getSecondsMod( 128.0 ) );
	shader->SetColorMultiply( color_multiply * (1.0f+screen_flash) );
	//Video::SetShader( Video::SHADER_SCREENFX );

	//Video::SetScreenShaderSamplers( 0, 1 );
	//Video::SetScreenShaderSineEffect( sine_frequency, sine_depth, sine_offset );
	//Video::SetScreenShaderColorOverlay( color_overlay_r + sf_white, color_overlay_g + sf_white, color_overlay_b + sf_white, color_overlay_a );
	//Video::SetScreenShaderHorizontalBlur( horiz_blur );
	//Video::SetScreenShaderScreenZoom( screen_zoom );
	//Video::SetScreenShaderDistortion( screen_distort );
	//Video::SetScreenShaderTime( (float)gametime::getSecondsMod(128.0 ) );
	//Video::SetScreenShaderColorMultiply( color_multiply * (1.0f+screen_flash) );

	//Video::EnableShaderVertexAttributes();
	 
	//if( Shaders::SS.uSampler >= 0 ) glUniform1i( Shaders::SS.uSampler, 0 );
	//if( Shaders::SS.uNoiseSampler >= 0 ) glUniform1i( Shaders::SS.uNoiseSampler, 1 );
	//if( Shaders::SS.uFragSineParams >= 0 ) glUniform3f( Shaders::SS.uFragSineParams, sine_frequency, sine_depth, sine_offset );
	//if( Shaders::SS.uColorOverlay >= 0 ) glUniform4f( Shaders::SS.uColorOverlay, color_overlay_r, color_overlay_g, color_overlay_b, color_overlay_a );
	//if( Shaders::SS.uHorizontalBlur >= 0 ) glUniform1f( Shaders::SS.uHorizontalBlur, horiz_blur );
	//if( Shaders::SS.uScreenZoom >= 0 ) glUniform1f( Shaders::SS.uScreenZoom, screen_zoom );
	//if( Shaders::SS.uDistort >= 0 ) glUniform1f( Shaders::SS.uDistort, screen_distort );
	//glUniform1f( Shaders::SS.uTime, gametime::getSecondsMod(128.0) );

	//if( Shaders::SS.aPosition >= 0 ) glEnableVertexAttribArray( Shaders::SS.aPosition );
	//glEnableVertexAttribArray( Shaders::SS.aLum );
	
	Video::SetDepthBufferMode( Video::ZBUFFER_DISABLED );
	//glDisable( GL_DEPTH_TEST );

	Video::SetActiveTextureSlot(1);
	Textures::Bind(Textures::NOISE);
	Video::SetActiveTextureSlot(0);
	

	vb_screen.Bind();
	shader->SetVertexAttributePointers(0);
//	Video::SetShaderVertexAttributePointers(0);
	Video::DrawQuads( 0, 4 );
	
//	Video::DisableShaderVertexAttributes();

	//if( Shaders::SS.aPosition >= 0 ) glDisableVertexAttribArray( Shaders::SS.aPosition );
		
	// release vbo
	//glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	//glEnable( GL_DEPTH_TEST );

	Video::SetDepthBufferMode( Video::ZBUFFER_ENABLED );
}

};

#endif
