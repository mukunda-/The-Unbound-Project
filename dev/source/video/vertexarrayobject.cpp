//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"
#include "video/vertexarrayobject.h"

//-------------------------------------------------------------------------------------------------
namespace Video {
 
//-------------------------------------------------------------------------------------------------
void VertexArrayObject::Create( int p_count ) {
	if( count ) Destroy();

	arrays = new GLuint[p_count]; 
	printf("2");
	glGenVertexArrays( p_count, arrays );
	
	printf("3");
	count = p_count;

	int error = glGetError();

	printf( "VAO created; code %d\n", error );
}

//-------------------------------------------------------------------------------------------------
void VertexArrayObject::Destroy() {
	if( count ) {
		glDeleteVertexArrays( count, arrays );
		delete[] arrays;
		arrays = 0;
	}
	count = 0;
}

}
