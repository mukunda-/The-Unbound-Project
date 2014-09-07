//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"
#include "memorylib.h"

//-------------------------------------------------------------------------------------------------
namespace Memory {



const int list_sizes[] = {
	64 - sizeof(MemoryHeader),		//64b XX-SMALL
	256 - sizeof(MemoryHeader),		//256b X-SMALL
	1024 - sizeof(MemoryHeader),	//1kb SMALL
	4096 - sizeof(MemoryHeader),	//4kb MEDIUM
	16384 - sizeof(MemoryHeader),	//16kb LARGE
	32768 - sizeof(MemoryHeader),	//32kb X-LARGE
	65536 - sizeof(MemoryHeader),	//64kb XX-LARGE
	262144 - sizeof(MemoryHeader)	//256kb MEGASIZE
};

//-------------------------------------------------------------------------------------------------
MemoryList::MemoryList() {
	first = last = 0;
	index = 0;
	size = 0;
}

//-------------------------------------------------------------------------------------------------
MemoryList::~MemoryList() {
	// todo.... :)
}

//-------------------------------------------------------------------------------------------------
void MemoryList::Erase() {
	first = last = 0;
	for( unsigned int i = 0; i < addresses.size(); i++ ) {
		_aligned_free( (void*)addresses[i] );
	}
	addresses.clear();
}

//-------------------------------------------------------------------------------------------------
void MemoryList::Create( int p_index, int p_size ) {
	Erase();
	index = p_index;
	size = p_size;
}

//-------------------------------------------------------------------------------------------------
void MemoryList::Expand() {
	int chunk_size = size + sizeof(MemoryHeader);

	MemoryHeader *new_first, *new_last;
	MemoryHeader *new_addresses = (MemoryHeader*)_aligned_malloc( chunk_size * ALLOCATION_CHUNK_COUNT, 16 );
	if( new_addresses == nullptr ) {
		throw std::runtime_error( "Out of memory." );
	}

	new_first = new_addresses;
	new_last = &new_addresses[ALLOCATION_CHUNK_COUNT-1];
	MemoryHeader *iter = new_first, *next, *prev = 0;

	for( int i = 0; i < ALLOCATION_CHUNK_COUNT-1; i++ ) {
		iter->index = index;
		next = (MemoryHeader*)((char*)iter + (sizeof(MemoryHeader)+size));
		iter->next = next;
		iter->prev = prev;
		prev = iter;
		iter = next;
	}

	iter->index = index;
	iter->next = 0;
	iter->prev = prev;
	new_last = iter;
	
	//{
		//boost::lock_guard<boost::mutex> lock(guard);
	if( last ) {
		last->next = new_first;
		new_first->prev = last;
		last = new_last;
	} else {
		first = new_first;
		last = new_last;
	}
	//}

}

//-------------------------------------------------------------------------------------------------
void MemoryList::Push( MemoryHeader *entry ) {

	boost::lock_guard<boost::mutex> lock(guard);

	entry->next = 0;

	if( last ) {
		last->next = entry;
		entry->prev = last;
		last = entry;
	} else {
		entry->prev = 0;
		first = last = entry;
	}
}

//-------------------------------------------------------------------------------------------------
MemoryHeader * MemoryList::Pop() {

	boost::lock_guard<boost::mutex> lock(guard);
	if( !first ) {
		Expand();
	}

	// unlink first entry and return it
	MemoryHeader *h = first;
	first = first->next;
	if( first ) {
		first->prev = 0;
	} else {
		last=0;
	}
	return h;
	
}

//-------------------------------------------------------------------------------------------------
MemoryManager::MemoryManager() {
	for( int i = 0; i < LIST_COUNT; i++ ) {
		lists[i].Create( i, list_sizes[i]  );
	}
}

//-------------------------------------------------------------------------------------------------
MemoryManager::~MemoryManager() {
	
}

//-------------------------------------------------------------------------------------------------
void *MemoryManager::GetMemory( int size ) {
	// convert size to index
	int size_index = -1;
	for( int i = 0; i < LIST_COUNT; i++ ) {
		if( size <= list_sizes[i] ) {
			size_index = i;
			break;
		}
	}

	if( size_index == -1 ) return 0;

	MemoryHeader *ch = lists[size_index].Pop();
	 
	return (void*)((unsigned int)((char*)ch + sizeof(MemoryHeader)));
}

//-------------------------------------------------------------------------------------------------
void MemoryManager::FreeMemory( void *address ) {

	// adjust address to point to header
	MemoryHeader *ch = (MemoryHeader*)((char*)address - sizeof(MemoryHeader));
 
	lists[ch->index].Push(ch);
}

//-------------------------------------------------------------------------------------------------
MemoryManager manager;

MemoryManager *GetManager() {
	return &manager;
}

//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
void *AllocMem( int size ) {
	return manager.GetMemory(size);
}

//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
void FreeMem( void * data ) {
	if( !data ) return;
	manager.FreeMemory( data );
}

//-------------------------------------------------------------------------------------------------
int SizeOf( void * data ) {
	if( !data ) return 0;
	MemoryHeader *h = (MemoryHeader*)((boost::uint8_t*)data - sizeof(MemoryHeader));
	return list_sizes[h->index];
}

}
