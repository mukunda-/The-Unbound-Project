//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#version 110

//#define METHOD_A
#define METHOD_B
//#define METHOD_C

uniform sampler2D sampler1; // perlin map 1
uniform sampler2D sampler2; // perlin map 2
uniform float mu; // interpolation factor
uniform vec3 tint;

uniform vec2 texture_offset;

uniform float density;

uniform vec3 eye;
varying vec3 target;

vec4 over( vec4 a, vec4 b ) {

	vec4 newcolor;
	newcolor.a = a.a + b.a * (1.0 - a.a);
	newcolor.rgb = clamp((a.rgb*a.a + b.rgb*b.a*(1.0-a.a)) / newcolor.a, 0.0, 1.0);
		
	return newcolor;
}

float sample_cloud_texture( vec2 uv ) {
	vec4 sample1 = texture2D( sampler1, uv + texture_offset );
	vec4 sample2 = texture2D( sampler2, uv + texture_offset );

	return mix(sample1.r, sample2.r, mu);
}

float CloudExpCurve(float v) {
	//float c = v*255.0 - (255.0-density*2.0*255.0);//20.0;
	

#ifdef METHOD_A
	float c = v - (1.0-density );//20.0;
	if (c < 0.0) c = 0.0;
	//float CloudDensity = 255.0 - ((pow(0.5, c)) * 255.0);
	float CloudDensity = 1.0 - ((pow(0.0001, c)) * 1.0);
	return CloudDensity / 1.0;
#else
	float c = v*255.0 - (255.0-density*2.0*255.0);//20.0;
	if (c < 0.0) c = 0.0;
	float CloudDensity = 255.0 - ((pow(0.95, c)) * 255.0);
	return CloudDensity / 255.0;
#endif

	
	//return CloudDensity / 255.0;
}

float get_lat( float vy, float y ) {
	
	float vx = cos(asin(vy));
	float b = 2.0 * (vy*y);
	float c = y*y - 1.0;
	float d = b*b - 4.0 * c;
	float t = (-b + sqrt(d)) / (2.0);
	float ty = y + vy * t;
	return asin(ty);
	

	
//	const float PI = 3.14159265;
/*
	float A = asin(vy)+(PI/2.0) + PI/2.0;
	float b = 1.0 / sin(A);
	float C = asin(y/b) + PI/2.0;
	float B = PI - C - A;
	*/

	/*
	float a = asin(vy);
	float b = a - PI/2.0 + asin(y * sin(PI/2.0 + a) / 1.0) - PI/2.0;

	return b;*/
}

void outputter( vec2 uv, float alpha ) {
	vec4 sample1 = texture2D( sampler1, uv + texture_offset );
	vec4 sample2 = texture2D( sampler2, uv + texture_offset );

	float sample = mix(sample1.r, sample2.r,mu);
	float tsample = CloudExpCurve( sample );

	float cloudcol;

	float bump = sample;

	cloudcol = 1.0 - (density*density * 0.5) - bump*(0.0 + density/3.0);

	
	gl_FragColor = vec4(tint*cloudcol, tsample * alpha);
	
}

void outputter2( vec4 col ) {//float tsample, float alpha ) {
	
	//float tsample = CloudExpCurve( sample );
	/*
	float cloudcol;

	float bump = tsample;

	cloudcol = 1.0 - (density*density * 0.5) - bump*(0.0 + density/3.0);

	
	gl_FragColor = vec4(tint*cloudcol, tsample * alpha);
	*/
	gl_FragColor = col;
}
 

#ifdef METHOD_A

vec4 cloudsample( vec3 ray, float azimuth, float height, vec4 color, float heighte )  {
	
	float projection = (height);// / ray.y);
	ray *= projection;
	float radius = distance( ray.xz, vec2(0.0) );
	//if( radius > 1500.0 ) discard;

	float alphai = clamp(1.0 - radius / 1500.0, 0.0, 1.0);
	radius *= 0.001;
	
	vec2 uv;

	uv.x = 0.5 + cos(azimuth) * radius *1.0;
	uv.y = 0.5 + sin(azimuth) * radius *1.0;
	
	vec4 sample1 = texture2D( sampler1, uv + texture_offset+height*4.0 );
	vec4 sample2 = texture2D( sampler2, uv + texture_offset );
	//float sample = sample1.r;///mix(sample1.r, sample2.r,mu);
	float sample = mix(sample1.r, sample2.r,mu);
	//color.rgb -= (1.0-sample)*heighte*4.0;// ((1.0-sample) * 0.5) + (1.0-density)*0.65;// - density*density * 0.55;
	return vec4(color.rgb,CloudExpCurve(sample*color.a) * alphai);
}



void main() {
	
	vec3 ray = (target-eye);
	if( ray.y <= 0.01 ) discard;
	ray = ray / ray.y;


	// polar coordinates
	float azimuth;
	//float radius;
	azimuth = atan(ray.z,ray.x);
	
	vec4 dest = vec4(0.0,0.0,0.0,0.0);

	//float density_adder = 0.0;

	//for( float layer = 0.0; layer < 30.0; layer += 1.0 ) {
	//	vec4 src = 
	//}
	
	float i = 1.0;
	for(i = 0.8; i <= 1.0; i+= 0.01 ) {
		vec4 src = cloudsample( ray, azimuth, 240.0 - i * 100.0, vec4( vec3(1.0 - (i-0.8) * 1.5 ), (i) ), (i-0.8) );
		//density_adder += src.a;
		
		src = clamp(src,0.0,1.0);

		vec4 newcolor;
		newcolor.a = src.a + dest.a * (1.0 - src.a);
		newcolor.rgb = clamp((src.rgb*src.a + dest.rgb*dest.a*(1.0-src.a)) / newcolor.a, 0.0, 1.0);
		
		


		dest = newcolor;
		
	}
	/*
	for( i = 1.01; i < 1.1; i += 0.01 ) {
		vec4 src = cloudsample( ray, azimuth, 240.0 - i * 100.0, vec4( vec3(1.0 - (i-0.8) * 1.5), 1.0-(i-1.00)*4.0 ) );
		//density_adder += src.a;
		
		src = clamp(src,0.0,1.0);

		vec4 newcolor;
		newcolor.a = src.a + dest.a * (1.0 - src.a);
		newcolor.rgb = clamp((src.rgb*src.a + dest.rgb*dest.a*(1.0-src.a)) / newcolor.a, 0.0, 1.0);
		
		


		dest = newcolor;
		
	}*/
	
	//density_adder *= 0.1;

	//dest.rgb = vec3( clamp(2.0- density_adder, 0.0, 1.0) );
	//dest.a = min(density_adder,1.0);
	
	outputter2( dest );

	/*
	float projection = (200.0 / ray.y);
	ray *= projection;
	radius = distance( ray.xz, vec2(0.0) );//
	if( radius > 1500.0 ) discard;

	float alpha = 1.0 - radius / 1500.0;
	radius *= 0.001;

	vec2 uv;

	uv.x = 0.5 + cos(azimuth) * radius *1.0;
	uv.y = 0.5 + sin(azimuth) * radius *1.0;

	outputter( uv, alpha );
	*/
}

#endif

#ifdef METHOD_B
void main() {
	
	float lat,lon;

	vec3 ray = normalize(target-eye);
	//if( ray.y < 0.0 ) return;
	lat = get_lat(ray.y,0.9);
	//lat = asin(ray.y);

	//if( lat > [HALFWAY] ) discard;

	//vec3 ray_h = normalize(vec3(ray.x,0.0,ray.z));
	 
	lon = atan(ray.z, ray.x);
	

	const float PI = 3.14159265;
	
	vec2 uv;
	lat += PI/2.0;
	lat = lat * (1.0 / PI);
	 
	
	lat = 1.0-lat;

	// lat: 0.0=top, 1.0 = bottom

	if( lat > 0.11  ) discard;
	 
	lat = lat * (1.0/0.11  );

	// lat: 1.0 = middle, 0.0 = top

	float alpha = 1.0-lat;

	uv.x = 0.5 + cos(lon) * lat *1.0;
	uv.y = 0.5 + sin(lon) * lat *1.0;
	
	outputter( uv, alpha );

 
}

#endif

#ifdef METHOD_C

vec4 sample_sphere( vec3 ray, float lon, float height, float offset ) {
	float lat = get_lat(ray.y,height);

	const float PI = 3.14159265;
	
	vec2 uv;
	lat += PI/2.0;
	lat = lat * (1.0 / PI);
	 
	lat = 1.0-lat;

	// lat: 0.0=top, 1.0 = bottom

	//if( lat > 0.11  ) discard;
	 
	lat = lat * (1.0/0.11  );

	// lat: 1.0 = middle, 0.0 = top

	float alpha = 1.0-lat;

	uv.x = 0.5 + cos(lon) * lat *1.0;
	uv.y = 0.5 + sin(lon) * lat *1.0;
	uv += offset;
	
	float sample = sample_cloud_texture( uv );
	float tsample = CloudExpCurve( sample );

	float cloudcol;
	float bump = sample;
	cloudcol = 1.0 - (density*density * 0.5) - bump*(0.0 + density/3.0);

	vec4 color = vec4(tint*cloudcol, tsample * alpha);
	return color;
}

void main() {
	vec3 ray = normalize(target-eye);
	float lat,lon;

	
	lon = atan(ray.z, ray.x);
	
	vec4 color = sample_sphere( ray, lon, 0.8, 0.0 );
	
	for( float layer = 0.0; layer < 10.0; layer+= 1.0 ) {
		color = over( sample_sphere( ray, lon, 0.9 + layer * 0.0005, 0.0 ), color ) ;
	}
	//clamp(( sample_sphere( ray, lon, 0.6, 0.4 ) ), 0.0, 1.0 );
	
	//color = clamp(over( sample_sphere( ray, lon, 0.8, 0.2 ), color ), 0.0, 1.0);
	//color = clamp(over( sample_sphere( ray, lon, 0.9, 0.1 ), color ), 0.0, 1.0);
	gl_FragColor = color;
 
}

#endif
