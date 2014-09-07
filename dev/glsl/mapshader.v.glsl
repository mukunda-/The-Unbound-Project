//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#version 110

uniform vec3 translate;
uniform mat4 camera; // VIEW*PROJECT MATRIX
uniform mat4 transform; // transformations
// final = camera * (transform * vertex) 

uniform vec3 sunlight_tint;
uniform float sunlight_intensity;
//uniform vec3 sunlight_vector;

uniform float fogDistance;
uniform float fogLength;

attribute vec2 texcoord;
attribute vec4 colour;
attribute vec3 position;
attribute vec2 normal;

varying vec2 fragUV;
varying vec3 fragColour;
//varying vec4 lightColour;

//varying float realz;

varying float fogFactor;

void main() {
	fragUV = texcoord;
	
	vec3 col = colour.rgb;
	col = clamp( col, 0.0, 1.5 ) + sunlight_tint * colour.a * sunlight_intensity;
	fragColour = col;
	//lightColour = colour;
	
	
	vec4 position = vec4( position+translate, 1.0 );
	
	gl_Position = camera * coord;

	//realz = length(gl_Position);
	//gl_FogFragCoord = gl_FogCoord;

	float z =  length(gl_Position) / fogDistance;
	z = clamp( (z - fogLength)*1.0, 0.0, 1.0 );

	z=1.0 -z;
	
	fogFactor =  clamp(z, 0.0, 1.0);
}
