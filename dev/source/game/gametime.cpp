//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#include "stdafx.h"

namespace gametime {

double lastTime = 0.0;
double currentTime = 0.0;
double timePassed = 0.0;

int resetTime;
double remainder=0.0;

int framethingy=0;

void reset() {
	lastTime = 0.0;
	currentTime = 0.0;
	timePassed = 0.0;
	resetTime = clock();
	remainder=0.0;
}

double getTimer() {
	int clocks = clock() - resetTime;
	double time = (double)clocks / ((double)CLOCKS_PER_SEC / 1000000.0);
	// todo: time wrapping
	return time;
}

double getSecondsMod( double m, double offset ) {
	return fmod(getTimer()/1000000.0 + offset,m);
}

void newTick() {
	double newTime = getTimer();
	timePassed = newTime - currentTime;
	currentTime = newTime;

	float frames = (float)(timePassed / 16666.666666666666666666666666667 + remainder);
	int int_frames = (int)floor(frames);
	remainder = frames - int_frames;
	framethingy= int_frames;
}

double passed() {
	return timePassed;
}


int frames_passed() {
	return framethingy;
}

} // namespace gametime