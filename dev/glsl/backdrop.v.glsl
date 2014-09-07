//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// fullscreen effect shader


#version 110

uniform vec3 near_a;
uniform vec3 near_b;
uniform vec3 near_c;
uniform vec3 near_d;

uniform vec3 far_a;
uniform vec3 far_b;
uniform vec3 far_c;
uniform vec3 far_d;


attribute vec2 position;


//varying vec2 fragUV;
//varying float fragLum;

varying vec3 near;
varying vec3 far;

void main() {
	
	//fragUV = (position + 1.0) / 2.0;
	//fragUV.y = -fragUV.y;
	vec2 coord = position;
	coord.y = 1.0-coord.y;
	coord = vec2( -1.0, -1.0 ) + coord * 2.0;
	gl_Position = vec4( coord , 0.1, 1.0 );
	
	
	
	//vec3 neartop = mix( near_a, near_d, position.x );
	//vec3 nearbottom = mix( near_b, near_c, position.x );
	//near = mix( neartop, nearbottom, position.y );
	near = near_a;

	vec3 fartop = mix( far_a, far_d, position.x );
	vec3 farbottom = mix( far_b, far_c, position.x );
	far = mix( fartop, farbottom, position.y );
}
