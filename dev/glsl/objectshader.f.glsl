//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#version 110

// object shader

uniform sampler2D textureSampler;


varying vec2 fragUV;
varying vec4 fragColor;

uniform float fogDistance;
uniform float fogLength;

void main() {

	vec4 col = texture2D( textureSampler, fragUV );
	col.a = 1.0;
//	col.rg = fragUV;
	//if( col.rgb == vec3(0.0,0.0,0.0) ) discard;
	
//	col.r *= fragColor.r;
//	col.g *= fragColor.g;
	//col.b *= fragColor.b;
	//col.a *= fragColor.a;

	float z =  (gl_FragCoord.z / gl_FragCoord.w) / fogDistance;
	
	z = clamp( (z - fogLength)*5.0, 0.0, 1.0 );
	
	z=z*z;
	z=1.0 -z;
	
	float fogFactor = z;
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	
	gl_FragColor = col;//mix(gl_Fog.color, col, fogFactor );
//	if(fragUV.y >= 0.5 )
//		gl_FragColor.a = 0.5;
//	else
//		gl_FragColor.a = 1.0;
	
	
}
