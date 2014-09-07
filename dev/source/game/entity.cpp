//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

namespace objects {

class entityList {

private:
	entity *first;
	entity *last;

public:
	
	void erase() {
		first=last=0;
	}
	
	void add( entity *i ) {
		if( last ) {
			i->prev = last;
			i->next = 0;
			last->next = i;
			last = i;
		} else {
			first=last=i;
			i->prev = i->next = 0;
		}
	}

	entity *remove( entity *i ) {
		if( i == first ) {
			first = first->next;
			if( first )
				first->prev = 0;
		} else {
			i->prev->next = i->next;
		}
		if( i == last ) {
			last = last->prev;
			if( last )
				last->next =0;
		} else {
			i->next->prev = i->prev;
		}
		i->prev = i->next = 0;
		return i;
	}

	entity *get_first() {
		return first;
	}

	entity *get_last() {
		return last;
	}

	bool empty() {
		return !(first != 0);
	}
};

entityList entities;

void add( objects::entity *e ) {
	entities.add( e );
}

void update() {
	for( entity *i = entities.get_first(); i;) {
		i->update();
		if( i->is_dead() ) {
			entity *del = i;
			i = i->next;
			entities.remove(del);
			delete del;
		} else {
			i = i->next;
		}
	}
}

void draw() {
	for( entity *i = entities.get_first(); i; i = i->next ) {
		i->render();
	}
}

};
