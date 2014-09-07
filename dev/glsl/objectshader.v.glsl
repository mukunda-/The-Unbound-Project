//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#version 110

uniform vec3 translate;
uniform mat4 camera;
uniform vec3 sunlight_tint;
uniform float sunlight_intensity;

attribute vec2 texcoord;
attribute vec4 colour;
attribute vec3 position;

varying vec2 fragUV;
varying vec4 fragColor;

void main() {
	fragUV = texcoord; 
	vec4 col = colour;
	col = clamp( col, 0.0, 1.5 );// + sunlight_tint * colour.a * sunlight_intensity;
	fragColor = col;
	
	vec4 coord = vec4( position+translate, 1.0 );
	gl_Position = camera * coord;
	
}
