//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#pragma once

// linked list for single threaded operations
//

//-------------------------------------------------------------------------------------------------
#ifndef AUDIO_LIST_H
#define AUDIO_LIST_H

//-------------------------------------------------------------------------------------------------
namespace Audio {

//-------------------------------------------------------------------------------------------------
template<class object> class AudioList {
	
private:
	object *first;
	object *last;

public:
	void Add( object *obj ) {
		obj->next = 0;
		if( !first ) {
			first=last=obj;
			obj->prev=0;
		} else {
			last->next = obj;
			obj->prev = last;
			last = last->next;
		}

	}

	void AppendList( object *p_first, object *p_last ) {
		if( !p_first || !p_last ) return;
		if( last ) {
			last->next = p_first;
			p_first->prev = last;
			last = p_last;
		} else {
			first=p_first;
			last=p_last;
		}
	}

	void Remove( object *obj ) {
		
		if( obj == first ) {
			if( obj == last ) {
				first = last = 0;

			} else {
				first = first->next;
				first->prev = 0;
			}
		} else if( obj == last ) {
			last = last->prev;
			last->next = 0;
		} else {
			obj->prev->next = obj->next;
			obj->next->prev = obj->prev;
		}
		obj->prev = obj->next = 0;
	}

	object *GetFirst() {
		return first;
	}

	object *GetLast() {
		return last;
	}
};

//-------------------------------------------------------------------------------------------------

}

#endif
