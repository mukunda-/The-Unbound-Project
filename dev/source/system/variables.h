//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include "util/stringles.h"
#include "system/console.h"

namespace System { 

#define PRINT_VARIABLE_WARNINGS

//---------------------------------------------------------------------------------------
class Variable {

public:

	enum Type {
		T_INT,
		T_FLOAT,
		T_STRING
	};

	typedef void (*CallbackT)( Variable &var );

private:
	
	struct Value {
		// only one of these are used.
		std::string m_string;
		int m_int;
		double m_float;

		Value() {
			m_int = 0;
			m_float = 0.0;
		}
	};


	char m_name[32];
	std::string m_description;
	
	Value m_value;
	Value m_previous_value;

	std::vector<CallbackT> m_change_handlers;

	int m_flags;

	//---------------------------------------------------------------------------------------
	Type m_type;

	//---------------------------------------------------------------------------------------
	enum {
		FCVAR_GLOBAL=1
	};
	static const int a = 5;
	//---------------------------------------------------------------------------------------
	static const char *TYPENAMES[];

	//---------------------------------------------------------------------------------------
	int IGetInt( Value &from, bool warn ) {
		if( m_type == T_INT ) {
			return from.m_int;
		}

		if( warn ) {
			System::Console::Print( "Warning: Reading INT from %s variable \"%s\"", TYPENAMES[m_type], m_name );
		}

		if( m_type == T_FLOAT ) {
			return (int)floor( from.m_float );
		} else if( m_type == T_STRING ) {
			return std::atoi( from.m_string.c_str() );
		}

		assert( !"Unknown m_type in variable!" );
		return 0;
	}

	//---------------------------------------------------------------------------------------
	double IGetFloat( Value &from, bool warn ) {
		if( m_type == T_FLOAT ) {
			return from.m_float;
		}

		if( warn ) {
			System::Console::Print( "Warning: Reading FLOAT from %s variable \"%s\"", TYPENAMES[m_type], m_name );
		}

		if( m_type == T_INT ) {
			return (double)from.m_int;
		} else if( m_type == T_STRING ) {
			return std::atof( from.m_string.c_str() );
		}

		assert( !"Unknown m_type in variable!" );
		return 0.0; // shouldn't happen
	}

	//---------------------------------------------------------------------------------------
	std::string IGetString( Value &from, bool warn ) {
		if( m_type == T_STRING ) {
			return from.m_string;
		}

		if( warn ) {
			System::Console::Print( "Warning: Reading STRING from %s variable \"%s\"", TYPENAMES[m_type], m_name );
		}

		if( m_type == T_INT ) {
			return std::to_string( from.m_int );
		} else if( m_type == T_FLOAT ) {
			return std::to_string( from.m_float );
		}

		assert( !"Unknown m_type in variable!" );
		return ""; // shouldn't happen
	}

	//---------------------------------------------------------------------------------------
	Variable( const char *name, Type type, const char *description = nullptr, int flags = 0 ) {
		// constructed only by the Create function.

		Util::CopyString( m_name, name );
		m_name[31] = 0;
		assert( m_name[0] != 0 );
		if( description ) {
			m_description = description;
		}
		m_type = type;
		m_flags = flags;
	}

	Variable( Variable& ) {
		// non-copyable
	}

public:
	

	//---------------------------------------------------------------------------------------
	int GetInt( bool warn = true ) {
		return IGetInt( m_value, warn );
	}

	//---------------------------------------------------------------------------------------
	double GetFloat( bool warn = true ) {
		return IGetFloat( m_value, warn );
	}

	//---------------------------------------------------------------------------------------
	std::string GetString( bool warn = true ) {
		return IGetString( m_value, warn );
	}

	//---------------------------------------------------------------------------------------
	int GetPreviousInt( bool warn = true ) {
		return IGetInt( m_previous_value, warn );
	}

	//---------------------------------------------------------------------------------------
	double GetPreviousFloat( bool warn = true ) {
		return IGetFloat( m_previous_value, warn );
	}

	//---------------------------------------------------------------------------------------
	std::string GetPreviousString( bool warn = true ) {
		return IGetString( m_previous_value, warn );
	}

	//---------------------------------------------------------------------------------------
	void SetInt( int value, bool warn = true ) {
		if( m_type == T_INT ) {
			if( m_value.m_int != value ) {

				m_previous_value.m_int = m_value.m_int;
				m_value.m_int = value;
				OnChange();
			}
			return;
		}

		if( warn ) {
			System::Console::Print( "Warning: Writing INT to %s variable \"%s\"", TYPENAMES[m_type], m_name );
		}

		if( m_type == T_FLOAT ) {
			SetFloat( (double)value );
		} else if( m_type == T_STRING ) {
			SetString( std::to_string(value).c_str() );
		}
	}

	//---------------------------------------------------------------------------------------
	void SetFloat( double value, bool warn = true ) {
		if( m_type == T_FLOAT ) {
			if( m_value.m_float != value ) {

				m_previous_value.m_float = m_value.m_float;
				m_value.m_float = value;
				OnChange();
			}
			return;
		}

		if( warn ) {
			System::Console::Print( "Warning: Writing FLOAT to %s variable \"%s\"", TYPENAMES[m_type], m_name );
		}

		if( m_type == T_INT ) {
			SetInt( (int)floor(value) );
		} else if( m_type == T_STRING ) {
			SetString( std::to_string(value).c_str() );
		}
	}

	//---------------------------------------------------------------------------------------
	void SetString( const char *value, bool warn = true ) {
		if( m_type == T_STRING ) {
			if( m_value.m_string != value ) {

				m_previous_value.m_string = m_value.m_string;
				m_value.m_string = value;
				OnChange();
			}
			return;
		}

		if( warn ) {
			System::Console::Print( "Warning: Writing STRING to %s variable \"%s\"", TYPENAMES[m_type], m_name );
		}

		if( m_type == T_INT ) {
			SetInt( std::atoi(value) );
		} else if( m_type == T_FLOAT ) {
			SetFloat( std::atof(value) );
		}
	}

	//---------------------------------------------------------------------------------------
	void OnChange() {
		for( auto handler : m_change_handlers ) {
			handler( *this );
		}
	}

	//---------------------------------------------------------------------------------------
	void HookChange( CallbackT callback ) {
		m_change_handlers.push_back( callback );
	}

	//---------------------------------------------------------------------------------------
	void UnhookChange( CallbackT callback ) {
		for( auto handler = m_change_handlers.begin(); handler != m_change_handlers.end(); handler++ ) {
			if( (*handler) == callback ) {
				m_change_handlers.erase( handler );
				return;
			}
		}
		throw std::runtime_error( "Tried to unhook nonexistant sysvar change handler." );
	}

	std::string Name() {
		return m_name;
	}

	void PrintInfo();

	//---------------------------------------------------------------------------------------
	// create a new sysvar
	// 
	// checks for existing variable first and simply returns it if so.
	// if it doesnt exist, a new variable is created and initialized with the arguments given.
	//
	static Variable &Create( const char *name, Variable::Type type, 
							const char *default_value, const char *description = nullptr, int flags = 0 );
	
	//---------------------------------------------------------------------------------------
	// find an existing sysvar
	//
	// returns nullptr if not found
	//
	static Variable *Find( const char *name );

};

 

}
