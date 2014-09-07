//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------

#pragma once

//-------------------------------------------------------------------------------------------------
#ifndef UTIL_SCRIPTPARSER_H
#define UTIL_SCRIPTPARSER_H

//-------------------------------------------------------------------------------------------------
namespace Util {

//-------------------------------------------------------------------------------------------------
enum {
	SCRIPT_SPACE,
	SCRIPT_LABEL,
	SCRIPT_INSTRUCTION,
	SCRIPT_ERROR
};

enum {
	SCRIPTERR_BADFILE,
	SCRIPTERR_UNEXPECTEDCHAR,
	SCRIPTERR_TOOLONG
};

//-------------------------------------------------------------------------------------------------
typedef struct t_ScriptParserData {
	int type;
	int error_id;
	int script_line;
	int errcol;
	int errdata;

	const char *args[8];
	int argc;
} ScriptParserData;

//-------------------------------------------------------------------------------------------------
// the script parser calback
// return false for an error
// return true to continue parsing
typedef bool (*ScriptParserCallback)( const ScriptParserData *script_parser_data, void *user_data );

//-------------------------------------------------------------------------------------------------
class ScriptParser {

	enum {
		MAXLINESIZE = 1024
	};

	ScriptParserData data;
	ScriptParserCallback callback;
	void *user_data;

	char *text;
	int textsize;
	int position;
	char line[MAXLINESIZE+1];
		
	void FireError( int id, int errdata = 0, int position = 0 );
	void ResetPosition();
	void AlignNextLine();
	void ReadLine();
	bool EndOfText();
	void ParseScript();
	bool ParseLine();
public:
	ScriptParser( const char *file, ScriptParserCallback func, void *p_user_data );
	~ScriptParser();
};

}

//-------------------------------------------------------------------------------------------------
#endif
