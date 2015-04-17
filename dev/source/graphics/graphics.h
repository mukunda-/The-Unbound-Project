//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

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
/** Materials are a combination of rendering settings used for rendering an
 *  element.
 */
class Material {

	enum {

		// max number of textures that may be used for a single material
		TEXTURE_SLOTS = 4
	};

	// shader to use to render this material
	Video::Shader *shader; 

	// shader kernel settings to pass to shader
	std::shared_ptr<Video::Shader::Kernel> kernel; 

	// texture handles for the textures used
	Video::Texture::Pointer textures[TEXTURE_SLOTS];


public:
	Material( const Stref &shader_name );
	virtual ~Material() {}

	/**
	 * Set a shader parameter.
	 *
	 * \param name Name of parameter to set.
	 * \param value Value to pass to shader kernel. 
	 */
	void SetParam( const std::string &name, const std::string &value ); 

	/**
	 * Set one of the material's textures.
	 *
	 * \param slot [0,TEXTURE_SLOTS) slot of texture to set.
	 * \param tex Texture handle
	 */
	void SetTexture( int slot, const Video::Texture::Pointer &tex );

	/**
	 * Get the handle to one of the textures used in this material.
	 *
	 * \param slot [0,TEXTURE_SLOTS) slot of the texture to read
	 * \return The texture handle in the slot, or nullptr if none.
	 */
	Video::Texture::Pointer GetTexture( int slot );

	/**
	 * Bind this texture to the GL context for rendering.
	 */
	void Bind();
};

//---------------------------------------------------------------------------------------
/** Elements are a single entity in the graphics scene. 
 */
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
/** Particles are fixed 8-vertex primitives.
 */
struct Particle : public Memory::FastAllocation, public Util::LinkedItem<Element> {

	Video::BlendMode blend_mode;
	float depth; // alpha-sorting for translucent, or simple sorting or opaque
	int data_index; // index into vertex data
	int shape; // form of the vertexes
	//struct t_particle *link;
};


//---------------------------------------------------------------------------------------
/**
 * REMOVED
 */
//void Reset();

/**
 * Initialize the graphic system... currently does NOTHING and may be
 * removed.
 *
 */
void Init();
//void new_scene();

//void compute_viewing_planes();

/**
 * Add a new element to the next scene to be rendered.
 *
 */
void AddElement( Element &element );

//void draw_sprite( cml::vector3f position, float width, float height, float u1, float v1, float u2, float v2, u8 blending, u8 r, u8 g, u8 b, u8 a, int rot, bool vertical );

// element will be buffered if blending mode is non opaque
//void draw_element( element *a );


// use to ignore buffering cycle for blended elements
//void render_element( const element *e );


// sort translucent objects and render
//void finalize_data();
//void render_solid_graphics();
//void render_blended_graphics();

/**
 * TODO define
 *
 */
void RenderScene();

/** 
 * initialize the basic attributes of an element
 *
 * This also sets all of the buffer params except for the size (which is an argument)
 * to zero.
 */
void SetupElement( Element &e, Video::VertexBuffer::Pointer &buffer, Video::BlendMode blendmode, 
				  Material &mat, GLuint buffer_size, GLenum render_mode );

//-----------------------------------------------------------------------------
class Instance final {
	
	Util::LinkedList<Element> m_elements_opaque;
	Util::LinkedList<Element> m_elements_blended;

	Util::LinkedList<Element> m_elements_ui;

	FT_Library m_ftlib;

public:
	Instance();
	~Instance();

	FT_Library *FreeType();
};

}
