//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// test sh
 
#version 330

in vec3 position; 

uniform mat4 camera;
  

void main() {
	gl_Position = camera * vec4(position,1.0);
}
