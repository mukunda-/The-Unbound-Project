//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2015, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include "util/stringles.h"
#include "system/console.h"
#include "console/console.h"

namespace Tests { class SystemTests; }

namespace System {  

/** ---------------------------------------------------------------------------
 * Base for a class that holds a system variable.
 */
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
	
	/** -----------------------------------------------------------------------
	 * Return the name of this variable.
	 */
	std::string Name() { return m_name; }
	std::string Description() { return m_description; }
	
	/** -----------------------------------------------------------------------
	 * Print the name and description of this variable to the console.
	 */
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

	/** -----------------------------------------------------------------------
	 * Delete a variable.
	 *
	 * @param name Name of variable to delete
	 * @returns true if a variable was deleted. false if the name was not
	 *          registered.
	 */
	bool Delete( const Util::StringRef &name );

	/** -----------------------------------------------------------------------
	 * Find a variable.
	 *
	 * @param name Name of variable
	 * @returns Variable or nullptr if none exists.
	 */
	Variable *Find( const Util::StringRef &name );
}

}
