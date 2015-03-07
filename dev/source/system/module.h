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
		SUPERVISOR, // top level for special needs
		SUBSYSTEM,  // interface modules (network,console etc)
		USER        // program level
	};

	/** -----------------------------------------------------------------------
	 * This function returns true if the module is busy doing something.
	 *
	 * During a shutdown, all modules must return FALSE for this function
	 * for the shutdown to complete.
	 *
	 * All modules are kept alive during the shutdown until they all return
	 * FALSE.
	 */
	virtual bool Busy() const = 0;
	
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

	/** -----------------------------------------------------------------------
	 * Called when this module is registered with the system.
	 */
	virtual void OnLoad() {}
	
	/** -----------------------------------------------------------------------
	 * Called when the system starts up. (most modules will be loaded)
	 */
	virtual void OnStart() {}

	/** -----------------------------------------------------------------------
	 * Called when the system is shutting down, this module and any other
	 * modules will not be destroyed until all return not Busy().
	 */
	virtual void OnShutdown() {}

	/** -----------------------------------------------------------------------
	 * Called when this module is about to be destroyed.
	 */
	virtual void OnUnload() {}

	/** -----------------------------------------------------------------------
	 * Set/get the name for this module. The name must be set before the
	 * module is registered. (in the implementation constructor)
	 */
	void SetName( const Stref &name );
	const std::string &GetName() const;

	//-------------------------------------------------------------------------
private:

	// program level
	Levels m_level;

	// name of module, ie "net", "db", etc. Must be set by 
	// implementation.
	std::string m_name;
};

}
