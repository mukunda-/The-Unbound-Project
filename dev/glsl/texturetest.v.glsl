//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// test sh
 
#version 330

uniform mat4 camera;
uniform vec3 translation;

in vec3 position; 
in vec2 uv;



out vec2 frag_uv;

void main() {
	frag_uv = uv;
	vec3 pos = position + translation;
	gl_Position = camera * vec4(pos,1.0);
}
