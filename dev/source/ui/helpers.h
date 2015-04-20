//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Ui {

	namespace {

		//---------------------------------------------------------------------
		static inline bool PointInsideRect( const Eigen::Vector2i &position, 
											const Eigen::Vector4i &rect ) {
			return position[0] >= rect[0] && 
				   position[0] < (rect[0] + rect[2]) && 
				   position[1] >= rect[1] && 
				   position[1] < (rect[1] + rect[3]);
		}
	}
}

