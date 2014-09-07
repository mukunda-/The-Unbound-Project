//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"



namespace Input {

	LPDIRECTINPUT8	di;
	
	LPDIRECTINPUTDEVICE8	Keyboard=0;
	LPDIRECTINPUTDEVICE8	Mouse=0;

	DIMOUSESTATE2			dimousestate;
	char					PreviousKeyBuffer[256];
	char					KeyBuffer[256];
	
	//std::queue<char>		charBuffer;
	//std::queue<int>			keyBuffer;

	struct WMS {
		bool buttons[2];
		bool down[2];
		bool up[2];
		int x;
		int y;
		int rawx;
		int rawy;
		
		//int lastMouseX;
		//int lastMouseY;
		//int lastMouseZ;
		
	} MouseState;

	bool mouse_locked= true;
	int mouselock_x;
	int mouselock_y;

	bool disable_input=false;
	//bool lastMousePressed[8];

	//enum { MM_WINDOWS,	MM_DIRECT } mouse_mode;
 
	void Init() {
		DirectInput8Create( Video::GetInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&di, NULL );
		di->CreateDevice( GUID_SysKeyboard, &Keyboard, NULL );
		di->CreateDevice( GUID_SysMouse, &Mouse, NULL );

		Keyboard->SetDataFormat( &c_dfDIKeyboard );
		Keyboard->SetCooperativeLevel( Video::GetWindowHandle(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE );
		Keyboard->Acquire();

		Mouse->SetDataFormat( &c_dfDIMouse2 );
		Mouse->SetCooperativeLevel( Video::GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
		Mouse->Acquire();
		//mouse_mode = MM_DIRECT;
		memset( (void*)&MouseState, 0, sizeof(WMS) );
		memset( (void*)&dimousestate, 0, sizeof(DIMOUSESTATE) );

		ShowCursor(false);
	}

	void AcquireMouse() {
		if( Mouse ) {
			Mouse->Acquire();
			//mouse_mode = MM_DIRECT;
		}
	}

	void DisableInput() {
		disable_input=true;
	}

	void EnableInput() {
		disable_input=false;
	}

	//void UnacquireMouse() {
		//Mouse->Unacquire();
		//mouse_mode = MM_WINDOWS;
	//}
	
	void CenterMouse() {
		RECT cr;
		GetWindowRect( Video::GetWindowHandle(), &cr );

		MouseState.x = cr.left + Video::ScreenWidth()/2;
		MouseState.y = cr.top + Video::ScreenHeight()/2;

		

		SetCursorPos( MouseState.x, MouseState.y );
		
	}

	void LockMouse() {
		if( !mouse_locked ) {
			mouse_locked=true;
			mouselock_x = MouseState.x;
			mouselock_y = MouseState.y;
			CenterMouse();
			ShowCursor(false);
		}
		//ShowCursor(b);
	}

	void UnlockMouse() {
		if( mouse_locked ) {
			mouse_locked=false;
			SetCursorPos( mouselock_x, mouselock_y );
			ShowCursor(true);
		}

		//ShowCursor(b);
	}
	

	bool MousePressed( int index ) {
		return MouseState.buttons[index];
		//return !!(MouseState.rgbButtons[index] & 0x80);
	}

	bool MouseClicked( int index ) {
		return MouseState.down[index];
		//if( mouse_mode == MM_DIRECT ) {
		//	return (!!(MouseState.rgbButtons[index] & 0x80)) && (!(lastMousePressed[index]));
		//} else {
		//}
	}

	bool MouseUp( int index ) {
		return MouseState.up[index];
		//return (!(MouseState.rgbButtons[index] & 0x80)) && ((lastMousePressed[index]));
	}
	/*
	char PeekCharBuffer() {
		if( charBuffer.empty() ) {
			return 0;
		} else {
			return charBuffer.front();
		}
	}

	char ReadCharBuffer() {
		if( charBuffer.empty() ) {
			return 0;
		} else {
			char c = charBuffer.front();
			charBuffer.pop();
			return c;
		}
		
	}

	void FlushCharBuffer() {
		std::queue<char> empty;
		std::swap( charBuffer, empty );
		
	}

	int PeekKeyBuffer() {
		if( keyBuffer.empty() ) {
			return 0;
		} else {
			return keyBuffer.front();
		}
	}

	int ReadKeyBuffer() {
		if( keyBuffer.empty() ) {
			return 0;
		} else {
			char c = keyBuffer.front();
			keyBuffer.pop();
			return c;
		}
	}

	void FlushKeyBuffer() {
		std::queue<int> empty;
		std::swap( keyBuffer, empty );
	}
	*/
	void Begin() {
		if( !disable_input ) {
			Keyboard->GetDeviceState( sizeof( KeyBuffer ), (LPVOID)&KeyBuffer );

			//lastMouseX = MouseState.lX;
			//lastMouseY = MouseState.lY;
		//	lastMouseZ = MouseState.lZ;
		//	for( int i = 0; i < 8; i++ )
		//		lastMousePressed[i] = MousePressed(i);


			//if( DIERR_INPUTLOST == Mouse->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&dimousestate) ) {
			//	Mouse->Acquire();
			//		
			//} else {
			//	MouseState.rawx = dimousestate.lX;
			//	MouseState.rawy = dimousestate.lY;
			//	/*	
			//	for( int b = 0; b < 2; b++ ) {
			//		if( !MouseState.buttons[b] ) {
			//			MouseState.buttons[b] = !!(dimousestate.rgbButtons[b] & 0x80);
			//			if( MouseState.buttons[b] ) MouseState.down[b] = true;
			//		} else {
			//			MouseState.buttons[b] = !!(dimousestate.rgbButtons[b] & 0x80);
			//			if( !MouseState.buttons[b] ) MouseState.up[b] = true;
			//		}

			//	}*/
			//}
	//
			if( mouse_locked ) {

				POINT p;
				RECT cr;
				GetCursorPos( &p );
				GetWindowRect( Video::GetWindowHandle(), &cr );
			
				MouseState.x = p.x - cr.left;
				MouseState.y = p.y - cr.top;
				
				MouseState.rawx = MouseState.x - Video::ScreenWidth() / 2;
				MouseState.rawy = MouseState.y - Video::ScreenHeight() / 2;

				CenterMouse();
			
			} else {
				MouseState.rawx=0;
				MouseState.rawy=0;
		
				POINT p;
				RECT cr;
				GetCursorPos( &p );
				GetWindowRect( Video::GetWindowHandle(), &cr );
			
				MouseState.x = p.x - cr.left;
				MouseState.y = p.y - cr.top;
			//}
			}	
		} else {
			MouseState.rawx = 0;
			MouseState.rawy = 0;
		}
	}
	
	void End() {
		if( !disable_input ) {
			//FlushCharBuffer();
			//FlushKeyBuffer();
			MouseState.down[0] = false;
			MouseState.down[1] = false;

			MouseState.up[0] = false;
			MouseState.up[1] = false;
			//MouseState.lastMouseX = MouseState.x;
			//MouseState.lastMouseY = MouseState.y;
		}
	}

	bool KeyPressed( int index ) {
		return !!(KeyBuffer[index] & 0x80);
	}

	

	int MouseRX() {
		return MouseState.rawx;
	}

	int MouseRY() {
		return MouseState.rawy;// - MouseState.lastMouseY;
	}
	/*
	int MouseRZ() {
		return MouseState.z - MouseState.lastMouseZ;
	}*/

	int MouseX() {
		return MouseState.x;
	}

	int MouseY() {
		return MouseState.y;
	}
	
	cml::vector2f GetMouse2f() {
		return cml::vector2f( (float)MouseX(), (float)MouseY() );
	}

	/*
	int MouseZ() {
		return MouseState.lZ;
	}*/

	void Unload() {
		if( Keyboard != NULL ) {
			Keyboard->Unacquire();
			Keyboard->Release();
			Keyboard = NULL;
		}
		
		if( Mouse != NULL ) {
			Mouse->Unacquire();
			Mouse->Release();
			Mouse = NULL;
		}
	}
	 
	void WINDOWS_MOUSE_DOWN( int msg ) {
		if( msg == WM_LBUTTONDOWN ) {
			MouseState.down[0] = true;
			MouseState.buttons[0] = true;
		}

		if( msg == WM_RBUTTONDOWN ) {
			MouseState.down[1] = true;
			MouseState.buttons[1] = true;
		}
	}

	void WINDOWS_MOUSE_UP( int msg ) {
		if( msg == WM_LBUTTONUP ) {
			MouseState.up[0] = true;
			MouseState.buttons[0] = false;
		}

		if( msg == WM_RBUTTONUP ) {

			MouseState.up[1] = true;
			MouseState.buttons[1] = false;
		}
	}
	/*
	void WINDOWS_CHAR( char c, int repeat ) {
		for( ;repeat--; ) {
			charBuffer.push(c);
		}
	}
	
	void WINDOWS_VKEY( int c, int repeat ) {
		for( ;repeat--; ) {
			keyBuffer.push(c);
		}
	}*/
};
