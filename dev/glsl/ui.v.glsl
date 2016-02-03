//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

// UI element shader

#version 330

//uniform vec2 translate;

attribute vec2 texcoord;
attribute vec4 color;
attribute vec2 position;

out vec2 frag_uv;
out vec4 frag_color;

void main() {
	frag_uv = texcoord; 
	frag_color = color;

	// Map range [0-1] to entire screen [-1,1], 
	// and inverse Y so that the top of the screen is 0.
	vec2 coord = position*2.0;
	coord -= 1.0;
	coord.y = -coord.y; 

	gl_Position = vec4( coord , 0.1, 1.0 );
}
