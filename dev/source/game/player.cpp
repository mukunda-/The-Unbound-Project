//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#include "stdafx.h"

#if 0 // obsolete

namespace Players {
static const double pi = 3.14159;
static const double maxpitch = 0.45;

static const double run_power = 0.08;
static const double walk_power = 0.04;
static const double gravity = 0.01;
static const double speed_dampen = 0.9;
static const double jump_power = 0.2;


void Player::reset( float x, float y, float z ) {
	pos = cml::vector3f(x,y,z);
	//set_vector64( &pos, x, y, z );
	pitch = 0.0;
	sprinting=false;
	moving=0;
}

void Player::look( double pangle, double ppitch, bool relative ) {
	if( relative ) {
		angle += pangle;
		pitch += ppitch;
		if( pitch > pi*maxpitch ) pitch = pi*maxpitch;
		if( pitch < -pi*maxpitch ) pitch =- pi*maxpitch;
	} else {
		angle = pangle;
		pitch = ppitch;
	}
}

void Player::move( int pmoves ) {
	moving = pmoves;
}

void Player::sprint( bool s ) {
	sprinting = s;
}

void Player::apply_movement() {

#define fatness 0.4
#define height 1.8
	float biggest_axis = (float)abs(vel[0]);
	if( (float)abs(vel[1]) > biggest_axis ) biggest_axis = (float)abs(vel[1]);
	if( (float)abs(vel[2]) > biggest_axis ) biggest_axis = (float)abs(vel[2]);

	int samples = (int)floor(biggest_axis)*4;
	if( samples < 1 ) samples = 1;

	cml::vector3d svl;
	
	float total_samples = (float)samples;;

	for( ; samples; samples-- ) {
		svl = vel / total_samples;
		if( vel[0] > 0 ) {
			for( float z = -1; z <= 1 && svl[0] != 0.0; z+= 1.0 ) {
				for( float y = 0.0; y <= 1 && svl[0] != 0.0; y += 0.5 ) { 
					if( planet::collision3d( pos + cml::vector3d( fatness + svl[0], y*height, z*fatness ) ) ) {
						pos[0] = floor(pos[0]+fatness+svl[0]) - fatness - 0.00390625;
						svl[0] = 0.0;
						vel[0] *= 0.9;
					}
				}
			}
		} else if( vel[0] < 0 ) {
			for( float z = -1; z <= 1 && svl[0] != 0.0; z += 1.0 ) {
				for( float y = 0.0; y <=1 && svl[0] != 0.0; y += 0.5 ) { 
					if( planet::collision3d( pos + cml::vector3d( -fatness + svl[0], y*height, z*fatness ) ) ) {
						pos[0] = floor(pos[0]-fatness+svl[0]) + 1 + fatness+0.00390625;
						svl[0] = 0.0;
						vel[0] *= 0.9;
					}
				}
			}
		}
		pos[0] += svl[0];

		if( vel[2] > 0 ) {
			for( float x = -1; x <= 1 && svl[2] != 0.0; x++ ) {
				for( float y =0.0; y <= 1.0 && svl[2] != 0.0; y += 0.5 ) { 
					if( planet::collision3d( pos + cml::vector3d( x*fatness, y*height, fatness + svl[2] ) ) ) {
						pos[2] = floor(pos[2]+fatness+svl[2]) - fatness - 0.00390625;
						svl[2] = 0.0;
						vel[2] *= 0.9;
					}
				}
			}
		} else if( vel[2] < 0 ) {
			for( float x = -1; x <= 1 && svl[2] != 0.0; x++ ) {
				for( float y = 0.0; y <=1 && svl[2] != 0.0; y += 0.5 ) { 
					if( planet::collision3d( pos + cml::vector3d( x*fatness, y*height, -fatness + svl[2] ) ) ) {
						pos[2] = floor(pos[2]-fatness+svl[2]) + 1 + fatness+0.00390625;
						svl[2] = 0.0;
						vel[2] *= 0.9;
					}
				}
			}
		}
		pos[2] += svl[2];

		if( vel[1] > 0 ) {
			grounded=false;
			for( float x = -1; x <= 1 && svl[1] != 0.0; x++ ) {
				for( float z = -1; z <= 1 && svl[1] != 0.0; z++ ) {
					if( planet::collision3d( pos + cml::vector3d( x*fatness, height + svl[1], z*fatness ) ) ) {
						pos[1] = floor(pos[1]+height+svl[1]) - height - 0.00390625;
						svl[1] = 0.0f;
						vel[1] = 0.0f;
					}
				}
			}
		} else if( vel[1] < 0 ) {
			bool collided=false;
			for( float x = -1; x <= 1 && svl[1] != 0.0; x++ ) {
				for( float z = -1; z <= 1 && svl[1] != 0.0; z++ ) {
					
					if( planet::collision3d( pos + cml::vector3d( x*fatness, 0 + svl[1], z*fatness ) ) ) {
						collided=true;
						pos[1] = floor(pos[1]+0+svl[1]) + 1+0.00390625;
						svl[1] = 0.0f;
						vel[1] = 0.0f;
						if( 
							!grounded 
										) {
							vel *= 0.25;
							grounded=true;
						}
					}
				}
			}
			grounded=collided;
		}
		pos[1] += svl[1];
	}

	//grounded=false;
	for( float x = -1; x <= 1 && !grounded; x++ ) {
		for( float z = -1; z <= 1 && !grounded; z++ ) {
			if( planet::collision3d( pos + cml::vector3d( x*fatness, 0 - 0.0625, z*fatness ) ) ) {
				
				
				break;
			}
		}
	}
}

void Player::tick() {

	

	for( int time = gametime::frames_passed(); time; time-- ) {
		if(moving&MOVE_UP) {
			//if( grounded ) {
				vel[1] += 0.02;// jump_power;//thrust_power;
				grounded=false;
			//}
		}
		
		float wp;
		if( 1 ) {// grounded ){ DEBUG
			wp=(float)walk_power/4.0f;
			if( sprinting )
				wp = (float)run_power;
		} else {
			wp = (float)walk_power/16.0f  ;
		}
		if( moving&MOVE_FORWARD) {
			vel[0] += wp * sin(angle);
			vel[2] -= wp * cos(angle) ;
		}
	
		if( moving&MOVE_LEFT) {
			vel[0] += wp * sin(angle-pi/2);
			vel[2] -= wp * cos(angle-pi/2) ;
		}

		if( moving&MOVE_RIGHT) {
			vel[0] += wp * sin(angle+pi/2) ;
			vel[2] -= wp * cos(angle+pi/2) ;
		}

		if( moving&MOVE_BACK) {
			vel[0] -= wp * sin(angle);
			vel[2] += wp * cos(angle);
		}
		
			vel[1] -= gravity ;
		
		if( 1 ) {// !grounded ) { DEBUG
			
			vel[0] *= 0.995;
			vel[1] *= 0.995;;//*0.1;
			vel[2] *= 0.995;
			
		} else {
			bob += 0.15f ;//* vel.length(); 
			vel *= 0.8;
			//vel[0] *= 0.8;
			//vel[2] *= 0.8;
		}

#if 0
		cml::vector3f collpoints[8];
#define fatness 0.2
#define tallness 1.8
		collpoints[0] = cml::vector3f( -fatness, 0.0, -fatness ) ;
		collpoints[1] = cml::vector3f( -fatness, 0.0, fatness ) ;
		collpoints[2] = cml::vector3f( fatness, 0.0, fatness ) ;
		collpoints[3] = cml::vector3f( fatness, 0.0, -fatness ) ;
		collpoints[4] = cml::vector3f( -fatness, tallness, -fatness ) ;
		collpoints[5] = cml::vector3f( -fatness, tallness, fatness ) ;
		collpoints[6] = cml::vector3f( fatness, tallness, fatness ) ;
		collpoints[7] = cml::vector3f( fatness, tallness, -fatness ) ;
		poody:
		int cpoint = -1;
		cml::vector3f cix;
		float clen = 999.0;
		cml::vector3f cnormal;
		cml::vector3f vel2 = cml::normalize(vel) * 0.1 + vel;
		for( int i = 0; i < 8; i++ ) {
			cml::vector3f ix;
			cml::vector3f norm;
			
			if( planet::pick( pos , (pos+collpoints[i]) +vel, &ix, NULL, NULL, &norm ) ) {
				
				float l = cml::length( ix - (pos+collpoints[i]) );
				
				if( l < clen )  {
					clen = l;
					cix = ix;
					cpoint = i;
					cnormal=norm;
				}
				/*
				pos = ix - collpoints[i];
				if( norm[0] != 0.0f ) vel[0] = 0.0;
				if( norm[1] != 0.0f ) vel[1] = 0.0;
				if( norm[2] != 0.0f ) vel[2] = 0.0;

				if( cnormal[1] > 0 ) grounded=true;*/
			} 
		}
		
		if( cpoint != -1 ) {
			
			pos = cix - collpoints[cpoint];
			//pos += cnormal * 0.01 ;
			if( cnormal[0] != 0.0f ) vel[0] = 0.0;
			if( cnormal[1] != 0.0f ) vel[1] = 0.0;
			if( cnormal[2] != 0.0f ) vel[2] = 0.0;
			//vel[0] += cnormal[0];
			//vel[1] += cnormal[1];
			//v//el[2] += cnormal[2];


			if( cnormal[1] > 0 ) grounded=true;
			//vel = -vel*0.1;
			goto poody;
		} else {
			
			pos += vel;
		}
		

		/*
		#define fatness 0.4
		{
			cml::vector3f ix;
			cml::vector3f start, end;
			
			if( vel[0] > 0 ) {
				for( float y = 0.0; y < 2.0; y += 0.9 ) {
					start = pos + cml::vector3f( fatness, y, -fatness );
					end = start + cml::vector3f( vel[0], 0, 0 );
					if( planet::pick( start, end, &ix, NULL, NULL, NULL ) ) {
						pos[0] = ix[0] - (start[0] - pos[0]);
						vel[0] = 0;
					}
					start = pos + cml::vector3f( fatness, y, fatness );
					end = start + cml::vector3f( vel[0], 0, 0 );
					if( planet::pick( start, end, &ix, NULL, NULL, NULL ) ) {
						pos[0] = ix[0] - (start[0] - pos[0]);
						vel[0] = 0;
					}
				}

			} else if( vel[0] < 0 ) {
				for( float y = 0.0; y < 2.0; y += 0.9 ) {
					start = pos + cml::vector3f( -fatness, y, -fatness );
					end = start + cml::vector3f( vel[0], 0, 0 );
					if( planet::pick( start, end, &ix, NULL, NULL, NULL ) ) {
						pos[0] = ix[0] - (start[0] - pos[0]);
						vel[0] = 0;
					}
					start = pos + cml::vector3f( -fatness, y, fatness );
					end = start + cml::vector3f( vel[0], 0, 0 );
					if( planet::pick( start, end, &ix, NULL, NULL, NULL ) ) {
						pos[0] = ix[0] - (start[0] - pos[0]);
						vel[0] = 0;
					}
				}
			}
			pos[0] += vel[0];
			
			if( vel[2] > 0 ) {
				for( float y = 0.0; y < 2.0; y += 0.9 ) {
					start = pos + cml::vector3f( -fatness, y, fatness );
					end = start + cml::vector3f( 0, 0, vel[2] );
					if( planet::pick( start, end, &ix, NULL, NULL, NULL ) ) {
						pos[2] = ix[2] - (start[2] - pos[2]);
						vel[2] = 0;
					}
					start = pos + cml::vector3f( fatness, y, fatness );
					end = start + cml::vector3f( 0, 0, vel[2] );
					if( planet::pick( start, end, &ix, NULL, NULL, NULL ) ) {
						pos[2] = ix[2] - (start[2] - pos[2]);
						vel[2] = 0;
					}
				}

			} else if( vel[2] < 0 ) {
				for( float y = 0.0; y < 2.0; y += 0.9 ) {
					start = pos + cml::vector3f( -fatness, y, -fatness );
					end = start + cml::vector3f( 0, 0, vel[2] );
					if( planet::pick( start, end, &ix, NULL, NULL, NULL ) ) {
						pos[2] = ix[2] - (start[2] - pos[2]);
						vel[2] = 0;
					}
					start = pos + cml::vector3f( fatness, y, -fatness );
					end = start + cml::vector3f( 0, 0, vel[2] );
					if( planet::pick( start, end, &ix, NULL, NULL, NULL ) ) {
						pos[2] = ix[2] - (start[2] - pos[2]);
						vel[2] = 0;
					}
				}
			}
			pos[2] += vel[2];
			
			if( vel[1] > 0 ) {
				for( float x = -1.0; x <= 1.0; x += 1.0 ) {
					for( float z = -1.0; z <= 1.0; z++ ) {
						start = pos + cml::vector3f( x*fatness, 1.8, z*fatness );
						end = start + cml::vector3f( 0, vel[1], 0 );
						if( planet::pick( start, end, &ix, NULL, NULL, NULL ) ) {
							pos = ix - (start - pos);
							vel[1] = 0;
						}
					}
				}

			} else if( vel[1] < 0 ) {
				for( float x = -1.0; x <= 1.0; x += 1.0 ) {
					for( float z = -1.0; z <= 1.0; z++ ) {
						start = pos + cml::vector3f( x*fatness, 0.0, z*fatness );
						end = start + cml::vector3f( 0, vel[1], 0 );
						if( planet::pick( start, end, &ix, NULL, NULL, NULL ) ) {
							pos = ix - (start - pos);
							vel[1] = 0;
							grounded=true;
						}
					}
				}
			}
			pos[1] += vel[1];
		}*/

		
		{
			if( grounded ) {
				bool onground=false;
				for( int i = 0; i < 4; i++ ) {
			
					if( planet::pick( pos  +collpoints[i], (pos+collpoints[i]) +cml::vector3f(0,-0.3,0), NULL, NULL, NULL, NULL ) ) {
						onground=true;
						break;
					} 
				}
				grounded=onground;
			}
		}
#endif
		apply_movement();

		/*
		pos[0] += vel[0];
		
		
		if( vel[0] > 0 ) {
			if( planet::collision(((int)(pos[0]+1.0)) & planet::PLANET_MASK,(int)(pos[1]),((int)pos[2])&planet::PLANET_MASK) ) {
				vel[0] = 0;
				pos[0] = floor(pos[0]+1.0)-1.0;
			}
		} else if( vel[0] < 0 ) {
			if( planet::collision(((int)(pos[0]-1.0)) & planet::PLANET_MASK,(int)pos[1], ((int)pos[2])&planet::PLANET_MASK) ) {
				vel[0] = 0;
				pos[0] = floor(pos[0]-1.0)+1.0+1.0;
			}
		}
	
		pos[2] += vel[2] ;
	
		if( vel[2] > 0 ) {
			if( planet::collision(((int)pos[0])&planet::PLANET_MASK,(int)(pos[1]),((int)(pos[2]+1.0))&planet::PLANET_MASK) ) {
				vel[2] = 0;
				pos[2] = floor(pos[2]+1.0)-1.0;
			}
		} else if( vel[2] < 0 ) {
			if( planet::collision(((int)pos[0])&planet::PLANET_MASK,(int)pos[1], ((int)(pos[2]-1.0))&planet::PLANET_MASK) ) {
				vel[2] = 0;
				pos[2] = floor(pos[2]-1.0)+1.0+1.0;
			}
		}

		pos[1] += vel[1];

	#define HUMAN_HEIGHT 1.8
	
		if( vel[1] < 0 ) {
			if( planet::collision(((int)pos[0])&planet::PLANET_MASK,(int)(pos[1]-HUMAN_HEIGHT),((int)pos[2])&planet::PLANET_MASK) ) {
				vel[1]=0;
				pos[1] = floor(pos[1]-HUMAN_HEIGHT)+1.0+HUMAN_HEIGHT;
				grounded=true;
			} else {
				grounded=false;
				bob=0.0;
			}
		} else if( vel[1] > 0 ) {
			grounded=false;
				bob=0.0;
			if( planet::collision(((int)pos[0])&planet::PLANET_MASK,(int)(pos[1]+HUMAN_HEIGHT),((int)pos[2])&planet::PLANET_MASK) ) {
				vel[1]=0;
				pos[1] = floor(pos[1]+HUMAN_HEIGHT)-HUMAN_HEIGHT;
			}
		}
		*/

		//if(pos[1] < (8+2.76f) ) {pos[1] = (8+2.76f); vel[1]=0;}
	}
}
/*
void Player::getEyePoints( vector64 &base, vector64 &point ) const {
	base.x = pos[0];
	base.y = pos[1];
	base.z = pos[2];

	// point forward
	vector64 temp;
	point.x = 0;
	point.y = 0;
	point.z = -1;
	
	// rotate angle
	D3DXMATRIXA16 rot;
	
	//D3DXMatrixRotationX( &rot, pitch );
	
	temp.x = point.x * rot._11 + point.y * rot._12 + point.z * rot._13;
	temp.y = point.x * rot._21 + point.y * rot._22 + point.z * rot._23;
	temp.z = point.x * rot._31 + point.y * rot._32 + point.z * rot._33;
	
		//D3DXMatrixRotationY( &rot, -angle );
	
	point.x = temp.x * rot._11 + temp.y * rot._12 + temp.z * rot._13;
	point.y = temp.x * rot._21 + temp.y * rot._22 + temp.z * rot._23;
	point.z = temp.x * rot._31 + temp.y * rot._32 + temp.z * rot._33;
	
	// translate
	point.x += base.x;
	point.y += base.y;
	point.z += base.z;

}
*/
float Player::getHeadBob() const {
	return bob;
}

float Player::getHeadBobScale() const {
	if( !grounded ) {
		return 0.0;
	} else {
		
		return (float)vel.length()*90.0f;
	}
}

}


#endif
