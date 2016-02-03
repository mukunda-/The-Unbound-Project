//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "ui/region.h"

//-----------------------------------------------------------------------------
namespace Ui {

class ColoredRegion : public Region {
	friend class Instance;

protected:
	void Draw( Graphics::Builder& ) override;

	int m_r;
	int m_g;
	int m_b;
	int m_a;

public:
	
	ColoredRegion( const Stref &name );

	void SetColor( int r, int g, int b, int a );
};

}
