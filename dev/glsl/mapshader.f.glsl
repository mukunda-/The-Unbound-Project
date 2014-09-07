//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#version 130

uniform sampler2D textureSampler;
uniform sampler2D noiseSampler;
//uniform float fragOpacity;

uniform vec3 sunlight_tint;
uniform float sunlight_intensity;

uniform vec2 texture_translation;

uniform vec3 fogColor;

varying vec2 fragUV;
varying vec3 fragColour; 

//varying float realz;
//varying vec4 lightColour; 

varying float fogFactor;

void main() {
	//vec2 uv = fragUV * 8.0;
	
	//vec4 n = texture2D( noiseSampler, fragUV*16.0 );
	//n += texture2D( noiseSampler, fragUV*32.0 )*0.5;
	
	//n *= 1.0 / 1.5;
	//n *= 1.0/2048.0;
	//n *= 2.0;

	
	vec4 col = texture2D( textureSampler, fragUV + texture_translation );//+ n.rg);//(uvf+uv+0.5)/512.0   );
	 
	if( col.a < 0.5 ) discard;
	
	col.rgb *= fragColour.rgb;
	

	gl_FragColor = vec4(mix( fogColor, col.rgb, fogFactor ), 1.0);
	
}
