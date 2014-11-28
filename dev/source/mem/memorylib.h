//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
// dynamic memory allocation functions
// Fast!

//-------------------------------------------------------------------------------------------------
// maximum allocation unit: 256KB - 16 bytes

//-------------------------------------------------------------------------------------------------
#pragma once
 
//-------------------------------------------------------------------------------------------------
namespace Memory {

//-------------------------------------------------------------------------------------------------
enum {
	ALLOCATION_CHUNK_COUNT = 128,
	LIST_COUNT = 8
};

//-------------------------------------------------------------------------------------------------
struct MemoryHeader {
	int index;
	MemoryHeader *next;
	MemoryHeader *prev;
	int reserved;
};


//-------------------------------------------------------------------------------------------------
class MemoryList {
	MemoryHeader *first;
	MemoryHeader *last;

	boost::mutex guard;

	std::vector<MemoryHeader*> addresses;

	void Expand();
	void Erase();

	int index;
	int size;

public:

	MemoryList();
	~MemoryList();

	void Create( int index, int size );

	void Push( MemoryHeader *entry );
	MemoryHeader * Pop();
};

//-------------------------------------------------------------------------------------------------
class MemoryManager {
	MemoryList lists[LIST_COUNT];

public:
	MemoryManager();
	~MemoryManager();

	void *GetMemory( int size );
	void FreeMemory( void * address );
};

//-------------------------------------------------------------------------------------------------
// internal use only
MemoryManager *GetManager();
//void FreeMemory( void *address );



//#################################################################################################
//#################################################################################################
//-------------------------------------------------------------------------------------------------
// allocate memory for a class
// ie instance = new(Memory::Alloc<myclass>()) myclass();
template<class T> T *Alloc();	
template<class T> T *Alloc<T>() 
{

	return (T*)GetManager()->GetMemory(sizeof(T));
}

//-------------------------------------------------------------------------------------------------
// allocate memory bytes
void *AllocMem( int );

// free a class
//template<class T> void Free( T* data );
template<class T> void Free( T* data ) {
	if( !data ) return;
	data->~T();
	FreeMem( (void*)data );
}

// free bytes
void FreeMem( void * );

// note: the Free functions include null-address safety checks
// (no need to check before calling free)

// returns actual size of allocated memory chunk, excluding header data
int SizeOf( void * );

//-------------------------------------------------------------------------------------------------
struct FastAllocation {
	void* operator new(size_t sz) { 
		void* m = AllocMem( (int)sz );
		return m;
	} 
	void operator delete(void* m) {
		FreeMem(m);
	}
};

}
