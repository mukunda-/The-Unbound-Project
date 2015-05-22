//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/stref.h"

namespace System {

/** ---------------------------------------------------------------------------
 * One or more modules implement program behavior.
 */ 
class Module {

public:

	/** -----------------------------------------------------------------------
	 * Higher level modules receive events before lower level modules. They
	 * are also created before and destroyed after.
	 */
	enum class Levels {
		SUPERVISOR = 10, // top level for special needs
		GL         = 15, // graphics layer, needs to be deleted after normal
		                 // modules to keep the context alive.
		SUBSYSTEM  = 20, // interface modules (network,console etc)
		USER       = 30  // program level
	};
	
	//-------------------------------------------------------------------------
	virtual ~Module();

protected:
	/** -----------------------------------------------------------------------
	 * Construct a module.
	 *
	 * @param name  Name of module, must be unique.
	 * @param level System level.
	 */
	Module( const Stref &name, Levels level );

	// non copyable/movable
	Module( Module&  )            = delete;
	Module( Module&& )            = delete;
	Module& operator=( Module&  ) = delete;
	Module& operator=( Module&& ) = delete;

	friend class Main;

	/** -----------------------------------------------------------------------
	 * Called when this module is registered with the system.
	 */
	virtual void OnLoad();

	/** -----------------------------------------------------------------------
	 * Called just before the system starts up.
	 */
	virtual void OnPrepare();
	
	/** -----------------------------------------------------------------------
	 * Called when the system starts up. 
	 */
	virtual void OnStart();

	/** -----------------------------------------------------------------------
	 * Called when the system is shutting down. This module and any other
	 * modules will not be destroyed until all are not busy.
	 */
	virtual void OnShutdown();

	/** -----------------------------------------------------------------------
	 * Called when this module is about to be destroyed.
	 */
	virtual void OnUnload();

	/** -----------------------------------------------------------------------
	 * Set the busy state.
	 *
	 * During shutdown, the system will wait until all modules are `idle`.
	 *
	 * A module initializes in a BUSY state; a module should only engage other
	 * modules while it is in a BUSY state. After setting the busy state to
	 * FALSE, the module should be treated as if it has been destroyed.
	 *
	 * A module MUST unset it's busy state when it has no more work, or when
	 * all references to it are destroyed. If a module is stuck in a busy
	 * state, the system will stall on shutdown.
	 *
	 * Modules can only be woken up by other active modules, or by the system.
	 */
	void SetBusy( bool busy );

	//-------------------------------------------------------------------------
private:

	// program level
	Levels m_level;

	// name of module, ie "net", "db", etc.
	std::string m_name;

	bool m_busy = false;

public:

	Levels             GetLevel()      { return m_level; }
	const std::string &GetName() const { return m_name; }
	bool               Busy()          { return m_busy; }
};

}
