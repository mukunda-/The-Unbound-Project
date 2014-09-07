//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include "mem/memorylib.h"
#include "video/video.h"
#include "video/shader.h"
#include "video/vertexbuffer.h"
#include "util/LinkedList.h"
#include "graphics/vertexstream.h"
#include "video/textures.h"

namespace Graphics {

		//         7/22/2014
		//          (shader is no longer fixed)
//PART				SHADER		ALPHA	ZSORT
//backdrop			BACKDROP	NO		NO		the sky box, basically the sky gradient i guess..	 rendered first
//sky objects		OBJECTS		YES		FIXED   sun, moon, etc, objects that are really far away
//clouds			CLOUDS		YES		FIXED	clouds... no z sorting, but they are translucent
//terrain			TERRAIN		NO		NO		solid terrain, and maybe minor translucent details
//water				TERRAIN		YES		FIXED	translucent layer of terrain
//objects/meshes	OBJECTS		YES		YES		most of everything
//props/boards		OBJECTS		YES		YES
//particles			PARTICLES	YES		YES		?
//ui				-			YES		?		ui "on-screen" elements, 2d, sorted by m_sort
	
enum RenderLayer {
	LAYER_BACKDROP,
	LAYER_SKYOBJ,
	LAYER_CLOUDS,
	LAYER_TERRAIN,
	LAYER_WATER,
	LAYER_OBJECTS,
	//LAYER_PROPS,
	//LAYER_PARTICLES,
	LAYER_UI
};
	
//---------------------------------------------------------------------------------------
class Material {

	static const int TEXTURE_SLOTS = 4;

	Video::Shader *shader; 
	boost::shared_ptr<Video::Shader::Kernel> kernel; 
	Video::Texture::Pointer textures[TEXTURE_SLOTS];


public:
	Material( const std::string &shader_name );

	void SetParam( const std::string &name, const std::string &value ); 
	void SetTexture( int slot, const Video::Texture::Pointer &tex );
	Video::Texture::Pointer GetTexture( int slot );
	void Bind();
};

//---------------------------------------------------------------------------------------
struct Element : public Memory::FastAllocation, public Util::LinkedItem<Element> {
	
	// translate; TODO? im guessing this is passed to 
	// a standard shader uniform before rendering
	//[1:59 PM 7/22/2014] this should be an extension of element, not in the base
	//Eigen::Vector3f m_translate;
	
	// m_depth is for alpha sorting and
	//   simple batch sorting for normal objects
	//  it's calculated and stored here before rendering everything
	float m_depth;

	RenderLayer m_layer;
	int m_sort;

	// blend mode used for rendering this object
	// 
	Video::BlendMode m_blend_mode;

	// pointer to vertex buffer
	Video::VertexBuffer::Pointer m_buffer;
	Material *m_material;
	
	GLuint m_buffer_index; // vertex buffer index, for VertexBuffers with 
						 // multiple buffer allocations
	GLuint m_buffer_offset; // data offset into the vertex buffer, in bytes
	GLuint m_buffer_start; // offset into the vertex data, in vertexes
	GLuint m_buffer_size; // number of vertices to render
	GLenum m_render_mode; // GL rendering mode (GL_TRIANGLES, etc)
	 

	// auto-remove modes,
	// after rendering this is performed:
	enum {
		AR_NONE,	// leave in the rendering list for the next frame
		AR_REMOVE,	// remove from the render list
		AR_DELETE	// remove from the render list and delete 'this' object
	} m_autoremove;
	  
	Element();
};

//---------------------------------------------------------------------------------------
// particles are fixed 8-vertex primitives
struct Particle : public Memory::FastAllocation, public Util::LinkedItem<Element> {

	Video::BlendMode blend_mode;
	float depth; // alpha-sorting for translucent, or simple sorting or opaque
	int data_index; // index into vertex data
	int shape; // form of the vertexes
	struct t_particle *link;

};


//---------------------------------------------------------------------------------------
void Reset();
void Init();
//void new_scene();

//void compute_viewing_planes();

void AddElement( Element &e );

//void draw_sprite( cml::vector3f position, float width, float height, float u1, float v1, float u2, float v2, u8 blending, u8 r, u8 g, u8 b, u8 a, int rot, bool vertical );

// element will be buffered if blending mode is non opaque
//void draw_element( element *a );


// use to ignore buffering cycle for blended elements
//void render_element( const element *e );


// sort translucent objects and render
//void finalize_data();
//void render_solid_graphics();
//void render_blended_graphics();

void RenderScene();

// helper functions
void SetupElement( Element &e, Video::VertexBuffer::Pointer &buffer, Video::BlendMode blendmode, 
				  Material &mat, GLuint buffer_index, GLuint buffer_offset, 
				  GLuint buffer_start, GLuint buffer_size, GLenum render_mode );

}
