//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

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
		SUBSYSTEM  = 20, // interface modules (network,console etc)
		USER       = 30  // program level
	};

	/** -----------------------------------------------------------------------
	 * This function blocks until the busy state of this module is FALSE
	 */
	virtual void WaitUntilFinished() = 0;
	
	//-------------------------------------------------------------------------
	virtual ~Module();

protected:
	/** -----------------------------------------------------------------------
	 * Construct a module.
	 */
	Module( Levels level );

	// non copyable/movable
	Module( Module&  ) = delete;
	Module( Module&& ) = delete;
	Module& operator=( Module&  ) = delete;
	Module& operator=( Module&& ) = delete;

	friend class Main;

	/** -----------------------------------------------------------------------
	 * Called when this module is registered with the system.
	 */
	virtual void OnLoad();
	
	/** -----------------------------------------------------------------------
	 * Called when the system starts up. (most modules will be loaded)
	 */
	virtual void OnStart();

	/** -----------------------------------------------------------------------
	 * Called when the system is shutting down, this module and any other
	 * modules will not be destroyed until all return not Busy().
	 */
	virtual void OnShutdown();

	/** -----------------------------------------------------------------------
	 * Called when this module is about to be destroyed.
	 */
	virtual void OnUnload();

	/** -----------------------------------------------------------------------
	 * Set the name for this module. The name must be set before the
	 * module is registered. (in the implementation constructor)
	 */
	void SetName( const Stref &name );
	
	/** -----------------------------------------------------------------------
	 * Set the busy state.
	 *
	 * During shutdown, the system will wait until all modules are `idle`.
	 *
	 * With proper behavior, after *all* modules have become idle, they
	 * should not be able to become busy again, as only busy modules should
	 * be able to engage other modules.
	 */
	void SetBusy( bool busy );

	//-------------------------------------------------------------------------
private:

	// program level
	Levels m_level;

	// name of module, ie "net", "db", etc. Must be set by 
	// implementation.
	std::string m_name;
	
	bool m_busy = false;

public:

	Levels GetLevel() { return m_level; }
	const std::string &GetName() const { return m_name; }
	bool Busy() { return m_busy; }
};

}
