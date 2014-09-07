//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// ServerConsole
//
// thread safe PDCURSES console interface and hardcoded layout
//

#pragma once
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
namespace System {

//-------------------------------------------------------------------------------------------------
namespace ServerConsole {

static const int SIDEBAR_WIDTH  = 20;
static const int SIDEBAR_HEIGHT = 22;

void SetTitle( const char *text );
/*
void Print( const char *format, ... );
void PrintEx( const char *format, ... );
void PrintError( const char *format, ... );
void PrintErrorEx( const char *format, ... );*/
void PrintToWindow( const char * format, bool newline, va_list args );
void PrintToWindow( const char * text, bool newline  );
void GetInput( char *input, int maxlen );
void GetInputEx( const char *prompt, char *input, int maxlen );

void Update();

void SetMenuItem( int line, const char *format, bool update=true, ... );

struct Init {
	Init();
	~Init();
};

//-------------------------------------------------------------------------------------------------
}

}