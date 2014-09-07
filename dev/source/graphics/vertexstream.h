//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

namespace Graphics {


// a simple dynamic buffer that can
// transfer its contents into a VertexBuffer
//---------------------------------------------------------------------------------------
template <typename TVertexFormat>
class VertexStream {

	std::vector<TVertexFormat> data;
public:

	// RESET
	void Clear() {
		data.clear();
	}

	// PUSH VERTEX
	void Push( const TVertexFormat &vert ) {
		data.push_back(vert);
	}

	// LOAD INTO VERTEX BUFFER
	void Load( Video::VertexBuffer &buffer ) {
		buffer.Load( (void*)data.data(), data.size() * sizeof( TVertexFormat ) );
	}
};

}
