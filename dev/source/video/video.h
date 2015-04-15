//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
#include "system/module.h"

namespace Video {

/** ---------------------------------------------------------------------------
 * A utility class for setting up the view matrices.
 */
class Camera {

	Eigen::Matrix3f m_rotation; // orientation
	Eigen::Vector3f m_position; // translation
	
	float m_fov;
	bool  m_fov_dirty;

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	Camera();
	virtual ~Camera();

	/** -----------------------------------------------------------------------
	 * Set the field of view for this camera.
	 *
	 * @param fov Vertical (?) field of view in radians (TODO confirm)
	 */
	void SetFOV( float fov );
	
	/** -----------------------------------------------------------------------
	 * Read the current rotation matrix.
	 */
	Eigen::Matrix3f &GetRotation() { return m_rotation; }
	const Eigen::Matrix3f &GetRotation() const { return m_rotation; }

	/** -----------------------------------------------------------------------
	 * Read the current translation vector.
	 */
	Eigen::Vector3f &GetPosition() { return m_position; }
	const Eigen::Vector3f &GetPosition() const { return m_position; }

	/** -----------------------------------------------------------------------
	 * Set the current translation vector.
	 */
	void SetPosition( const Eigen::Vector3f &vec_position );

	/** -----------------------------------------------------------------------
	 * Add to the current translation vector.
	 */
	void Move( const Eigen::Vector3f &add_to_position );

	/** -----------------------------------------------------------------------
	 * Set the current rotation to Look At a point in space.
	 *
	 * @param target Point to look at.
	 * @param up     Vector pointing "up", to determine orientation.
	 */
	void LookAt( const Eigen::Vector3f &target, const Eigen::Vector3f &up );
	  
	/** -----------------------------------------------------------------------
	 * @returns the vector that points forward, or towards the camera's focus.
	 */
	Eigen::Vector3f Forward() const { return -m_rotation.col(2); } 

	
	/** -----------------------------------------------------------------------
	 * @returns the vector that points "right" from the camera's position,
	 *          perpendicular to the "up" and "forward" vectors.
	 */
	Eigen::Vector3f Right() const { return m_rotation.col(0); }
	 
	/** -----------------------------------------------------------------------
	 * @returns the vector that points upward. Not straight up, but
	 *          perpendicular to the right and forward vectors.
	 */
	const Eigen::Vector3f Up() const { return m_rotation.col(1); }

	/** -----------------------------------------------------------------------
	 * Rotate the camera using euler angles.
	 *
	 * @param angles [0] = pitch, [1] = yaw, [2] = roll
	 */
	void Rotate( const Eigen::Vector3f &angles );
	
	/** -----------------------------------------------------------------------
	 * Rotate the camera using euler angles.
	 *
	 * @param pitch,yaw,roll Euler angles.
	 */
	void Rotate( float pitch = 0.0f, float yaw = 0.0f, float roll = 0.0f ) {
		Rotate( Eigen::Vector3f( pitch, yaw, roll ) );
	} 

	/** -----------------------------------------------------------------------
	 * Normalize the rotation matrix to correct deformation from floating
	 * point inaccuracies.
	 *
	 * This should be called frequently.
	 */
	void Fixup();
	
	/** -----------------------------------------------------------------------
	 * Update the video matrices with this camera.
	 */
	void UpdateVideo(); 

	friend class Instance;
};

/** ---------------------------------------------------------------------------
 * Depth buffer modes for SetDepthBufferMode.
 */
typedef enum : uint8_t {
	ZBUFFER_DISABLED,
	ZBUFFER_WRITEONLY,
	ZBUFFER_READONLY,
	ZBUFFER_ENABLED
} DepthBufferMode;

/** ---------------------------------------------------------------------------
 * Blend modes for Get/SetBlendMode
 */
typedef enum : uint8_t {
	BLEND_OPAQUE,
	BLEND_ALPHA,
	BLEND_ADD,
	BLEND_SUB
} BlendMode;

/** ---------------------------------------------------------------------------
 * Culling modes for Get/SetCullingMode.
 */
typedef enum : uint8_t {
	CULLMODE_NONE,
	CULLMODE_BACK,
	CULLMODE_FRONT
} CullingMode;

/** ---------------------------------------------------------------------------
 * not sure what this is used for yet.
 */
typedef enum : uint8_t {
	WINDOWED,
	FULLSCREEN
} ScreenModes;

/** ---------------------------------------------------------------------------
 * Open/create a display.
 *
 * @param width  Width of window to create.
 * @param height Height of window to create.
 */
void Open( int p_width, int p_height );

/** ---------------------------------------------------------------------------
 * Close the display.
 */
void Close(); 

/** ---------------------------------------------------------------------------
 * Swap view after finishing rendering operations.
 */
void Swap();

/** ---------------------------------------------------------------------------
 * Get screen dimensions.
 */
int ScreenWidth();
int ScreenHeight();

/** ---------------------------------------------------------------------------
 * Direct (OpenGL) texture binding functions
 */
void BindTextureHandle( uint32_t texture );
void BindTextureArrayHandle( uint32_t texture );

/** ---------------------------------------------------------------------------
 * Target global surface for rendering operations.
 */
void UseGlobalSurface();

/** ---------------------------------------------------------------------------
 * Set the background color for the Clear function, and for custom shaders
 * that use it.
 *
 * @param r,g,b Components in range [0,1]
 */
void SetBackgroundColor( float r, float g, float b );

//-------------------------------------------------------------------------------------------------
// simple set camera function
// xyz = position
// u = yaw
// v = pitch
//
//void SetCamera( float x, float y, float z, float u, float v );

//-------------------------------------------------------------------------------------------------
// set camera with point and focus target
// assumed up vector is 0,+1,0
//void SetCamera_LookAt( cml::vector3f position, cml::vector3f target, float roll = 0.0f );

//-------------------------------------------------------------------------------------------------
// create a vector for picking from screen coordinates
//
// coordinates are in range 0.0 (left/top) to 1.0 (bottom/right)
//cml::vector3f GetPickRay( cml::vector2f screen_coordinates );

//-------------------------------------------------------------------------------------------------
// get position of camera
//
//cml::vector3f GetCamera();

//-------------------------------------------------------------------------------------------------
// create a vector pointing to the right of the camera
//
//cml::vector3f GetCameraRightVector();

//-------------------------------------------------------------------------------------------------
// create a vector pointing forward from the camera
//
//cml::vector3f GetCameraForwardVector();

//-------------------------------------------------------------------------------------------------
// similar to getpickray (???)
//cml::vector3f GetRelativeCameraPoint( float x, float y, float z );

//-------------------------------------------------------------------------------------------------
// bob camera function that really shouldn't be here
// (game specific shit)
// anyway offset is a time value, and velocity is the speed/scale of the view bobbing
//
//void SetCameraBob( float offset, float velocity );

//-------------------------------------------------------------------------------------------------
// camera shake functions (todo: move this somewhere else)
//void ShakeCamera( float power );
//void FadeCameraShake( float factor );
//float AddCameraShakeTime( float time );

//-------------------------------------------------------------------------------------------------
// get the distance from a point in space to the camera
// the second version returns distance^2 (faster)
//
//float GetCameraDistance( cml::vector3f point );
//float GetCameraDistance2( cml::vector3f point );

//-------------------------------------------------------------------------------------------------
// ?????
//GLuint GetFrameBufferObject();

//-------------------------------------------------------------------------------------------------
//? ? ?? ?? ? not used anymore
//void BindFramebufferTexture();

/** ---------------------------------------------------------------------------
 * Set the far plane (clipping) distance.
 *
 * @param distance Distance in world units.
 */
void SetViewDistance( float distance );

/** ---------------------------------------------------------------------------
 * @returns the distance from the camera to the near plane.
 */
float NearPlaneZ();

/** ---------------------------------------------------------------------------
 * @returns the distance from the camera to the far plane.
 */
float FarPlaneZ();

/** ---------------------------------------------------------------------------
 * @returns the distance between the two view planes.
 */
float ViewPlanesRange();

/** ---------------------------------------------------------------------------
 * @returns the current transformation/projection (camera) matrix
 */
const Eigen::Matrix4f &GetXPMatrix();

/** ---------------------------------------------------------------------------
 * @returns the current serial number for the xp matrix. This increments
 *          every time the xp matrix changes.
 */
int GetXPMatrixSerial();

//-------------------------------------------------------------------------------------------------
// return the inverse of transformation/projection (camera) matrix
//
//const cml::matrix44f_c *GetXPMatrixInverse();

//-------------------------------------------------------------------------------------------------
// get the corner points of the near or far planes
// [0]<-------[3]
//  |          ^  vertex ordering on screen
//  v          |
// [1]------->[2]
//
//cml::vector3f *GetNearPlane();
//cml::vector3f *GetFarPlane();

//-------------------------------------------------------------------------------------------------
// get/set the length of fog from the far plane
//
//void SetFogLength( float f ); THIS SHOULD BE A SHADER INHERITANCE ?
//float GetFogLength();
 
/** ---------------------------------------------------------------------------
 * Get/set the rendering blend mode.
 *
 * @see BlendMode
 */
BlendMode GetBlendMode();
void SetBlendMode( BlendMode mode );


/** ---------------------------------------------------------------------------
 * Get/set the render culling mode.
 *
 * @see CullingMode
 */
void SetCullingMode( CullingMode mode );
CullingMode GetCullingMode();

/** ---------------------------------------------------------------------------
 * Get/set the depth buffering mode.
 *
 * @see DepthBufferMode enum.
 */
void SetDepthBufferMode( DepthBufferMode mode );
DepthBufferMode GetDepthBufferMode();


/** ---------------------------------------------------------------------------
 * Render quads.
 *
 * @param start Starting vertex index.
 * @param count Number of vertexes to render.
 */
void DrawQuads( int start_vertex_index, int number_of_vertexes );

/** ---------------------------------------------------------------------------
 * Render quads using instancing.
 *
 * @param start     Starting vertex index.
 * @param count     Number of vertexes to render (should be multiple of 4).
 * @param instances Number of instances to render.
 */
void DrawQuadsInstanced( int start, int count, int instances );

/** ---------------------------------------------------------------------------
 * Fill the screen with the background color.
 */
void Clear();

/** ---------------------------------------------------------------------------
 * Change the texture unit slot.
 */
void SetActiveTextureSlot( int slot );

/** ---------------------------------------------------------------------------
 * Bind an array (vertex) buffer.
 */
void BindArrayBuffer( GLuint buffer );

/** ---------------------------------------------------------------------------
 * @returns a pointer to the currently active shader.
 */
Shader *GetActiveShader();

/** ---------------------------------------------------------------------------
 * Find a shader by name.
 *
 * @param name The name the shader registered with.
 *
 * @returns pointer to the shader, or nullptr if not found.
 */
Shader *FindShader( const Stref &name );

/** ---------------------------------------------------------------------------
 * Register a new shader.
 *
 * @param name The name the shader registered with.
 *
 * @returns pointer to the shader, or nullptr if not found.
 */
template< typename T, typename ... A > void RegisterShader( A ... args ) {
	GetInstance()->RegisterShader<T>( args ... );
}

//-----------------------------------------------------------------------------
class Instance : public System::Module {

private:
	BlendMode       m_blendmode;
	CullingMode     m_cullmode;
	DepthBufferMode m_depthmode;
	
	float           m_bg_color[4];

	float           m_fog_color[4];
	float           m_fog_length; 

	float           m_fov; // field of view
	float           m_view_distance;

	SDL_Window     *m_window     = nullptr; 
	SDL_GLContext   m_gl_context = nullptr; 
	
	int             m_screen_width  = 0;
	int             m_screen_height = 0;

	Eigen::Matrix4f m_mat_view; 
	Eigen::Matrix4f m_mat_projection;
	Eigen::Matrix4f m_mat_xp;

	Eigen::Vector3f m_near_plane[4];
	Eigen::Vector3f m_far_plane[4];

	// this number is incremented each time the mat_xp matrix changes
	// it's used as a quick check if the matrix in a shader should be updated
	int             m_matxp_serial = 0;

	std::unordered_map<	std::string, std::unique_ptr<Shader> > m_shaders;
	Shader         *m_active_shader;

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	
	Instance();
	~Instance();

	void Open( int width, int height );
	void Close();

	int ScreenWidth() const { return m_screen_width; }
	int ScreenHeight() const { return m_screen_height; }

	void Swap();

	void UseGlobalSurface();
	void SetBackgroundColor( float r, float g, float b );
	
	void BindTextureHandle( GLuint texture );
	void BindTextureArrayHandle( GLuint texture );

	void SetFogLength( float length );
	float GetFogLength() const { return m_fog_length; }

	void SetBlendMode( BlendMode mode );
	BlendMode GetBlendMode() const { return m_blendmode; }

	void SetCullingMode( CullingMode mode );
	CullingMode GetCullingMode() const { return m_cullmode; }

	void SetDepthBufferMode( DepthBufferMode mode );
	DepthBufferMode GetDepthBufferMode() const { return m_depthmode; }

	void Clear();

	void DrawQuads( int start, int size );
	void DrawQuadsInstanced( int start, int size, int instances );

	void SetActiveTextureSlot( int slot );

	void BindArrayBuffer( GLuint buffer );

	float NearPlaneZ();
	float FarPlaneZ();
	float ViewPlanesRange();

	void SetupProjection( float fovY, float aspect, float fnear, float ffar );
	void UpdateViewport();

	void CopyCamera( Camera &camera );

	const Eigen::Matrix4f &GetXPMatrix() const { return m_mat_xp; }
	int GetXPMatrixSerial() const { return m_matxp_serial; }

	Shader *GetActiveShader();
	Shader *FindShader( const Stref &name );
};
 
}
