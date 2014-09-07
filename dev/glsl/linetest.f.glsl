//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// test fragment shader

#version 330

uniform vec3 color;

out vec4 fragment_color;
  
void main() {
	
	fragment_color = vec4( color,1.0 ); 
}
