//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// HUD overlay shader (vertex)


#version 110

uniform vec2 translate;

attribute vec2 texcoord;
attribute vec4 color;
attribute vec2 position;

varying vec2 fragUV;
varying vec4 fragColor;

void main() {
	fragUV = texcoord;
	fragUV.x = fragUV.x;
	fragColor = color;

	vec2 coord = position*2.0;
	coord -= 1.0;
	coord.y = -coord.y;

	coord += translate;

	gl_Position = vec4( coord , 0.1, 1.0 );

}
