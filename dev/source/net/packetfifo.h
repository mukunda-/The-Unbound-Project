//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once
 
#include "packet.h"

namespace Net {

class PacketFIFO {

	Packet *first;
	Packet *last;
	std::mutex mut;
	std::condition_variable cv_new_data;

	volatile int count;
public:

	PacketFIFO() {
		first=last=0;
	}

	~PacketFIFO() {
		std::lock_guard<std::mutex> lock(mut);
		Packet *p = first;
		while(p) {
			Packet *next = p->next;
			Packet::Delete(p);
			p = next;
		}
	}
	
	void Push( Packet *p ) {
		std::lock_guard<std::mutex> lock(mut);
		if( last ) {
			last->next = p;
			last = p;
		} else {
			first = last = p;
		}
		p->next = 0;
		count++;
		cv_new_data.notify_all();
	}

	Packet *Pop() {
		std::lock_guard<std::mutex> lock(mut);
		if( first ) {
			Packet *p = first;
			first = first->next;
			if( !first ) last = 0;
			count--;
			return p;
		}
		return 0;
	}

	void WaitForData() {
		std::unique_lock<std::mutex> lock(mut);
		while( !first ) {
			cv_new_data.wait( lock );
		}
	}

	int Count() {
		return count;
	}
};

}
