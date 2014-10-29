//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//


#pragma once

#include "util/math.h"

namespace Game {

class GameCamera {

public: 
	Video::Camera camera;
	Eigen::Vector3f orientation;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	GameCamera() {
		orientation = Eigen::Vector3f( 0.0f, 1.0f, 0.0f );
	}



	void UpdateVideo() {
		camera.Fixup();
		camera.UpdateVideo();
	}

	void SetPosition( const Eigen::Vector3f &pos ) {
		camera.SetPosition( pos );
	}

	void LookAt( const Eigen::Vector3f &pos ) {
		camera.LookAt( pos, orientation );
	}

	void SetOrientation( const Eigen::Vector3f &up ) {
		orientation = up;
	}

	void Pan( float mx, float my ) {

		using Util::DegToRad;
		
		if( my < 0.0f ) {
			float ang = my * 0.001f;
			Eigen::Vector3f fwd = orientation.cross(camera.Right()).normalized();
			float theta = acos(fwd.dot(camera.Forward()));
			camera.Rotate( -ang );
			float theta2 = acos(fwd.dot(camera.Forward()));
			if( theta2 > theta ) {
				// we are moving away from the center
				if( theta2 > DegToRad(95.0f) ) {
					// we went too far
					camera.Rotate( -(theta2 - DegToRad(95.0f)) );
				}
			} 
		} else if( my > 0.0f ) {
			float ang = -my * 0.001f;
			Eigen::Vector3f fwd = orientation.cross(camera.Right()).normalized();
			float theta = acos(fwd.dot(camera.Forward()));
			camera.Rotate( ang );
			float theta2 = acos(fwd.dot(camera.Forward()));
			if( theta2 > theta ) {
				// we are moving away from the center
				if( theta2 > DegToRad(95.0f) ) {
					// we went too far
					camera.Rotate( (theta2 - DegToRad(95.0f)) );
				}
			} 
		}
		camera.Rotate( 0.0f, -mx * 0.001f );
	}

	// 
	void MoveRel( const Eigen::Vector3f &amt ) {
		if( amt[0] != 0.0f ) {
			camera.Move( camera.Right() * amt[0] );
		}
		if( amt[1] != 0.0f ) {
			camera.Move( camera.Up() * amt[1] );
		}
		if( amt[2] != 0.0f ) {
			camera.Move( camera.Forward() * amt[2] );
		}
	}

	void Rotate( const Eigen::Vector3f &angles ) {
		camera.Rotate(angles);
	}

	void OnTick() {
		using Util::DegToRad;
		

		Eigen::Vector3f oriented_up = orientation.cross(camera.Forward()).normalized();
		Eigen::AngleAxisf a( 3.14159f/2.0f, camera.Forward());
		float roll = -oriented_up.dot(camera.Up());
		oriented_up = a * oriented_up;
		float tolerance = orientation.dot(camera.Forward());
		float balance = acos(oriented_up.dot(camera.Up()));

		float horizon = abs(Util::PI/2.0f - (acos(tolerance)));

		float deadzone = DegToRad( 80.0f );
		float strictzone = DegToRad( 5.0f );
 
		float speed = (horizon - strictzone) / (deadzone-strictzone);// * 180.0f;
		if( speed < 0.0f ) speed = 0.0f;
		if( speed > 1.0f ) speed = 1.0f;
		speed = 1.0f - speed; 
		speed = 1.0f - pow((1.0f-speed),0.3f);
		speed *= 0.3f; 
				
		float amount = balance; 
				
		if( amount > 0.0f ) {
			 
			if( roll < 0.0f ) {
				amount = -amount;
			}
			camera.Rotate( 0.0f, 0.0f, amount * speed ); 
		}
	}
};

}
