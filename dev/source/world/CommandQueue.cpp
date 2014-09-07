//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#include "stdafx.h"

//-------------------------------------------------------------------------------------------------
namespace World {

//-------------------------------------------------------------------------------------------------
CommandQueue::CommandQueue() {
	first = last = 0;
	enabled = true;
	disable_waits = false;
}

//-------------------------------------------------------------------------------------------------
CommandQueue::~CommandQueue() {

}

//-------------------------------------------------------------------------------------------------
Command *CommandQueue::Get() {
	boost::lock_guard<boost::mutex> guard(mut);
	return first;
}

//-------------------------------------------------------------------------------------------------
Command *CommandQueue::Pop() {
	boost::lock_guard<boost::mutex> guard(mut);
	if( !first ) return 0;
	Command *a = first;
	first = first->cq_next;
	if( !first ) last = 0;
	return a;
}

//-------------------------------------------------------------------------------------------------
void CommandQueue::Push( Command *cmd ) {
	{
		boost::lock_guard<boost::mutex> guard(mut);
		if( last ) {
			last->cq_next = cmd;
			last = cmd;
		} else {
			first = last = cmd;
		}
		cmd->cq_next = 0;
	}
	commands_added.notify_one();
}

//-------------------------------------------------------------------------------------------------
void CommandQueue::WaitForCommand() {
	boost::unique_lock<boost::mutex> lock(mut);

	while( !first && !disable_waits ) {
		commands_added.wait(lock);
	}
	
}
 
//-------------------------------------------------------------------------------------------------
void CommandQueue::DisableWaits() {
	boost::lock_guard<boost::mutex> lock(mut);
	disable_waits = true;
	commands_added.notify_all();
}

//-------------------------------------------------------------------------------------------------
void CommandQueue::EnableWaits() {
	boost::lock_guard<boost::mutex> lock(mut); // necessary ??
	disable_waits = false;
}

}
