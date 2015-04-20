//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

// single-threaded double-linked-list template
//

#pragma once

namespace Util {
	 
template<class Object>
class LinkedItem {

public:
	Object *m_prev;
	Object *m_next;

	LinkedItem() {
		m_prev = m_next = nullptr;
	}

	virtual ~LinkedItem() {}
};

//-------------------------------------------------------------------------------------------------
template<class Object>
class LinkedList final {
//-------------------------------------------------------------------------------------------------

protected:
	Object *first;
	Object *last;

public:
	//---------------------------------------------------------------------------------------------
	LinkedList() {
		Erase();
	}

	virtual ~LinkedList() {}
	
	//---------------------------------------------------------------------------------------------
	// add new item
	//
	void Add( Object *obj ) {
		obj->m_next = 0;
		if( !first ) {
			first=last=obj;
			obj->m_prev=0;
		} else {
			last->m_next = obj;
			obj->m_prev = last;
			last = last->m_next;
		}

	}
	
	//---------------------------------------------------------------------------------------------
	// append a linked list of the same type
	//
	void AppendList( Object *p_first, Object *p_last ) {
		if( !p_first || !p_last ) return;
		if( last ) {
			last->m_next = p_first;
			p_first->m_prev = last;
			last = p_last;
		} else {
			first=p_first;
			last=p_last;
		}
	}

	//---------------------------------------------------------------------------------------------
	void AppendList( LinkedList<Object> list ) {
		AppendList( list.first, list.last );
	}

	
	//---------------------------------------------------------------------------------------------
	// remove an item
	void Remove( Object *obj ) {
		
		if( obj == first ) {
			if( obj == last ) {
				first = last = 0;

			} else {
				first = first->m_next;
				first->m_prev = 0;
			}
		} else if( obj == last ) {
			last = last->m_prev;
			last->m_next = 0;
		} else {
			obj->m_prev->m_next = obj->m_next;
			obj->m_next->m_prev = obj->m_prev;
		}
		obj->m_prev = obj->m_next = 0;
	}

	//---------------------------------------------------------------------------------------------
	// clear the list
	//
	void Erase() {
		first = 0;
		last  = 0;
	}

	//---------------------------------------------------------------------------------------------
	// return first item
	//
	Object *GetFirst() {
		return first;
	}

	//---------------------------------------------------------------------------------------------
	const Object *GetFirstC() const {
		return first;
	}
	
	//---------------------------------------------------------------------------------------------
	// return last item
	//
	Object *GetLast() {
		return last;
	}
};

}
