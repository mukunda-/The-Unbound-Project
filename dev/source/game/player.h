//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#pragma once

#ifndef PLAYER_H
#define PLAYER_H

namespace Players {

enum {
	MOVE_LEFT=1,
	MOVE_RIGHT=2,
	MOVE_FORWARD=4,
	MOVE_BACK=8,
	MOVE_UP=16,
	MOVE_DOWN=32
};

class Player {

private:
	void apply_movement();

public:
	cml::vector3d pos;
	cml::vector3d vel;

	int moving;

	double angle;
	double pitch;
	float bob;

	bool grounded;
	bool sprinting;

//	void getEyePoints( vector64 &base, vector64 &point ) const;
	void Player::reset( float x, float y, float z );
	void look( double pangle, double ppitch, bool relative );

	void Player::move( int pmoves );
	void Player::tick();

	float Player::getHeadBob() const ;
	float Player::getHeadBobScale() const ;

	void sprint( bool );
};

}

#endif
