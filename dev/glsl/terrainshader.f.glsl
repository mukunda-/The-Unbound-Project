//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// terrain shader 2.0

#version 150

uniform sampler2DArray sampler_texture;
uniform sampler2D sampler_noise;

uniform vec3 texture_translation;
uniform float opacity;

uniform vec3 fog_color;

in vec3 fragment_uvw;
in vec3 fragment_color;
in float fragment_fog;
//int sampler2DArray sampler;

in vec3 test_color;

out vec4 fragment_output;

void main() {
	
	//vec3 uv = vec3( clamp(fragment_uvw.st, vec2(0.03125,0.03125), vec2(0.96875,0.96875)), fragment_uvw.z );  
	vec4 texture_color = texture( sampler_texture , fragment_uvw + texture_translation );
	
	if( texture_color.a < 0.5 ) discard;

	texture_color.rgb *= fragment_color;

	//fragment_output = vec4( 1.0, 0.0, 0.0, 1.0 );
	//fragment_output = vec4(texture_color);
	fragment_output = vec4( mix( fog_color, texture_color.rgb, fragment_fog ), opacity );
}
