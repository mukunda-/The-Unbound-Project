//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2015, Mukunda Johnson, All rights reserved. =========//

#include "ui/region.h"

//-----------------------------------------------------------------------------
namespace Ui {

class ColoredRegion : public Region {
	friend class Instance;

protected:
	void Draw() override;

public:
	
	ColoredRegion( const Stref &name );
};

}
