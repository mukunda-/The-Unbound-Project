//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
#include "system/module.h"

namespace Video {

//-----------------------------------------------------------------------------
class Camera {

	Eigen::Matrix3f rotation; // orientation
	Eigen::Vector3f position; // translation
	//Eigen::Vector3f position;
	//Eigen::Vector3f focus;
	//Eigen::Vector3f orientation;
	
	float fov;
	bool fov_dirty;

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	Camera() {
		rotation.setIdentity();
		fov = 45.0;
		fov_dirty = false;
		
	}

	void SetFOV( float new_fov ) {
		if( fov != new_fov ) {
			fov = new_fov;
			fov_dirty = true;
		}
	}
	
	Eigen::Matrix3f &GetRotation() {
		return rotation;
	}

	const Eigen::Vector3f &GetPosition() {
		return position;
	}

	void SetPosition( const Eigen::Vector3f &vec_position ) {
		position = vec_position;
	}

	void Move( const Eigen::Vector3f &add_to_position ) {
		position += add_to_position;
	}

	void LookAt( const Eigen::Vector3f &target, const Eigen::Vector3f &up ) {
		
		rotation.col(2) = (position-target).normalized();
		rotation.col(0) = up.cross(rotation.col(2)).normalized();
		rotation.col(1) = rotation.col(2).cross(rotation.col(0));
	}
	  
	const Eigen::Vector3f Forward() const {
		return -rotation.col(2);
	} 

	const Eigen::Vector3f Right() const {
		return rotation.col(0);
	}
	 
	const Eigen::Vector3f Up() const {
		return rotation.col(1);
	}

	void Rotate( const Eigen::Vector3f &angles ) {
		if( angles[0] != 0.0f ) {
			Eigen::AngleAxisf rot( angles[0], rotation.col(0) );
			rotation = rot * rotation;
		}
		if( angles[1] != 0.0f ) {
			Eigen::AngleAxisf rot( angles[1], rotation.col(1) );
			rotation = rot * rotation;
		}
		if( angles[2] != 0.0f ) {
			Eigen::AngleAxisf rot( angles[2], rotation.col(2) );
			rotation = rot * rotation;
		}
	}

	 
	void Rotate( float pitch = 0.0f, float yaw = 0.0f, float roll = 0.0f ) {
		Rotate( Eigen::Vector3f( pitch, yaw, roll ) );
	} 

	void Fixup() {
		// normalize things to be orthogonal
		rotation.col(0) = rotation.col(1).cross(rotation.col(2)).normalized();
		rotation.col(1) = rotation.col(2).cross(rotation.col(0)).normalized();
		rotation.col(2).normalize();
		 
	}
	 
	void UpdateVideo();
	 
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

//-----------------------------------------------------------------------------
class Instance : public System::Module {

private:
	BlendMode       m_blendmode;
	CullingMode     m_cullmode;
	DepthBufferMode m_depthmode;
	
	float         m_bg_color[4];

	float         m_fog_color[4];
	float         m_fog_length; 

	float         m_fov; // field of view
	float         m_view_distance;

	SDL_Window   *m_window     = nullptr; 
	SDL_GLContext m_gl_context = nullptr; 
	
	int           m_screen_width  = 0;
	int           m_screen_height = 0;

public:
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
};
 
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

//-------------------------------------------------------------------------------------------------
// set the far clipping plane distance
//
//void SetViewDistance( float distance );

//-------------------------------------------------------------------------------------------------
// return the distance from the camera to the near plane
//
//float NearPlaneZ();

//-------------------------------------------------------------------------------------------------
// return the distance from the camera to the far plane
//float FarPlaneZ();

//-------------------------------------------------------------------------------------------------
// return the distance between the two viewing planes
//float ViewPlanesRange();

//-------------------------------------------------------------------------------------------------
// return the transformation/projection (camera) matrix
//
const Eigen::Matrix4f &GetXPMatrix();
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
 
//-------------------------------------------------------------------------------------------------
// get/set the rendering blend mode (cleaner method??)
//
BlendMode GetBlendMode();
void SetBlendMode( BlendMode blend_mode );

//-------------------------------------------------------------------------------------------------
// get/set the render culling mode
//
CullingMode GetCullingMode();
void SetCullingMode( CullingMode mode );

//-------------------------------------------------------------------------------------------------
// set the depth buffer mode
//
void SetDepthBufferMode( DepthBufferMode mode );

//-------------------------------------------------------------------------------------------------
// render quads from a vertex buffer
//
void DrawQuads( int start_vertex_index, int number_of_vertexes );

//-------------------------------------------------------------------------------------------------
// render quads using instancing
// start = starting vertex index
// size = number of vertexes
// instances = number of instances of this to render
//
void DrawQuadsInstanced( int start_vertex_index, int number_of_vertexes, int instances );

//-------------------------------------------------------------------------------------------------
// fill the screen with the background color
//
void Clear();

//-------------------------------------------------------------------------------------------------
// change the texture unit slot
//
void SetActiveTextureSlot( int slot );

//-------------------------------------------------------------------------------------------------
void RunWindowLoop( void (*WindowFrameCallback)() );


void BindArrayBuffer( int id );

}
