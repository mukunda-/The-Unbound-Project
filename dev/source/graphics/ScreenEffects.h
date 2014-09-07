//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#ifndef SCREENSHADER_H
#define SCREENSHADER_H

namespace screenshader {

void latch();
void unlatch();
void render();

void init();
void setSineEffect( float offset, float frequency, float depth );
void setZoomEffect( float zoom );
void setHorizontalBlur( float s );
void setColorOverlay( float r, float g, float b, float a );
void setColorMultiply( float a );
void SetScreenFlash( float intensity );
void UpdateScreenFlash();

void setPreset_Underwater( float time );
void setPreset_Normal();

};

#endif
