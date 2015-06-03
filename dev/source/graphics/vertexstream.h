//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Graphics {
	 
/** ---------------------------------------------------------------------------
 * A simple dynamic buffer that can
 * transfer its contents into a VertexBuffer
 * 
 * @param T Vertex format.
 */
template <typename T>
class VertexStream {

	std::vector<T> data;
public:

	virtual ~VertexStream() {}

	/** -----------------------------------------------------------------------
	 * Reset the buffer.
	 */
	void Clear() {
		data.clear();
	}
	
	/** -----------------------------------------------------------------------
	 * Add a vertex.
	 */
	void Push( const T &vert ) {
		data.push_back(vert);
	}
	
	/** -----------------------------------------------------------------------
	 * Copy result into a vertex buffer.
	 */
	void Load( Video::VertexBuffer &buffer ) {
		buffer.Load( (void*)data.data(), (int)data.size() * sizeof( T ) );
	}
};

}
