//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

// Unit testing interface

//-----------------------------------------------------------------------------
namespace Tests {

	/// -----------------------------------------------------------------------
	/// Test case 
	///
	class Test {
		// name of test/description
		std::string m_name;

	public:
		/// -------------------------------------------------------------------
		/// Run this test. Prints output to console.
		///
		bool Run();

	protected:
		
		/// -------------------------------------------------------------------
		/// Create a test case. 
		///
		Test();

		/// -------------------------------------------------------------------
		/// Function for test actions.
		///
		/// @returns true if the test passed.
		///
		virtual bool Execute() = 0;

		/// -------------------------------------------------------------------
		/// Returns the name of this test.
		///
		virtual std::string Name() { return "Unnamed test"; }
	};
}
