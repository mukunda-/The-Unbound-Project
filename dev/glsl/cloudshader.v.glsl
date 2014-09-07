//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// CLOUDSHADER

#version 110

//uniform vec3 translate;
//uniform mat4 camera;

//attribute vec2 texcoord;
//attribute vec4 color;
//attribute vec3 position;

attribute vec2 position;
//attribute vec3 spacecoord;

uniform vec3 eye;
uniform vec3 topleft;
uniform vec3 topright;
uniform vec3 bottomleft;
uniform vec3 bottomright;


//varying vec2 fragUV;
//varying vec4 fragColor;


varying vec3 target;

void main() {
	
	gl_Position = vec4(position,1.0,1.0);
	
	vec3 fartop = mix(topleft,topright,(position.x+1.0)/2.0);
	vec3 farbottom = mix(bottomleft,bottomright,(position.x+1.0)/2.0);

	target = mix( farbottom, fartop, (position.y+1.0)/2.0 );
}
