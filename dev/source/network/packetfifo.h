//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include <boost/thread.hpp>
#include "network/packet.h"

namespace Network {

class PacketFIFO {

	Packet *first;
	Packet *last;
	boost::mutex mut;
	boost::condition_variable cv_new_data;

	volatile int count;
public:

	PacketFIFO() {
		first=last=0;
	}

	~PacketFIFO() {
		boost::lock_guard<boost::mutex> lock(mut);
		Packet *p = first;
		while(p) {
			Packet *next = p->next;
			DeletePacket(p);
			p = next;
		}
	}
	
	void Push( Packet *p ) {
		boost::lock_guard<boost::mutex> lock(mut);
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
		boost::lock_guard<boost::mutex> lock(mut);
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
		boost::unique_lock<boost::mutex> lock(mut);
		while( !first ) {
			cv_new_data.wait( lock );
		}
	}

	int Count() {
		return count;
	}
};

}
