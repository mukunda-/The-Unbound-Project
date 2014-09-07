//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// HUD overlay shader (fragments)

#version 110

uniform sampler2D textureSampler;

varying vec2 fragUV;
varying vec4 fragColor;

void main() {
	vec4 col = texture2D( textureSampler, fragUV );

	//if( col.rgb == vec3(0.0,1.0,0.0) ) discard;
	
	col *= fragColor;

	
	gl_FragColor = col;
}
