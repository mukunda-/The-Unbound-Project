//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Util {

	/// -----------------------------------------------------------------------
	/// a simple class to measure the rough execution time for code
	///
	class CodeTimer {
	
		clock_t m_begin;

	public:
		CodeTimer() {
			m_begin = clock();
		}

		/// -------------------------------------------------------------------
		/// Reset the timer.
		///
		void Reset() {
			m_begin = clock();
		}
		
		/// -------------------------------------------------------------------
		/// @returns the current running time.
		///
		double Duration() {
			return (double)(clock() - m_begin) / (double)CLOCKS_PER_SEC;
		}
		
		/// -------------------------------------------------------------------
		/// @returns the starting time of the timer.
		///
		double Start() {
			return (double)(m_begin) / (double)CLOCKS_PER_SEC;
		}

		/// -------------------------------------------------------------------
		/// @returns the current time.
		///
		double Now() {
			return (double)(clock()) / (double)CLOCKS_PER_SEC;
		}
	};

}
