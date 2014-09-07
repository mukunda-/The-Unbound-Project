//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// fullscreen effect shader

#version 110

uniform sampler2D sampler1;
uniform sampler2D sampler2;

uniform float alpha;

varying vec2 fragUV;

void main() {

	vec4 col1 = texture2D( sampler1, fragUV );	// dest
	vec4 col2 = texture2D( sampler2, fragUV );	// source

	
	
	col1 = mix(col1, col2, col2.a * alpha);
	
	gl_FragColor = col1;
}
