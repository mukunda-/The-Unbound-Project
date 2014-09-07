//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// fullscreen effect shader

#version 110

uniform sampler2D texture_sampler;
uniform sampler2D noise_sampler;

uniform float yrot;
uniform float xrot;

uniform float skyu;

varying vec2 fragUV;

varying vec3 near;
varying vec3 far;

void main() {
	
	vec3 ray = normalize(far-near);

	vec3 ray_h = normalize(vec3(ray.x,0.0,ray.z));
	
	float lon = atan(ray_h.z, ray_h.x);

	float lat = asin(ray.y);
	//lat = lat + texture2D( noise_sampler,vec2( lat*9999.0, lon*9999.0) ).s * 0.2;
	
	const float PI = 3.14159265;
	lat += PI/2.0;

	
	//gl_FragColor = vec4((lon+PI)/(PI*2.0),lat/PI,0.0,0.0);//texture2D( texture_sampler, vec2( ((lon + PI) / PI*2.0), 0.0 ) );

	float x = (lon+PI)/(PI*2.0);
	float y = lat/PI;

	gl_FragColor = texture2D( texture_sampler, vec2( skyu, 0.99  -y*0.98 ) );

	//gl_FragColor = vec4(0.0,y,0.0,0.0);
	// gl_FragColor = texture2D( texture_sampler, vec2( ((lon + PI) / PI*2.0),0.99 -y*0.98));//0.0 ) );

	//vec4 col = vec4(0.0,0.0,0.0,0.0);
	//for( float v = -5.0; v <= 5.0; v += 1.0 )
	//	col += texture2D( texture_sampler, vec2(x+v*0.0005, y ) );
	//for( float v = -5.0; v <= 5.0; v += 1.0 )
//		col += texture2D( texture_sampler, vec2(x, y+v*0.0005 ) );
	//gl_FragColor = col / 10.0;

	//vec4 col = texture2D( texture_sampler, vec2(x, y ) );
	//
	gl_FragColor.a = 1.0;

	//const float PI2R = 0.15915494309189533576888376337251;
	
	//vec2 coord = fragUV;
	//coord.x = coord.x - 0.5;
	//coord.x *= coord.y;
	//coord.x += 0.5;
	
	//gl_FragColor = texture2D( texture_sampler, coord / 6.283185307179586476925286766559 + vec2(-yrot * PI2R,-xrot * PI2R/2.0) );
	//gl_FragColor = texture2D( texture_sampler, coord * 0.19098593171027440292266051604702 + vec2(-yrot * PI2R,xrot * PI2R) );
	//gl_FragColor.a = 1.0;
}
