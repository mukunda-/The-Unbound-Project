//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#pragma once

#ifndef GAMETIME_H
#define GAMETIME_H

namespace gametime {

	double getTimer();
	double getSecondsMod( double m, double offset = 0.0 );
	
	void newTick();
	double passed();
	int frames_passed();
	void reset();
};

#endif
