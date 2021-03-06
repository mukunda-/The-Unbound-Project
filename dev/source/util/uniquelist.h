//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

// unique_ptr linked list

namespace Util {

template <class T> class UniqueList;

/** ---------------------------------------------------------------------------
 * Interface for objects to be contained in a UniqueList.
 */
template< class T >
class UniqueListItem {
	friend class UniqueList<T>;
	T* m_prevlink = nullptr;
	std::unique_ptr<T> m_nextlink;

public:
	virtual ~UniqueListItem() {}

	T* NextLink()     { return m_nextlink.get(); }
	T* PreviousLink() { return m_prevlink;       }
};

/** ---------------------------------------------------------------------------
 * A linked list using unique_ptr.
 */
template< class T >
class UniqueList {

	using ptr = std::unique_ptr<T>;

	ptr m_first;
	T*  m_last = nullptr;

public:

	virtual ~UniqueList() {}

	/** -----------------------------------------------------------------------
	 * Clear the list.
	 */
	void Clear() {
		m_first = nullptr;
		m_last  = nullptr;
	}

	/** -----------------------------------------------------------------------
	 * Add a new item to this list.
	 *
	 * @param item Item to add, ownership will be removed from this pointer.
	 *
	 * @returns Raw pointer to item. 
	 */
	T *Push( ptr &&item ) {
		if( !m_first ) {
			m_first = std::move( item );
			m_last  = m_first.get();
		} else {
			m_last->m_nextlink = std::move( item );
			m_last->m_nextlink->m_prevlink = m_last;
			m_last = m_last->m_nextlink.get();
		}
		return m_last;
	}
	
	/** -----------------------------------------------------------------------
	 * Append another list to this one.
	 *
	 * @param other Other list to append to the end of this list. The other
	 *              list will be empty after this operation.
	 */
	void Push( UniqueList &other ) {
		if( !other.m_first ) return; // other is already empty
		if( m_last ) {

			m_last->m_nextlink = std::move( other.m_first );
			m_last->m_nextlink->m_prevlink = m_last;
			m_last = other.m_last;

		} else {

			// we are empty!
			m_first = std::move( other.m_first );
			m_last  = other.m_last;
		}
	}
	
	/** -----------------------------------------------------------------------
	 * Remove an item from this list.
	 *
	 * @param item Pointer to item to remove.
	 * @returns unique_ptr to removed item, you now have control over it.
     */
	ptr Pull( T *item ) {

		if( item->m_nextlink ) {
			item->m_nextlink->m_prevlink = item->m_prevlink;
		} else {
			m_last = item->m_prevlink;
		}

		ptr owner;
		if( item->m_prevlink ) {
			owner = std::move( item->m_prevlink->m_nextlink );
			item->m_prevlink->m_nextlink = std::move( item->m_nextlink );
			item->m_prevlink = nullptr;
		} else {
			owner = std::move( m_first );
			m_first = std::move( item->m_nextlink );
		}

		return owner;
	}

	//-------------------------------------------------------------------------
	void operator +=( ptr &&a       ) { Push( std::move(a) ); }
	void operator +=( UniqueList &a ) { Push( a );            }
	void operator -=( T *a          ) { Pull( a );            } 

	UniqueList &operator +( ptr &&a       ) { Push( std::move(a) ); return *this; }
	UniqueList &operator +( UniqueList &a ) { Push( a );            return *this; }
	UniqueList &operator -( T *a          ) { Pull( a );            return *this; }

	/** -----------------------------------------------------------------------
	 * @returns First item in list, or nullptr if the list is empty.
	 */
	T *First()             { return m_first.get(); }
	const T *First() const { return m_first.get(); }
	
	/** -----------------------------------------------------------------------
	 * @returns Last item in list, or nullptr if the list is empty.
	 */
	T *Last()             { return m_last; }
	const T *Last() const { return m_last; }
};

}
