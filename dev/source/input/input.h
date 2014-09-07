//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once
 
namespace Input {
	void Init();
	void Update();
	bool KeyPressed( int index );
	void Unload();

	
	int MouseRX();
	int MouseRY();
	int MouseRZ();
	int MouseX();
	int MouseY();
	int MouseZ();

	bool MousePressed( int index );
	bool MouseClicked( int index );
	bool MouseUp( int index );
	cml::vector2f GetMouse2f();

	void AcquireMouse();
	void LockMouse();
	void UnlockMouse();

	
	void DisableInput();
	void EnableInput();
	


	void Begin();
	void End();
	void WINDOWS_MOUSE_DOWN( int wParam );
	void WINDOWS_MOUSE_UP( int wParam );
	//void WINDOWS_CHAR( char c, int count );

	//char PeekCharBuffer();
	//char ReadCharBuffer();
	//void FlushCharBuffer();

	//int PeekKeyBuffer();
	//int ReadKeyBuffer();
	//void FlushKeyBuffer();
};
