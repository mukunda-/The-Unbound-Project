//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "mem/memorylib.h"
#include "video/video.h"
#include "video/shader.h"
#include "video/vertexbuffer.h"
#include "util/sharedlist.h"
#include "util/linkedlist.h"
#include "graphics/vertexstream.h"
#include "video/texture.h"
#include "forwards.h"
#include "system/system.h"

//-----------------------------------------------------------------------------
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
	
//-----------------------------------------------------------------------------
enum class RenderLayer : uint8_t {
	BACKDROP,
	SKY,
	CLOUDS,
	TERRAIN,
	WATER,
	OBJECTS,
	PROPS,
	PARTICLES,
	UI
};
	
//-----------------------------------------------------------------------------
/** Elements are a single entity in the graphics scene. 
 */
class Element : 
		public Memory::FastAllocation, 
		public Util::SharedItem<>,
		public std::enable_shared_from_this<Element> {
	
public:
	/** -----------------------------------------------------------------------
	 * Actions that may be taken after the rendering is done.
	 */
	enum class PostAction : uint8_t {
		NONE,		// leave in the render list for the next frame
		REMOVE		// remove from the render list 
	};

private:
	
	// m_depth is for alpha sorting and simple 
	// batch sorting for normal objects
	// it's calculated and stored here before rendering everything
	float       m_depth = 0.0;

	RenderLayer m_layer = RenderLayer::OBJECTS;

	// sorting order for UI elements.
	int         m_sort;

	// blend mode used for rendering
	Video::BlendMode m_blend_mode;

	// pointer to vertex buffer
	Video::VertexBuffer::ptr m_buffer;
	MaterialPtr              m_material;
	
	int m_buffer_index  = 0;
	int m_buffer_offset = 0;
	int m_buffer_start  = 0;
	int m_buffer_size   = 0;

	// primitive rendering mode
	Video::RenderMode m_render_mode = Video::RenderMode::TRIANGLES; 
	
	PostAction m_postaction = PostAction::NONE;
	
	friend class Instance;
protected:
	
public:
	Element();
	virtual ~Element();

	/** -----------------------------------------------------------------------
	 * Set the rendering layer.
	 */
	void        SetRenderLayer( RenderLayer layer ) { m_layer = layer; }
	RenderLayer GetRenderLayer() { return m_layer; }

	/** -----------------------------------------------------------------------
	 * Set the blend mode.
	 */
	void SetBlendMode( Video::BlendMode mode ) { m_blend_mode = mode; }
	Video::BlendMode GetBlendMode() { return m_blend_mode; }

	/** -----------------------------------------------------------------------
	 * Set the vertex buffer.
	 */
	void SetBuffer( Video::VertexBuffer::ptr &ptr ) { m_buffer = ptr; }
	Video::VertexBuffer::ptr &GetBuffer() { return m_buffer; }

	/** -----------------------------------------------------------------------
	 * Set the material.
	 */
	void         SetMaterial( MaterialPtr &mat ) { m_material = mat; }
	MaterialPtr &GetMaterial() { return m_material; }

	/** -----------------------------------------------------------------------
	 * Set the sorting order. This is only used for UI elements.
	 */
	void SetSort( int sort ) { m_sort = sort; }
	int  GetSort() { return m_sort; }

	/** -----------------------------------------------------------------------
	 * Set the vertex buffer parameters.
	 *
	 * Specify `-1` to not change a certain parameter.
	 *
	 * @param index Vertex buffer index, for vertex buffers with multiple
	 *              buffers allocated.
	 * @param offset Byte offset into the vertex buffer for the start of the
	 *               data.
	 * @param start  Starting vertex index.
	 * @param size   Number of vertices to render.
	 */ 
	void SetBufferParams( int index = -1, int offset = -1, int start = -1, 
		                  int size = -1 ) {
		if( index  != -1 ) m_buffer_index  = index;
		if( offset != -1 ) m_buffer_offset = offset;
		if( start  != -1 ) m_buffer_start  = start;
		if( size   != -1 ) m_buffer_size   = size;
	}

	void GetBufferParams( int *index = nullptr, int *offset = nullptr, 
		                  int *start = nullptr, int *size = nullptr ) {

		if( index  ) *index  = m_buffer_index;
		if( offset ) *offset = m_buffer_offset;
		if( start  ) *start  = m_buffer_start;
		if( size   ) *size   = m_buffer_size;
	}

	/** -----------------------------------------------------------------------
	 * Set the rendering mode.
	 */
	void SetRenderMode( Video::RenderMode mode ) { m_render_mode = mode; }
	Video::RenderMode GetRenderMode() { return m_render_mode; }

	/** -----------------------------------------------------------------------
	 * Set the action to be performed after rendering.
	 *
	 * @param action See PostAction.
	 */
	void SetPostAction( PostAction action ) { m_postaction = action; }
	PostAction GetPostAction() { return m_postaction; }

	/** -----------------------------------------------------------------------
	 * initialize the basic attributes of an element
	 *
	 * This also sets all of the buffer params except for 
	 * the size (which is an argument) to zero.
	 */
	void Setup( const Video::VertexBuffer::ptr &buffer, 
		        Video::BlendMode blendmode, 
			    const MaterialPtr &mat, int buffer_size, 
				Video::RenderMode render_mode );

	/** -----------------------------------------------------------------------
	 * Add this element into the rendering list.
	 *
	 * After this is done, the element handle is stored in the graphics
	 * system, and will not (ever!) be deleted if the handle outside goes 
	 * out of scope.
	 *
	 * Ideally you want to use a "remove" post-action if you are discarding
	 * the handle outside.
	 */
	void Add();

	/** -----------------------------------------------------------------------
	 * Remove this element from the rendering list.
	 */
	void Remove();

	using ptr = std::shared_ptr<Element>;

	static ptr Create() {
		// TODO allocate_shared and use pool allocator
		return std::make_shared<Element>();
	}
};

//-----------------------------------------------------------------------------
/** Particles are fixed 8-vertex primitives.
 */
struct Particle : public Memory::FastAllocation, 
	              public Util::LinkedItem<Element> {

	Video::BlendMode blend_mode;
	float depth; // alpha-sorting for translucent, or simple sorting or opaque
	int data_index; // index into vertex data
	int shape; // form of the vertexes
	//struct t_particle *link;
};

/**
 * TODO define
 *
 */
void RenderScene(); 

/** ---------------------------------------------------------------------------
 * @returns the FreeType library handle.
 */
FT_Library FTLib();

/** ---------------------------------------------------------------------------
 * Create a new material.
 *
 * @param name   Name of material.
 * @param shader Name of shader the material will use.
 *
 * @returns pointer to material.
 */
MaterialPtr CreateMaterial( const Stref &name, const Stref &shader );

/** ---------------------------------------------------------------------------
 * Delete a material.
 *
 * @param name Name of material.
 */
void DeleteMaterial( const Stref &name );

/** ---------------------------------------------------------------------------
 * Create a new element.
 */
Element::ptr CreateElement();

/** ---------------------------------------------------------------------------
 * Render a list of elements.
 */
void RenderList( Util::SharedList<Element> &list );

/** ---------------------------------------------------------------------------
 * Render the scene.
 */
void RenderScene();

//-----------------------------------------------------------------------------
class Instance final : public System::Module {
	
	Util::SharedList<Element> m_elements_opaque;
	Util::SharedList<Element> m_elements_blended;

	Util::SharedList<Element> m_elements_ui;

	FT_Library m_ftlib;

	std::unordered_map<std::string, MaterialPtr> m_materials;

	//-------------------------------------------------------------------------
public:
	Instance();
	~Instance();

	FT_Library FTLib() { return m_ftlib; }

	MaterialPtr CreateMaterial( const Stref &name, const Stref &shader );
	void DeleteMaterial( const Stref &name );

	MaterialPtr GetMaterial( const Stref &name );
	MaterialPtr GetMaterial( const Stref &name, MaterialFactory factory );
	
	void RenderList( Util::SharedList<Element> &list );
	void RenderScene();

	void AddElement( const Element::ptr &e );

	Element::ptr CreateElement();
};

}
