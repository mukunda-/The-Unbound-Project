//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// fullscreen effect shader

#version 110

uniform sampler2D textureSampler;
uniform sampler2D noiseSampler;
uniform vec3 fragSineParams;      // x = freq, y = depth, z = offset
uniform float screenZoom;
uniform float colorMul;
uniform float distort;
uniform float time;

uniform vec4 colorOverlay;
uniform float horizontalBlur;

varying vec2 fragUV;
varying float fragLum;

void main() {
	
	vec2 coord = fragUV;
	coord.x += sin(fragUV.y*fragSineParams.x+fragSineParams.z)*fragSineParams.y;
	coord -= 0.5;
	
	
	float dist = length(coord)*distort;//*2.0;

	
	
	dist=dist*dist;
	dist=1.0 -dist;
	coord *= dist;

	coord *= screenZoom;
	coord += 0.5;

	// todo: translate shit
//	vec4 col = vec4(0.0,0.0,0.0,0.0);
	//for( float i = -5.0; i <= 5.0; i+= 1.0 ) {
//		col += texture2D( textureSampler, coord + vec2(i*horizontalBlur,0.0)  );
//	}

//	float tt = time/64.0;

//	col /= 11.0;

/*
	float ns = 0.05;//0.2;
	
	vec4 noise = texture2D( noiseSampler, (vec2(coord.x ,coord.y)+tt)*ns ) * 1.0;
	noise += texture2D( noiseSampler, (vec2(coord.x ,coord.y)+tt)*ns*2.0 ) * 0.5;
	
	noise += texture2D( noiseSampler, (vec2(coord.x ,coord.y)+tt)*ns*4.0 ) * 0.25;
	noise += texture2D( noiseSampler, (vec2(coord.x ,coord.y)+tt)*ns*8.0 ) * 0.125;

	noise = noise * (1.0/1.875);	
	noise -= (0.5);
	noise = noise * 64.0/1920.0;//horizontalBlur;
 */
	//col = texture2D( textureSampler, coord + vec2(noise.r,noise.g) );

	vec4 col = vec4(0.0,0.0,0.0,0.0);
	for( float i = -5.0; i <= 5.0; i+= 1.0 ) {
		col += texture2D( textureSampler, coord + vec2(i*horizontalBlur,0.0) );// + vec2(noise.r,noise.g)  );
	}

	col /= 11.0;
	 

	//col.rgb *= fragLum;
	//col.rgb = vec3(1.0,0.0,0.0);
	

	//col.rgb = colorOverlay.rgb * colorOverlay.a + col.rgb * (1.0 - colorOverlay.a);
	col.rgb = mix(col.rgb,colorOverlay.rgb,colorOverlay.a);
	col.rgb *= colorMul;
	col.a = 1.0;
	
	gl_FragColor = col;
}
