//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once
  
namespace Util {

template <typename, int> class SharedList;

/** ---------------------------------------------------------------------------
 * Interface for objects to be contained in a SharedList.
 *
 * @param T Link index to use.
 */ 
template< int L = 1 >
class SharedItem {
	template< typename, int > friend class SharedList;

private:
	std::shared_ptr<SharedItem> m_prev;
	std::shared_ptr<SharedItem> m_next;

#ifdef _DEBUG
	void *debug_parent = nullptr; // the list this item is in
#endif

public:
	virtual ~SharedItem() {}
//	std::shared_ptr<SharedItem> NextLink()     { return m_next; }
//	std::shared_ptr<SharedItem> PreviousLink() { return m_prev; }

	template< typename T > std::shared_ptr<T> NextLink() { 
		return std::static_pointer_cast<T>( m_next ); 
	}

	template< typename T > std::shared_ptr<T> PreviousLink() { 
		return std::static_pointer_cast<T>( m_prev ); 
	}
};	

/** ---------------------------------------------------------------------------
 * A linked list using shared_ptr.
 *
 * @param T The type that will be contained in the list.
 * @param L The link index used.
 */
template< typename T, int L = 1 >
class SharedList {

	using ptr  = std::shared_ptr<T>;

	using Item = SharedItem<L>;
	using Link = std::shared_ptr<Item>;

	Link m_first;
	Link m_last;

public:
	
	virtual ~SharedList() {
		Clear();
	}

	/** -----------------------------------------------------------------------
	 * Clear the list.
	 */
	void Clear() {

		// destroy circular references
		for( auto a = m_first; a; ) {
			auto n = a->m_next;
			a->m_prev.reset();
			a->m_next.reset();
			a = n;
		}

		m_first.reset();
		m_last.reset();
	}

	/** -----------------------------------------------------------------------
	 * Add an item to this list.
	 *
	 * @param item Item to add.
	 */
	void Push( const ptr &item ) {

		assert( item );
		auto i = std::static_pointer_cast<Item>( item );

#       ifdef _DEBUG
			assert( i->debug_parent == nullptr );
			i->debug_parent = (void*)this;
#       endif 

		i->m_next = nullptr;
		
		if( !m_first ) {

			m_first = item;
			m_last  = item; 
			i->m_prev.reset();

		} else {

			m_last->m_next = item;
			i->m_prev      = m_last;
			m_last         = item;
		}
	}
	
	/** -----------------------------------------------------------------------
	 * Append another list to this one.
	 *
	 * @param other Other list to append to the end of this list. The other
	 *              list will be empty after this operation.
	 */
	void Push( SharedList &other ) {
		if( !other.m_first ) return;

		if( m_last ) {
			m_last->m_next = other.m_first;
			other.m_first->m_prev = m_last;
			m_last = other.m_last;
		} else {

			// we are empty
			m_first = other.m_first;
			m_last  = other.m_last;
		}

		other.m_first = nullptr;
		other.m_last  = nullptr;
	}
	
	/** -----------------------------------------------------------------------
	 * Remove an item from this list.
	 *
	 * @param item Pointer to item to remove. 
	 */
	void Pull( const ptr &item ) {
		assert( item );

		auto i = std::static_pointer_cast<Item>( item );
		
#       ifdef _DEBUG
			assert( i->debug_parent == (void*)this );
			i->debug_parent = nullptr;
#       endif 

		if( i->m_next ) {
			i->m_next->m_prev = i->m_prev;
		} else {
			m_last = i->m_prev;
		}

		if( i->m_prev ) {
			i->m_prev->m_next = i->m_next;
		} else {
			
			m_first = i->m_next;
		} 
	}

	// operator fun :)
	void operator +=( const ptr &a  ) { Push( a ); }
	void operator +=( SharedList &a ) { Push( a ); }
	void operator -=( const ptr &a  ) { Pull( a ); } 

	SharedList &operator +( const ptr &a  ) { Push( a ); return *this; }
	SharedList &operator +( SharedList &a ) { Push( a ); return *this; }
	SharedList &operator -( const ptr &a  ) { Pull( a ); return *this; }

	/** -----------------------------------------------------------------------
	 * Returns first item in the list, or nullptr if the list is empty.
	 */
	      ptr  First() { return std::static_pointer_cast<T>(m_first); }
	const ptr &First() const { return m_first; } // todo
	
	/** -----------------------------------------------------------------------
	 * Returns last item in list, or nullptr if the list is empty.
	 */
	      ptr  Last() { return m_last; }// todo
	const ptr &Last() const { return m_last; }// todo
};

}
