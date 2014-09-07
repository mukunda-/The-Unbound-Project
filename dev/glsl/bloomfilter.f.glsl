//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// simple downsampler

#version 110

uniform sampler2D sampler1;
uniform float pixelDistanceX;
uniform float pixelDistanceY;
uniform float kernel[7];

varying vec2 fragUV;

void main() {

	//float kernel[] = {0.006,0.061,0.242,0.383,0.242,0.061,0.006};

	vec4 col = texture2D( sampler1, fragUV + vec2( -3.0 * pixelDistanceX, -3.0 * pixelDistanceY ) ) * kernel[0];	
	 col += texture2D( sampler1, fragUV + vec2( -2.0 * pixelDistanceX, -2.0 * pixelDistanceY ) ) * kernel[1];	
	 col += texture2D( sampler1, fragUV + vec2( -1.0 * pixelDistanceX, -1.0 * pixelDistanceY ) ) * kernel[2];
	 col += texture2D( sampler1, fragUV + vec2( 0.0 * pixelDistanceX, 0.0 * pixelDistanceY ) ) * kernel[3];	
	 col += texture2D( sampler1, fragUV + vec2( 1.0 * pixelDistanceX, 1.0 * pixelDistanceY ) ) * kernel[4];	
	 col += texture2D( sampler1, fragUV + vec2( 2.0 * pixelDistanceX, 2.0 * pixelDistanceY ) ) * kernel[5];	
	 col += texture2D( sampler1, fragUV + vec2( 3.0 * pixelDistanceX, 3.0 * pixelDistanceY ) ) * kernel[6];	
	gl_FragColor = col;
}
