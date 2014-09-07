//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// bloom result calculator


#version 110

uniform sampler2D sampler1; // original image
uniform sampler2D sampler2; // bloom scale 1x
uniform sampler2D sampler3; // bloom scale 2x
uniform sampler2D sampler4; // bloom scale 4x
uniform sampler2D sampler5; // bloom scale 8x
uniform float scale;


varying vec2 fragUV;

void main() { 
	vec4 col = texture2D( sampler1, fragUV ) * 1.0;	
	 col.rgb += texture2D( sampler2, fragUV ).rgb *scale* 1.0 / 8.0;	 
	 col.rgb += texture2D( sampler3, fragUV ).rgb *scale* 2.0 / 8.0;	 
	 col.rgb += texture2D( sampler4, fragUV ).rgb *scale* 4.0 / 8.0;	
	 col.rgb += texture2D( sampler5, fragUV ).rgb *scale* 8.0 / 8.0;	
	 

	 //col.rgb *= 1.0 - pow( vec3(2.71828183,2.71828183,2.71828183), -col.rgb / 1.0 );
	// col.rgb = 1.0 - pow(vec3(255.0,255.0,255.0),-col.rgb/2.0 );
	 col.a = 1.0;
	gl_FragColor = col;
}
