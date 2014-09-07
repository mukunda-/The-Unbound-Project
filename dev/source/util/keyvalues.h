//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include <stack>
#include <vector>
#include <assert.h>

#include "util/stringles.h"
//-------------------------------------------------------------------------------------------------
namespace Util {

//-------------------------------------------------------------------------------------------------
class KeyValues {

	class Section;
	 
	//---------------------------------------------------------------------------------------------
	class KeyValue {

	public:
		char name[64];
		char value[256];
		Section *section;

		KeyValue() {
			name[0] = 0;
			value[0] = 0;
			section = 0;
		}
	};

	//---------------------------------------------------------------------------------------------
	class Section {
		
	public:
		Section *parent;
		int parent_index;
		std::vector<KeyValue> values;

		Section() {
			parent = 0;
		}
	};
	
	//---------------------------------------------------------------------------------------------
	Section root;
	Section *current_section;
	unsigned int current_index;
	int current_level;
	//std::stack<Section*> stack_section;
	//std::stack<int> stack_index;

	//---------------------------------------------------------------------------------------------
	// sub function for LoadFromFile
	bool KeyValues::ParseKVFile( const char *file );

	//---------------------------------------------------------------------------------------------
	// Erase subfunction
	void DeleteSection( Section *section );

	bool KeyJumpCheck( const char *key );

//-------------------------------------------------------------------------------------------------
public:

	KeyValues();
	void Erase();

	//---------------------------------------------------------------------------------------------
	// load a key values file
	// 
	// this will append the contents of the file if 
	bool LoadFromFile( const char *file );
	
	//---------------------------------------------------------------------------------------------
	// for the following functions:
	// if 'key' is "" then it will read from the currently selected key
	// otherwise the position will be moved to that key
	//---------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------
	// get key value as a string
	//
	void GetString( const char *key, char *value, int maxlen, const char *defvalue = "" );

	//---------------------------------------------------------------------------------------------
	// get key value as number
	//
	int GetNum( const char *key, int defvalue=0 );

	//---------------------------------------------------------------------------------------------
	// get key value as float
	//
	double GetFloat( const char *key, double defvalue=0.0 );

	//---------------------------------------------------------------------------------------------
	// get key value as color
	//
	void GetColor( const char *key, int &r, int &g, int &b, int &a );

	//---------------------------------------------------------------------------------------------
	// get key value as vector
	// formatted like this: "1513.15 165.1584 16843.25"
	//
	void GetVector( const char *key, float (&vec)[3] );

	//---------------------------------------------------------------------------------------------
	// jump to a certain key
	// if 'enter' is true, then Enter() will be called too
	//
	// note: will not jump to previous keys from the selected one (use GotoFirstKey first if desired)
	//
	bool JumpToKey( const char *key, bool enter=true );

	//---------------------------------------------------------------------------------------------
	// go to the first key in this section
	// if 'section' is true then it will goto the first key that contains a sub section
	//
	bool GotoFirstKey( bool section=true );

	//---------------------------------------------------------------------------------------------
	// go to the next key in the section
	//
	// if 'section' is true then it will skip keys that are values and will stop at the next section key
	//
	bool GotoNextKey( bool section=true );

	//---------------------------------------------------------------------------------------------
	// returns true if the selected key contains a section
	//
	bool IsSection();
	
	//---------------------------------------------------------------------------------------------
	// enter the currently selected key for reading/browsing
	//
	// returns false if the current key is not a section
	//
	// if 'create' is true, this will always succeed and will create a sub section for normal
	// keys
	//
	bool Enter( bool create = false );

	//---------------------------------------------------------------------------------------------
	// exit the current section returning to the parent key
	//
	bool Exit(); 

	//---------------------------------------------------------------------------------------------
	// return to the root node
	//
	void Rewind();

	//---------------------------------------------------------------------------------------------
	// get the name of the selected key
	// 
	// returns "" if no key is selected (if no keys are in a section)
	//
	void GetKeyName( char *name, int maxlen );
	
	//---------------------------------------------------------------------------------------------
	// get the name of the section that contains the selected key
	// returns "" if in the root node
	//
	void GetSectionName( char *section, int maxlen );
	
	//---------------------------------------------------------------------------------------------
	// insert a new key into the current section
	//
	// if 'section' is true then the key will contain a subsection and 'value' is ignored
	// 
	// the new key will be selected as current
	// if 'enter' is true and it is a section key, Enter() will be called
	//
	void Insert( const char *key, const char *value, bool section=false, bool enter=true );
};

//-------------------------------------------------------------------------------------------------
}

