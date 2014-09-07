//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

//-------------------------------------------------------------------------------------------------
#ifndef VIDEO_SURFACE_H
#define VIDEO_SURFACE_H

//-------------------------------------------------------------------------------------------------
#include <GL/GL.h>
#include <gl/GLU.h>

//-------------------------------------------------------------------------------------------------
namespace Video {
	
//-------------------------------------------------------------------------------------------------
class Surface {
	
private:
	bool has_depth;
	bool filtered;
	bool created;
	int ratio;
	int width, height;
	GLuint frame_buffer_object;
	GLuint frame_buffer_texture;
	GLuint rendering_buffer;

	bool CreateI( bool p_filtered, bool p_depth, int p_width, int p_height );

public:

	Surface();
	
	//--------------------------------------------------------
	// create using a ratio of the screen width and height
	//
	// p_filtered = use filtering when rendering from this surface
	// p_depth = create a depth buffer for this surface
	// p_ratio = divisor for screen width and height to compute the surface size
	//
	bool Create( bool p_filtered, bool p_depth, int p_ratio );

	//--------------------------------------------------------
	// create using width and height
	//
	bool Create( bool p_filtered, bool p_depth, int p_width, int p_height );

	//--------------------------------------------------------
	// destroy surface
	//
	void Destroy();

	//--------------------------------------------------------
	// set filtering (bilinear) on surface
	//
	void SetFiltering( bool p_filtered );

	//--------------------------------------------------------
	// activate this surface for rendering
	//
	void Use();

	//--------------------------------------------------------
	// bind this surface to the active texture unit
	//
	void BindTexture();

	//--------------------------------------------------------
	// add depth buffering to this surface
	//
	void AddDepthBuffer();

	//--------------------------------------------------------
	// use another surface's depth buffer
	//
	void LinkDepthBuffer( Surface* );

	//--------------------------------------------------------
	// get this surface's depth buffer handle
	// returns invalid handle if using linked depth buffer
	//
	GLuint GetDepthBuffer();

};

//-------------------------------------------------------------------------------------------------
}
#endif
