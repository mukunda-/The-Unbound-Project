//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#include "stdafx.h"

//-------------------------------------------------------------------------------------------------
namespace Util {

//-------------------------------------------------------------------------------------------------
void ScriptParser::FireError( int id, int errdata, int column ) {
	data.error_id = id;
	data.type = SCRIPT_ERROR;
	data.errdata = errdata;
	data.errcol = column;
	callback( &data, user_data );
}

//-------------------------------------------------------------------------------------------------
void ScriptParser::AlignNextLine() {
	if( position >= textsize ) return;
	if( text[position] == 0 ) return;
	if( text[position] == '\r' ) position++;
	if( position >= textsize ) return;
	if( text[position] == 0 ) return;
	if( text[position] == '\n' ) position++;
}

//-------------------------------------------------------------------------------------------------
void ScriptParser::ReadLine() {
	data.script_line++;
	line[0] = 0;
	int i;
	for( i = 0; i < MAXLINESIZE; i++ ) { 
		if( position >= textsize ) {
			break;
		}

		line[i] = text[position];

		if( text[position] == 0 ) {
			break;
		}

		if( text[position] == '\r' || text[position] == '\n' ) {
			break;
		}
	}
	AlignNextLine();
	line[i] = 0;
}

//-------------------------------------------------------------------------------------------------
void ScriptParser::ResetPosition() {
	position = 0;
}

//-------------------------------------------------------------------------------------------------
bool ScriptParser::EndOfText() {
	return position == textsize;
}

enum {
	STATE_TERMSCAN,
};

static bool IsWhitespace( char a ) {
	return (a == ' ' || a == '\t');
}

//-------------------------------------------------------------------------------------------------
bool ScriptParser::ParseLine() {
 
	ReadLine();
	data.type = SCRIPT_SPACE;
	for( int i = 0; i < 8; i++ ) {
		data.args[i] = 0;
	}
	data.argc = 0;
	// strip comments
	char *str = strchr( line, ';' );

	if( str ) {
		// strip comment
		str[0] = 0;
	}
	TrimString( line );

	// EXAMPLE: oipoiawegoihagioe error
	//           example2    arg1, "   arg2","arg3"


	if( line[0] == 0 ) return true; // empty line

	char term[512];

	char args[8][512];
	
	int termw = 0;

	int state = STATE_TERMSCAN;
	int termindex = 0;
	int quoted = 0;
	int pos;
	int findterm = 1;

	for( pos = 0; line[pos]; pos++ ) {
		char inp = line[pos];

		switch( termindex ) {
		case -1:
			if( !IsWhitespace( inp ) ) {
				FireError( SCRIPTERR_UNEXPECTEDCHAR, inp, pos );
				return false;
			}
			break;

		case 0:
			if( findterm ) {
				if( IsWhitespace( inp ) ) {
					continue;
				} else {
					findterm = false;
				}
			}

			if( inp == ':' ) {
				// label
				term[termw] = 0;
				strcpy_s<sizeof(args[0])>( args[0], term );
				data.type = SCRIPT_LABEL;
				data.args[0] = args[0];
				data.argc++;
				termindex = -1;
			}
			if( IsWhitespace( inp ) ) {
				// start of instruction
				findterm = 1;
				term[termw] = 0;

				strcpy_s<sizeof(args[0])>( args[0], term );
				data.type = SCRIPT_INSTRUCTION;
				data.args[0] = args[0];
				termw = 0;
				termindex++;
				data.argc++;
			} else {
				if( termw >= sizeof(term)-1 ) {
					FireError( SCRIPTERR_TOOLONG, 0, pos );
					return false;
				}
				term[termw++] = inp;
			}
			break;
		default:
			if( findterm ) {
				if( IsWhitespace( inp ) ) {
					continue;
				} else {
					findterm = 0;
				}
			}

			if( inp == ',' ) {
				// end of term
				term[termw] = 0;
				termw = 0;
				strcpy_s( args[termindex], sizeof(args[0]), term );
				
				TrimString( args[termindex] );
				StripQuotes( args[termindex] );
				
				termindex++;
				findterm = 1;
			}
		}
		/*
		if( state == STATE_TERMSCAN ) {
			if( inp == '"' ) {
				quoted = !quoted;
				continue;
			}

			if( !quoted ) {
				if( inp == ':' ) { // this term is a label
					if( termindex != 0 ) {
						// error
						FireError( SCRIPTERR_UNEXPECTEDCHAR, inp, pos );
						return;
					}
					// end of label statement
					data.type = SCRIPT_LABEL;
					data.label_string = label;
					strcpy_s<sizeof(label)>( label, term );

					state = STATE_EXPECTEND;
					continue;
				}

				if( termindex == 0 )
				if( inp == ',' ) {
					if( termindex == 0 ) {
						FireError( SCRIPTERR_UNEXPECTEDCHAR, inp, pos );
						return;
					}
					// end of term
					strcpy_s<sizeof(args[0])>( args[termindex], term );

				}
				if( IsWhitespace( inp ) ) {
					// end of term

				}
			}

			term[termw] = inp;
		} else if( state == STATE_EXPECTEND ) {
			FireError( SCRIPTERR_UNEXPECTEDCHR, inp, pos );
		}
		*/

	}

	if( data.type == SCRIPT_LABEL ) {
	 

		return callback( &data, user_data );
	} else if( data.type == SCRIPT_INSTRUCTION ) {
		term[termw] = 0;
		termw = 0;
		strcpy_s( args[termindex], sizeof(args[0]), term );
		termindex++;
		TrimString( args[termindex] );
		StripQuotes( args[termindex] );
		return callback( &data, user_data );
	}

	return true;

	//term[termw] = 0;
	//termw = 0;
	//strcpy_s( args[termindex], sizeof(args[0]), term );
				
	//TrimString( args[termindex] );
	//StripQuotes( args[termindex] );
				
	//termindex++;
	/*
	// CHNGE TO STATE MACHINE
	char work[256];
	ScanArgString( str, work, sizeof(work), " \t" );
	TrimString(work);
	if( work[0] ) {
		int len = strlen(work);
		if( work[len-1] == ':' ) {
			// its a label

		}
	}*/
}

//-------------------------------------------------------------------------------------------------
void ScriptParser::ParseScript() {
	while( !EndOfText() ) {
		if( !ParseLine() ) break;
	}
}

//-------------------------------------------------------------------------------------------------
ScriptParser::ScriptParser( const char *p_file, ScriptParserCallback func, void *p_user_data ) {
	user_data = p_user_data;
	callback = func;
	FILE *f = IO::OpenFile( p_file, "r" );
	if( !f ) {
		FireError( SCRIPTERR_BADFILE );
		return;
	}

	
	fseek( f, SEEK_END, 0 );
	textsize = ftell( f );
	text = new char[textsize];
	fseek( f, SEEK_SET, 0 );
	fread_s( text, textsize, 1, textsize, f );
	fclose(f);

	memset(&data,0,sizeof(data));
	data.script_line = -1;

	ResetPosition();
	ParseScript();
	

}

//-------------------------------------------------------------------------------------------------
ScriptParser::~ScriptParser() {
	if( text ) {
		delete[] text;
	}
	
}

//-------------------------------------------------------------------------------------------------
}
