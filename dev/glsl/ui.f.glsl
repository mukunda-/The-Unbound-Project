//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2016, Mukunda Johnson, All rights reserved. =========//

// UI element shader

#version 330

uniform sampler2D sampler;

in vec2 frag_uv;
in vec4 frag_color;

out vec4 fragment_color;

void main() {
	vec4 col = texture2D( sampler, frag_uv ); 
	col *= frag_color; 
	col.a = 1.0;
	
	fragment_color = vec4(1.0,1.0,1.0,1.0);// col;
}
