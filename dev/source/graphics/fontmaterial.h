//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "graphics/graphics.h"
#include "graphics/rasterfont.h"
#include "graphics/forwards.h"

namespace Graphics {


class FontMaterial : public Graphics::Font {

public:
	//Font m_font;
	MaterialPtr m_material; 

	FontMaterial();
	void LoadMaterial();
};

}
