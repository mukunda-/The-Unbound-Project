//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright � 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

//-------------------------------------------------------------------------------------------------
namespace Game {
 

//-------------------------------------------------------------------------------------------------
class EntityBase {
//-------------------------------------------------------------------------------------------------
private:

//-------------------------------------------------------------------------------------------------
protected:

	
public:
	EntityType type;
//-------------------------------------------------------------------------------------------------
	
	EntityBase *prev;
	EntityBase *next;
};

};
