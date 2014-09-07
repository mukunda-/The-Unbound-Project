//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#include "stdafx.h"

namespace TitleScreen {

void OnExitButton( void *source, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch( msg ) {
	case WM_LBUTTONUP:
		PostQuitMessage(0);
	}
	
}

void pooper( void *source, UINT msg, WPARAM wParam, LPARAM lParam) {
	hud::gui_object *src = (hud::gui_object*)source;

	switch( msg ) {
	case WM_USER:
		switch( wParam & 255 ) {
		case hud::USER_MSG_DROP:
			{
				hud::gui_object *from = (hud::gui_object*)lParam;
			
				src->swap_item( *from );
			}
			break;
		case hud::USER_MSG_NULLDRAG:
			src->remove_item();
		}
	}
}

void Render() {

	Video::UseGlobalSurface();
	
	hud::render();
	Video::swap();
}

void Start() {

	printf( "Starting Title Screen\n" );
	
	hud::set_mode( hud::MODE_TITLE );

	gametime::reset();
	
	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );
	while( msg.message != WM_QUIT ) {
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ) {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        } else {

			// graphics
			Render();

			// logic
			Input::Begin();

			if( Input::KeyPressed( DIK_ESCAPE ) ) {
				PostQuitMessage(0);
			}


			
			hud::update();
			
			Input::End();


			hud::draw();
		}
	} 
}

}
