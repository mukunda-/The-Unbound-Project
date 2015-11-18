//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//


#pragma once

#include "util/math.h"
#include "video/video.h"

namespace Game {

class GameCamera {

public: 
	Video::Camera camera;
	Eigen::Vector3f orientation;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	GameCamera();

	/** -----------------------------------------------------------------------
	 * Update the video matrices for rendering.
	 */
	void UpdateVideo();

	/** -----------------------------------------------------------------------
	 * Set the position of the camera in the world.
	 */
	void SetPosition( const Eigen::Vector3f &pos );

	/** -----------------------------------------------------------------------
	 * Set where the camera is looking at.
	 */
	void LookAt( const Eigen::Vector3f &pos );

	/** -----------------------------------------------------------------------
	 * Set which way is 'up' on the camera.
	 */
	void SetOrientation( const Eigen::Vector3f &up );

	/** -----------------------------------------------------------------------
	 * Rotate the camera using a mouse.
	 *
	 * @param mx,my Pixels the mouse has moved.
	 */
	void Pan( float mx, float my );

	/** -----------------------------------------------------------------------
	 * Move the camera.
	 */
	void MoveRel( const Eigen::Vector3f &amt );

	/** -----------------------------------------------------------------------
	 * Rotate the camera.
	 *
	 * @param angles Euler angles in radians.
	 */
	void Rotate( const Eigen::Vector3f &angles );

	/** ------------------------------------------------------------------------
	 * Called on every frame to update the camera movement.
	 */
	void OnTick();
};

}
