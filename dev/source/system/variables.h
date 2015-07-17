//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include "util/stringles.h"
#include "system/console.h"
#include "console/console.h"

namespace Tests { class SystemTests; }

namespace System {  

/// ---------------------------------------------------------------------------
/// Base for a class that holds a system variable.
///
class Variable {

public:
	using ptr = std::unique_ptr<Variable>;
	using ChangeHandler = std::function<void(Variable&)>;

	//-------------------------------------------------------------------------
	enum {
		
	};

	//-------------------------------------------------------------------------
	struct Value {
		int m_int = 0;
		double m_float = 0.0;
		std::string m_string;

		bool operator ==(Value &other) {
			return
				m_int == other.m_int && 
				m_float == other.m_float &&
				m_string == other.m_string;
		}
	};
	
	Value m_value; // current value
	Value m_prev;  // previous value
	
	/** -----------------------------------------------------------------------
	 * Read the value of this variable.
	 */ 
	int                GetInt() const     { return m_value.m_int;            }
	double             GetFloat() const   { return m_value.m_float;          }
	const std::string &GetString() const  { return m_value.m_string;         }
	const char        *GetCString() const { return m_value.m_string.c_str(); }

	/** -----------------------------------------------------------------------
	 * Get the previously set value for this variable.
	 *
	 * Ideally used when handling a value change and comparing with the
	 * new value.
	 */
	int                PreviousInt() const    { return m_prev.m_int;    }
	double             PreviousFloat() const  { return m_prev.m_float;  }
	const std::string &PreviousString() const { return m_prev.m_string; }

	/** -----------------------------------------------------------------------
	 * Set the value of this variable.
	 *
	 * @param value Value to set.
	 *
	 * @returns true if the value of the variable was changed.
	 */
	bool SetInt( int value );
	bool SetFloat( double value );
	bool SetString( const Util::StringRef &value );
	
	/* -----------------------------------------------------------------------
	 * Add an "on-change" handler.
	 *
	 * When this variable is changed, your callback will be triggered.
	 *
	 * @param callback Callback function to add to the on-changed 
	 *                 handler list.
	 * @returns Handle for unhooking later.
	 */
	int HookChange( ChangeHandler callback );

	/** -----------------------------------------------------------------------
	 * Remove an "on-change" handler.
	 *
	 * This cannot be used during an on-changed callback.
	 *
	 * @param callback ID that was returned from HookChange.
	 */
	void UnhookChange( int id );
	
	/// -----------------------------------------------------------------------
	/// Return the name of this variable.
	///
	std::string Name() { return m_name; }
	std::string Description() { return m_description; }
	
	/// -----------------------------------------------------------------------
	/// Print the name and description of this variable to the console.
	///
	void PrintInfo();

	//-------------------------------------------------------------------------
	virtual ~Variable();

protected:
	friend class Main;
	Variable( const Util::StringRef &name, 
			  const Util::StringRef &description,
			  const Util::StringRef &init,
			  int flags );


	Variable( Variable&  ) = delete;
	Variable( Variable&& ) = delete;
	Variable& operator=( Variable&  ) = delete;
	Variable& operator=( Variable&& ) = delete;

	//-------------------------------------------------------------------------
private:
	
	bool OnChanged();
	bool SetStringI( const Util::StringRef &value, bool skipchg );

	// registered callbacks for when the value of this variable changes.
	std::vector<std::pair<int, ChangeHandler>> m_change_handlers;

	// FLAG_* bits
	int m_flags;

	// the name used to reference this variable
	std::string m_name;

	// brief description of this variable
	std::string m_description; 

	int m_changehandler_nextid = 0;

	friend class ::Tests::SystemTests;
};
  
namespace Variables {

	/** -----------------------------------------------------------------------
	 * Create a new system variable.
	 * 
	 * This checks for an existing variable first and simply returns it if so.
	 * If it doesnt exist, a new variable is created and initialized with the 
	 *  arguments given.
	 * 
	 *  @param name Name of variable to create. The name is used to reference
	 *              the variable later on. 
	 * 
	 *  @param default_value Value to initialize the variable with. Only used
	 *                       when a new variable is created.
	 * 
	 *  @param description Description to assign to the variable. Only used
	 *                     when a new variable is created.
	 * 
	 *  @param flags Creation flags (reserved for later use)
	 * 
	 *  @return Existing Variable or newly created Variable.
	 */
	Variable &Create( const Util::StringRef &name, 
					  const Util::StringRef &default_value = "",
					  const Util::StringRef &description = "",
					  int flags = 0 );

	/// -----------------------------------------------------------------------
	/// Delete a variable.
	///
	/// @param name Name of variable to delete
	/// @returns true if a variable was deleted. false if the name was not
	///          registered.
	///
	bool Delete( const Util::StringRef &name );

	/// -----------------------------------------------------------------------
	/// Find a variable.
	///
	/// @param name Name of variable
	/// @returns Variable or nullptr if none exists.
	///
	Variable *Find( const Util::StringRef &name );
}

}

/* old version, ill just leave this here for a while.
//---------------------------------------------------------------------------------------
class Variable {

public:

	enum Type {
		T_INT,   // Integer variable
		T_FLOAT, // Floating point variable
		T_STRING // String variable
	};

	// callback when a variable event occurs
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

	// list of handlers to call when the value changes.
	std::vector<CallbackT> m_change_handlers;

	int m_flags;

	//---------------------------------------------------------------------------------------
	Type m_type;

	//---------------------------------------------------------------------------------------
	enum {
		FCVAR_GLOBAL=1
	};
 
	//---------------------------------------------------------------------------------------
	static const char *TYPENAMES[];

	//---------------------------------------------------------------------------------------
	int IGetInt( Value &from, bool warn ) {
		if( m_type == T_INT ) {
			return from.m_int;
		}

		if( warn ) {
			::Console::Print( "Warning: Reading INT from %s variable \"%s\"", TYPENAMES[m_type], m_name );
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
			::Console::Print( "Warning: Reading FLOAT from %s variable \"%s\"", TYPENAMES[m_type], m_name );
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
			::Console::Print( "Warning: Reading STRING from %s variable \"%s\"", TYPENAMES[m_type], m_name );
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
	
	// ------------------------------------------------------------------------
	// READING FUNCTIONS
	//
	// GetX, X = type of variable to read. If the base type of this variable
	// is not the type requested, it will try to convert the value into the
	// type requested.
	//
	// If 'warn' is set to true, a warning will be printed to the console if
	// the base type does not match the type requested.
	//
	// If the conversion fails, a zero value will be returned.
	// ------------------------------------------------------------------------

	/// -----------------------------------------------------------------------
	/// Read an integer.
	///
	/// See READING FUNCTIONS
	///
	int GetInt( bool warn = true ) {
		return IGetInt( m_value, warn );
	}
	
	/// -----------------------------------------------------------------------
	/// Read a floating point number.
	///
	/// See READING FUNCTIONS
	///
	double GetFloat( bool warn = true ) {
		return IGetFloat( m_value, warn );
	}
	
	/// -----------------------------------------------------------------------
	/// Read a string.
	///
	/// See READING FUNCTIONS
	///
	std::string GetString( bool warn = true ) {
		return IGetString( m_value, warn );
	}

	// ------------------------------------------------------------------------
	// PREVIOUS READING FUNCTIONS
	//
	// These functions read the last value that was assigned to this
	// variable.
	//
	// Ideally used when handling a value change and comparing with the
	// new value.
	//
	// See READING FUNCTIONS for behavior.
	// ------------------------------------------------------------------------
	
	/// -----------------------------------------------------------------------
	/// Read the previous integer.
	///
	/// See PREVIOUS READING FUNCTIONS
	///
	int GetPreviousInt( bool warn = true ) {
		return IGetInt( m_previous_value, warn );
	}
	
	/// -----------------------------------------------------------------------
	/// Read the previous floating point number.
	///
	/// See PREVIOUS READING FUNCTIONS
	///
	double GetPreviousFloat( bool warn = true ) {
		return IGetFloat( m_previous_value, warn );
	}
	
	/// -----------------------------------------------------------------------
	/// Read the previous string.
	///
	/// See PREVIOUS READING FUNCTIONS
	///
	std::string GetPreviousString( bool warn = true ) {
		return IGetString( m_previous_value, warn );
	}

	// ------------------------------------------------------------------------
	// WRITING FUNCTIONS
	// 
	// SetX, X = type of variable to write. If the base type of this variable
	// is not the type being set, it will try to convert the value given into 
	// the base type of this variable, and assign it to that.
	//
	// If 'warn' is set to true, a warning will be printed to the console if
	// the base type does not match the type being set.
	//
	// If the conversion fails, a zero value will be set.
	// ------------------------------------------------------------------------
	
	/// -----------------------------------------------------------------------
	/// Set an integer value.
	///
	/// See WRITING FUNCTIONS
	///
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
			::Console::Print( "Warning: Writing INT to %s variable \"%s\"", TYPENAMES[m_type], m_name );
		}

		if( m_type == T_FLOAT ) {
			SetFloat( (double)value );
		} else if( m_type == T_STRING ) {
			SetString( std::to_string(value).c_str() );
		}
	}
	
	/// -----------------------------------------------------------------------
	/// Set a floating point value.
	///
	/// See WRITING FUNCTIONS
	///
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
			::Console::Print( "Warning: Writing FLOAT to %s variable \"%s\"", TYPENAMES[m_type], m_name );
		}

		if( m_type == T_INT ) {
			SetInt( (int)floor(value) );
		} else if( m_type == T_STRING ) {
			SetString( std::to_string(value).c_str() );
		}
	}
	
	/// -----------------------------------------------------------------------
	/// Set a string.
	///
	/// See WRITING FUNCTIONS
	///
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
			::Console::Print( "Warning: Writing STRING to %s variable \"%s\"", TYPENAMES[m_type], m_name );
		}

		if( m_type == T_INT ) {
			SetInt( std::atoi(value) );
		} else if( m_type == T_FLOAT ) {
			SetFloat( std::atof(value) );
		}
	}
	
	/// -----------------------------------------------------------------------
	/// Called when the variable is changed by the system.
	///
	/// This executes the "on-changed" handler sequence.
	///
	void OnChange() {
		for( auto handler : m_change_handlers ) {
			handler( *this );
		}
	}

	/// -----------------------------------------------------------------------
	/// Add an "on-change" handler.
	///
	/// When this variable is changed, your callback will be triggered.
	///
	/// \param callback Callback function to add to the on-changed 
	///                 handler list
	///
	void HookChange( CallbackT callback ) {
		m_change_handlers.push_back( callback );
	}

	/// -----------------------------------------------------------------------
	/// Remove an "on-change" handler.
	///
	/// \param callback Callback that was registered with HookChange
	///
	void UnhookChange( CallbackT callback ) {
		for( auto handler = m_change_handlers.begin(); handler != m_change_handlers.end(); handler++ ) {
			if( (*handler) == callback ) {
				m_change_handlers.erase( handler );
				return;
			}
		}
		throw std::runtime_error( "Tried to unhook nonexistant sysvar change handler." );
	}
	
	/// -----------------------------------------------------------------------
	/// Return the name of this variable.
	///
	std::string Name() {
		return m_name;
	}
	
	/// -----------------------------------------------------------------------
	/// Print the name, type and description of this variable to the console.
	///
	void PrintInfo();

	static Variable &Create( const char *name, Variable::Type type, 
							const char *default_value, 
							const char *description = nullptr, 
							int flags = 0 );
	
	/// -----------------------------------------------------------------------
	/// Find an existing sysvar
	///
	/// \param name Name to search for.
	/// \return System Variable matching the name, or nullptr if not found.
	///
	static Variable *Find( const char *name );

};

 */
