//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

// doubly linked list template
//

#pragma once

//-----------------------------------------------------------------------------
namespace Util {
	 
//-----------------------------------------------------------------------------
template< typename T >
class LinkedItem {

public:
	T *m_prev;
	T *m_next;

#ifdef _DEBUG
	void *debug_parent = nullptr; // the list this item is in
#endif

protected:

	// cannot delete linked item directly.
	~LinkedItem() {}
};

//-----------------------------------------------------------------------------
template< typename T >
class LinkedList {
//-----------------------------------------------------------------------------

protected:
	T *m_first = nullptr;
	T *m_last  = nullptr;

public:
	//-------------------------------------------------------------------------
	LinkedList() {}

	//-------------------------------------------------------------------------
	// noncopyable
	LinkedList( LinkedList & ) = delete;
	LinkedList &operator=( LinkedList & ) = delete;

	//-------------------------------------------------------------------------
	// movable
	LinkedList( LinkedList &&other ) {
		Clear();
		AppendList( other );
	}

	LinkedList &operator=( LinkedList &&other ) {
		Clear();
		AppendList( other );
		
		return *this;
	}

	//-------------------------------------------------------------------------
	virtual ~LinkedList() {

		// make sure list is empty.
		assert( !first );
		assert( !last );
	}
	
	/** -----------------------------------------------------------------------
	 * Add an item to this list.
	 *
	 * @param item Item to add.
	 */
	void Push( T &item ) { 

#       ifdef _DEBUG
			assert( i.debug_parent == nullptr );
			i.debug_parent = (void*)this;
#       endif 

		item.m_next = nullptr;
		
		if( !m_first ) {

			m_first = &item;
			m_last  = &item; 
			item->m_prev = nullptr;

		} else {

			m_last->m_next = &item;
			item.m_prev    = m_last;
			m_last         = &item;
		}

	}
	
	/** -----------------------------------------------------------------------
	 * Append another list to this one.
	 *
	 * @param first The first item in the other list.
	 * @param last  The last item in the other list.
	 */
	void AppendList( T *first, T *last ) {
		if( !first ) return; // other list is empty

		assert( last );

		if( m_last ) {
			m_last->m_next = first;
			first->m_prev  = m_last;
			m_last = last;
		} else {
			m_first = first;
			m_last  = last;
		}
	}

	/** -----------------------------------------------------------------------
	 * Append another list to this one.
	 *
	 * @param list Other list to append to this one, it will be empty after
	 *             this operation.
	 */
	void AppendList( LinkedList &list ) {
		AppendList( list.first, list.last );

		list.m_first = nullptr;
		list.m_last  = nullptr;
	}
	
	/** -----------------------------------------------------------------------
	 * Remove an item from this list.
	 *
	 * @param item Pointer to item to remove. 
	 */
	void Pull( T &item ) { 
		
#       ifdef _DEBUG
			assert( item.debug_parent == (void*)this );
			item.debug_parent = nullptr;
#       endif 
		
		if( item.m_next ) {
			item.m_next->m_prev = i.m_prev;
		} else {
			m_last = item.m_prev;
		}

		if( item.m_prev ) {
			item.m_prev->m_next = i.m_next;
		} else {
			
			m_first = item.m_next;
		} 
	}
	
	/** -----------------------------------------------------------------------
	 * Clear the list.
	 *
	 * All items are assumed to be unlinked after.
	 */
	void Clear() {

#       ifdef _DEBUG
			// unlink items
			for( T *i = m_first; i; i = i->m_next ) {
				i->debug_parent = nullptr;
			}
#       endif 

		m_first = nullptr;
		m_last  = nullptr;
	}
	
	void operator +=( T &a          ) { Push( a ); }
	void operator +=( LinkedList &a ) { Push( a ); }
	void operator -=( T &a          ) { Pull( a ); } 

	LinkedList &operator +( T &a          ) { Push( a ); return *this; }
	LinkedList &operator +( LinkedList &a ) { Push( a ); return *this; }
	LinkedList &operator -( T &a          ) { Pull( a ); return *this; }
	
	/** -----------------------------------------------------------------------
	 * Returns first item in the list, or nullptr if the list is empty.
	 */
	      T *First()       { return m_first; }
	const T *First() const { return m_first; }
	
	/** -----------------------------------------------------------------------
	 * Returns last item in list, or nullptr if the list is empty.
	 */
	      T *Last()       { return m_last; }
	const T *Last() const { return m_last; }
};

}
