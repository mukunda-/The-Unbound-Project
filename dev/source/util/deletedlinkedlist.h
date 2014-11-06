//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

// linkedlist that deletes its items upon destruction.
//

#pragma once

#include "util/linkedlist.h"

namespace Util {

template<class Object>
class DeletedLinkedList : public LinkedList<Object> {
public:
	~DeletedLinkedList() {
		Object *next = nullptr;

		for( Object *obj = first; obj; obj = next ) {
			next = obj->next;
			delete obj;
		}
	}
}

}