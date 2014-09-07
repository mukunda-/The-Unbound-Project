//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "keyvalues.h"
#include "fopen2.h"
#include "system/console.h"

//-------------------------------------------------------------------------------------------------
namespace Util {

//-------------------------------------------------------------------------------------------------
KeyValues::KeyValues() {
	current_section = &root;
	current_index = 0;
	current_level = 0;
}

//-------------------------------------------------------------------------------------------------
void KeyValues::DeleteSection( Section *section ) {
	if( !section ) return;
	for( unsigned int i = 0; i < section->values.size(); i++ ) {
		DeleteSection( section->values[i].section );
	}
}

//-------------------------------------------------------------------------------------------------
void KeyValues::Erase() {

	for( unsigned int i = 0; i < root.values.size(); i++ ) {
		DeleteSection( root.values[i].section );
	}
	
	Rewind();

}

//-------------------------------------------------------------------------------------------------
bool KeyValues::ParseKVFile( const char *file ) {
	char *contents;
	
	FILE *f = fopen2( file, "rb" );
	if( !f ) return false;
	fseek( f, 0, SEEK_END );
	int length = ftell(f);
	fseek( f, 0, SEEK_SET );
	contents = new char[length+2];
	fread( contents, 1, length, f );
	fclose(f);

	contents[length] = ' '; // add extra whitespace
	contents[length+1] = 0;

	// replace whitespace
	for( int i = 0; contents[i]; i++ ) {
		if( contents[i] > 0 && contents[i] < 32 ) contents[i] = ' ';
	}

	char *read = contents;

	int state = 0;
	// 0 = key
	// 1 = value
	

	while( true ) {
		// skip whitespace
		while( (*read) && (*read) == ' ' ) {
			read++;
		}

		char term[256];
		int write = 0;

		bool quoted;
		quoted=false;

		// term types:
		// a {quoted} string
		// the character '{'
		// the character '}'

		int termtype = 0;

		if( (*read) == '{' ) {
			read++;
			termtype = 1;
			goto foundterm;
		}
		if( (*read) == '}' ) {
			read++;
			termtype = 2;
			goto foundterm;
		}
		
		while(true) {
			// first letter
			if( (*read) == 0 ) break;

			if( (*read) == '"' ) {
				if( read[1] == '"' ) {
					term[write++] = '"';
					if( write == 256 ) return false;
					
					read+= 2;
					continue;
				}
				quoted = !quoted;
				read++;
				continue;
			}
			
			if( quoted ) {
				term[write++] = *read++;
				if( write == 256 ) return false;
				continue;
			} else {
				if( (*read) == ' ' ) break;
				if( (*read) == 0 ) break;
				
			}
			if( (*read) == ' ' ) break;
			if( (*read) == '{' ) break;
			if( (*read) == '}' ) break;

			term[write++] = *read++;
			if( write == 256 ) return false;
			
		}

foundterm:

		term[write] = 0;


		if( state == 0 ) {
			if(termtype == 1 ) {
				// not expecting opening brace
				return false;
			} else if( termtype == 2 ) {
				// closing brace: terminate current section
				if( !Exit() ) {
					return false; // parsing error (closing brace at top level)
				}
				continue;
			}
			if( Util::StrEmpty(term) ) {
				return true;
			}
			// add new keyvalue
			KeyValue kv;
			Util::CopyString( kv.name, term );
			current_section->values.push_back( kv );
			current_index = current_section->values.size()-1;

			state = 1;
		} else if( state == 1 ) {
			if( termtype == 1 ) {

				Enter( true );
				state = 0;
				continue;
			} else if( termtype == 2 ) {
				// not expecting closing brace
				return false;
			}

			// normal key value
			Util::CopyString( current_section->values[current_index].value, term );
			state = 0;
		}
	}
	
	if( current_level != 0 || state != 0 ) {
		return false;
	}

	Rewind();
	return true;
}

//-------------------------------------------------------------------------------------------------
bool KeyValues::LoadFromFile( const char *file ) {
	if( !ParseKVFile( file ) ) {
		System::Console::Print( "*** Error parsing KeyValues from: %s", file );
		Erase();
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
static bool ValidKey( const char *key ) {
	if( !key ) return false;
	if( key[0] == 0 ) return false;
	return true;
}

//-------------------------------------------------------------------------------------------------
bool KeyValues::KeyJumpCheck( const char *key ) {
	bool badkey = false;

	if( ValidKey( key ) ) {
		if( !JumpToKey( key, false ) ) {
			badkey = true;
			
		} else if( current_section->values[current_index].section ) {
			badkey = true;
		}

	} else {
		if( current_index >= current_section->values.size() ) {
			badkey = true;
		}
		if( current_section->values[current_index].section ) {
			badkey = true;
		}
	}

	return !badkey;
}

//-------------------------------------------------------------------------------------------------
void KeyValues::GetString( const char *key, char *value, int maxlen, const char *defvalue ) {

	if( KeyJumpCheck( key ) ) {
		Util::CopyString( value, maxlen, current_section->values[current_index].value );
	} else {
		Util::CopyString( value, maxlen, defvalue );
	}
}

//-------------------------------------------------------------------------------------------------
int KeyValues::GetNum( const char *key, int defvalue ) {
	if( KeyJumpCheck( key ) ) {
		return Util::StringToInt( current_section->values[current_index].value );
	} else {
		return defvalue;
	}
}

//-------------------------------------------------------------------------------------------------
double KeyValues::GetFloat( const char *key, double defvalue ) {
	if( KeyJumpCheck( key ) ) {
		return Util::StringToFloat( current_section->values[current_index].value );
	} else {
		return defvalue;
	}
}

//-------------------------------------------------------------------------------------------------
void KeyValues::GetColor( const char *key, int &r, int &g, int &b, int &a ) {

	// todo
}

//-------------------------------------------------------------------------------------------------
void KeyValues::GetVector( const char *key, float (&vec)[3] ) {
	// todo
}

//-------------------------------------------------------------------------------------------------
bool KeyValues::JumpToKey( const char *key, bool enter ) {
	for( unsigned int i = current_index; i < current_section->values.size(); i++ ) {
		if( Util::StrEqual( key, current_section->values[i].name ) ) {
			current_index = i;
			return true;
		}
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
bool KeyValues::GotoFirstKey( bool section ) {
	
	if( section ) {
		for( unsigned int i = 0; i < current_section->values.size(); i++ ) {
			if( current_section->values[i].section ) {
				current_index = i;
				return true;
			}
		}
		return false;
	} else {
		if( current_section->values.empty() ) return false;
		current_index = 0;
		return true;
	}
}

//-------------------------------------------------------------------------------------------------
bool KeyValues::GotoNextKey( bool section ) {
	if( section ) {
		for( unsigned int i = current_index+1; i < current_section->values.size(); i++ ) {
			if( current_section->values[i].section ) {
				current_index = i;
				return true;
			}
		}
		return false;
	} else {
		if( current_index+1 >= current_section->values.size() ) return false;
		current_index++;
		return true;
	}
}

//-------------------------------------------------------------------------------------------------
//void KeyValues::SavePosition() {
//	stack_section.push( current_section );
//	stack_index.push( current_index );
//}

//-------------------------------------------------------------------------------------------------
void KeyValues::Rewind() {
	current_section = &root;
	current_index = 0;
	//while( !stack_section.empty() ) stack_section.pop();
	//while( !stack_index.empty() ) stack_index.pop();
}

//-------------------------------------------------------------------------------------------------
void KeyValues::GetKeyName( char *name, int maxlen ) {
	if( current_index >= current_section->values.size() ) {
		name[0] = 0;
		return;
	}
	Util::CopyString( name, maxlen, current_section->values[current_index].name );
}

//-------------------------------------------------------------------------------------------------
void KeyValues::GetSectionName( char *section, int maxlen ) {
	// get the name of the keyvalue that contains this section

	assert( maxlen > 0 );

	if( !current_section->parent ) {
		section[0] = 0;
		return;
	}
	
	Util::CopyString( section, maxlen, current_section->parent->values[current_section->parent_index].name );
}

//-------------------------------------------------------------------------------------------------
void KeyValues::Insert( const char *key, const char *value, bool section, bool enter ) {
	// note: values are not used for sections
	// but the data will still exist in memory

	KeyValue kv;
	Util::CopyString( kv.name, key );
	Util::CopyString( kv.value, value );
	current_section->values.push_back( kv );
	current_index = current_section->values.size() - 1;
	if( section ) {
		// enter will create a new section if it doesn't exist (yes in this case)
		Enter( true );
	}
}

//-------------------------------------------------------------------------------------------------
bool KeyValues::IsSection() {
	if( current_section->values.size() < current_index ) return false;
	return current_section->values[current_index].section != 0;
}

//-------------------------------------------------------------------------------------------------
bool KeyValues::Enter( bool create ) {
	if( current_section->values.size() < current_index ) return false;

	if( !current_section->values[current_index].section ) {
		if( !create ) return false;
		// create new section if it doenst exist
		current_section->values[current_index].section = new Section;
		current_section->values[current_index].section->parent = current_section;
		current_section->values[current_index].section->parent_index = current_index;
	}

	// push position to stack and enter section
	//stack_section.push( current_section );
	//stack_index.push( current_index );
	current_section = current_section->values[current_index].section;
	current_index = 0;
	return true;
}

//-------------------------------------------------------------------------------------------------
bool KeyValues::Exit() {
	if( !current_section->parent ) return false;
	current_index = current_section->parent_index;
	current_section = current_section->parent;
	//current_section = stack_section.top();
	//current_index = stack_index.top();
	//stack_section.pop();
	//stack_index.pop();
	return true;
}

//-------------------------------------------------------------------------------------------------
}
