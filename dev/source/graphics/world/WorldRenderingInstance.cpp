//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

namespace Graphics {


WorldRenderingInstance::WorldRenderingInstance() {
	parent = 0;
	prev = next = 0;

}

WorldRenderingInstance::~WorldRenderingInstance() {

}

void WorldRenderingInstance::SetParent( WorldRenderingList *p_parent ) {
	parent = p_parent;
}

WorldRenderingList *WorldRenderingInstance::GetParent() {
	return parent;
}

}

