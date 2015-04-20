//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once
  
namespace Util {

template < typename T > class SharedList;

/** ---------------------------------------------------------------------------
 * Interface for objects to be contained in a SharedList.
 */
template< typename T >
class SharedListItem {
	friend class SharedList<T>;

private:
	std::shared_ptr<T> m_prev;
	std::shared_ptr<T> m_next;

public:
	virtual ~SharedListItem() {}
	std::shared_ptr<T> NextLink()     { return m_next; }
	std::shared_ptr<T> PreviousLink() { return m_prev; }
};

/** ---------------------------------------------------------------------------
 * A linked list using shared_ptr.
 */
template< typename T >
class SharedList {

	using ptr = std::shared_ptr<T>;

	ptr m_first;
	ptr m_last;

public:
	
	virtual ~SharedList() {}

	/** -----------------------------------------------------------------------
	 * Clear the list.
	 */
	void Clear() {
		m_first.reset();
		m_last.reset();
	}

	/** -----------------------------------------------------------------------
	 * Add an item to this list.
	 *
	 * @param item Item to add.
	 */
	void Push( const ptr &item ) {

		item->m_next = nullptr;
		
		if( !m_first ) {

			m_first      = item;
			m_last       = item;
			item->m_prev = nullptr;

		} else {

			m_last->m_next = item;
			item->m_prev   = m_last;
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

		if( item->m_next ) {
			item->m_next->m_prev = item->m_prev;
		} else {
			m_last = item->m_prev;
		}

		if( item->m_prev ) {
			item->m_prev->m_next = item->m_next;
		} else {
			m_first = item->m_next;
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
	ptr First() { return m_first; }
	const ptr &First() const { return m_first; }
	
	/** -----------------------------------------------------------------------
	 * Returns last item in list, or nullptr if the list is empty.
	 */
	ptr Last() { return m_last; }
	const ptr &Last() const { return m_last; }
};

}
