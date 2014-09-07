//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include <stdafx.h>

//-------------------------------------------------------------------------------------------------
namespace Graphics {

//-------------------------------------------------------------------------------------------------
void WorldRenderingList::Erase() {
	list.Erase();
}

//-------------------------------------------------------------------------------------------------
void WorldRenderingList::Add( WorldRenderingInstance *i ) {
	list.Add(i);
	i->SetParent(this);

}

//-------------------------------------------------------------------------------------------------
WorldRenderingInstance *WorldRenderingList::Remove( WorldRenderingInstance *i ) {
	list.Remove(i);
	i->SetParent(this);
	return i;
}

//-------------------------------------------------------------------------------------------------
WorldRenderingInstance *WorldRenderingList::GetFirst() {
	return list.GetFirst();
}

//-------------------------------------------------------------------------------------------------
WorldRenderingInstance *WorldRenderingList::GetLast() {
	return list.GetLast();
}

//-------------------------------------------------------------------------------------------------
WorldRenderingInstance *WorldRenderingList::PopFirst() {
	return Remove(GetFirst());
}

//-------------------------------------------------------------------------------------------------
bool WorldRenderingList::IsEmpty() const {
	return list.GetFirstC() == 0;
}

}
