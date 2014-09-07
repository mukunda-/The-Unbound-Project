//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// test fragment shader
//
// for texture_slot_test
//
//

#version 330
 
uniform sampler2D sampler;

in vec2 frag_uv;
out vec4 fragment_color;
  
void main() {
	
	vec4 col = texture2D( sampler, frag_uv );
	col.a = 1.0;

	fragment_color = vec4( col ); 
}
