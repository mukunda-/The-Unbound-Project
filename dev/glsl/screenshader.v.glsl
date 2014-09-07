//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// fullscreen effect shader


#version 110

attribute vec2 position;

varying vec2 fragUV;
varying float fragLum;

void main() {
	
	fragUV = (position + 1.0) / 2.0;
	gl_Position = vec4( position , 0.1, 1.0 );

}
