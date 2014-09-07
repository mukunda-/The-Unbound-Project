//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// simple downsampler

#version 110

uniform sampler2D sampler1;

varying vec2 fragUV;

void main() {

	
	gl_FragColor = texture2D( sampler1, fragUV );// * scale;	// dest
	
}
