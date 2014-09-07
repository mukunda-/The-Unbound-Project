//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// terrain shader 2.0

#version 150

#define maxlights 32

//uniform sampler2DArray sampler_texture;
//uniform sampler2DArray sampler_texture_hd;

uniform vec3 light_positions[maxlights];
uniform vec3 light_colors[maxlights];
uniform float light_brightness[maxlights];

#define pattern_entries 8192.0

uniform sampler2D sampler_patterns;
uniform sampler2D sampler_noise;

uniform vec3 global_translation;
uniform mat4 camera;

uniform vec3 skylight_color;
uniform float skylight_intensity;

uniform float fog_distance;
uniform float fog_length;

in float instance_translation;
in float instance_form;
in float instance_texture;

in mat4 instance_color;

//flat out mat3 fragment_layers;
//out vec2 fragment_xy;

out vec3 fragment_uvw;
out vec3 fragment_color;
out float fragment_fog;
out vec3 test_color;//DEBUG
//out sampler2DArray sampler;

//----------------------------------------------------------------------------------------------
void main() {
//----------------------------------------------------------------------------------------------
	// compute pattern texture indexes
	float pattern_x = gl_VertexID / 16.0;// + (0.5/16.0); // x / 4.0 / 4.0
	float pattern_y_texture = instance_texture / pattern_entries;// + (0.5/pattern_entries);
	float pattern_y_form = instance_form / pattern_entries;// + (0.5/pattern_entries);

	//----------------------------------------------
	// compute fragment coordinates
	vec3 coord = global_translation;

	// unpack local translation
	vec3 local_translation;
	local_translation.x = mod( instance_translation, 32.0 );
	local_translation.y = floor( instance_translation / 1024.0 );
	local_translation.z = mod( floor(instance_translation / 32.0), 32.0 );

	// add local translation
	coord += local_translation;
	
	vec2 metatexture_coordinates;

	//vec3 mtx, mty;

	vec2 metatexture_selection = texture(sampler_patterns, vec2( (12.0/16.0), pattern_y_form ) ).st;
	if( metatexture_selection.s == 0 ) {
		//mtx = vec3(1.0,0.0,0.0);
		metatexture_coordinates.x = coord.x;
	} else if( metatexture_selection.s == 1 ) {
		//mtx = vec3(0.0,1.0,0.0);
		metatexture_coordinates.x = coord.y;
	} else if( metatexture_selection.s == 2 ) {
		//mtx = vec3(0.0,0.0,1.0);
		metatexture_coordinates.x = coord.z;
	}

	if( metatexture_selection.t == 0 ) {
		//mty = vec3(1.0,0.0,0.0);
		metatexture_coordinates.y = coord.x;
	} else if( metatexture_selection.t == 1 ) {
		//mty = vec3(0.0,1.0,1.0);
		metatexture_coordinates.y = coord.y;
	} else if( metatexture_selection.t == 2 ) {
		//mty = vec3(0.0,0.0,1.0);
		metatexture_coordinates.y = coord.z;
	}

	//metatexture_coordinates = vec2( dot(mtx,coord.x), dot(mty,coord.y) );

	vec2 metatexture_dimensions = texture(sampler_patterns, vec2( (13.0/16.0), pattern_y_texture ) ).st;

	
	fragment_uvw = texture( sampler_patterns, vec2( pattern_x, pattern_y_texture ) ).stp; // x+0,y
	//fragment_uvw.p += mod(noise.x, metatexture_dimensions.x) + mod(noise.y, metatexture_dimensions.y) * metatexture_dimensions.x;
	fragment_uvw.p += mod(metatexture_coordinates.x, metatexture_dimensions.x) + mod(metatexture_coordinates.y, metatexture_dimensions.y) * metatexture_dimensions.x;

	vec3 form_translation = texture( sampler_patterns, vec2( pattern_x +0.25, pattern_y_form ) ).xyz;

	// add form translation
	coord += form_translation; // x+4,y
	
	//fragment_xy = mod(coord,1.0);

	vec2 noise = texture( sampler_noise, vec2( coord.x + coord.y * 15.0, coord.z + coord.y * 5.0 ) / 1800.0 + 0.5 ).st - 0.5;
	noise += (texture( sampler_noise, vec2( coord.x + coord.y * 15.0, coord.z + coord.y * 5.0 ) / 900.0 + 0.5 ).st - 0.5) * 0.5;
	noise += (texture( sampler_noise, vec2( coord.x + coord.y * 15.0, coord.z + coord.y * 5.0 ) / 450.0 + 0.5 ).st - 0.5) * 0.25;

	// transform with camera
	gl_Position = camera * vec4(coord,1.0);

	float z = length( gl_Position ) / fog_distance;
	z = clamp( (z - fog_length), 0.0, 1.0 );
	z = 1.0-z;
	
	fragment_fog = clamp( z, 0.0, 1.0 );

	//----------------------------------------------
	// read uvw from pattern

	//----------------------------------------------
	// read color from instance data
	// compute skylight addition
	// final color = ic.dim + sc * ic.sky * si
	//mat4 instance_color = mat4( instance_color_1, instance_color_2, instance_color_3, instance_color_4 );
	
	vec4 color;
	color = instance_color[gl_VertexID];
	/*switch( gl_VertexID ) {
	case 0:
		color= instance_color[0];
		break;
	case 1:
		color= instance_color[1];
		break;
	case 2:
		color= instance_color[2];
		break;
	case 3:
		color= instance_color[3];
		break;
		
	} */

	vec3 light_color = vec3(0.0f);
	vec3 normal = texture( sampler_patterns, vec2( pattern_x +0.50, pattern_y_form ) ).xyz; // x+8,y
	

	for( int i = 0; i < 4; i++ ) {
		if( light_brightness[i] > 0.01 ) {
			vec3 dv = light_positions[i] - (coord); // this is an angle
			vec3 dir = normalize(dv);
			
			float angle = max(dot(normal, dir), 0.5);
			//angle = max(angle,0.75);
			
			
			//float i2 = cos(angle);

			float dist = dot(dv,dv);
			dist = sqrt(dist);
			vec3 a = light_colors[i];
			float intensity = 1.0 - (dist / light_brightness[i]);
			
			
			
			if( intensity > 0.0 ) {
				light_color += light_colors[i] * (intensity*intensity) * angle;
			}
		}
	}
	
	fragment_color = color.rgb + light_color + skylight_color * color.a * 4.25 * skylight_intensity; // 4.25 = 63/255
	fragment_color = min(fragment_color, 1.0);

	//fragment_color = 1.0 - exp(-fragment_color); //exposure function
	 
}
