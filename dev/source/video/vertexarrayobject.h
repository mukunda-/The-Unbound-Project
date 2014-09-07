//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
namespace Video {

//-------------------------------------------------------------------------------------------------
class VertexArrayObject {
	int count;
	GLuint *arrays;

public:
	VertexArrayObject() {
		count = 0;
	}
	~VertexArrayObject() {
		Destroy();
	}
	void Create( int p_count = 1 );
	void Bind( int index = 0 ) {
		
		glBindVertexArray( arrays[index] );
	}
	void Destroy();
};

}
