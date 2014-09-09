//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "video/video.h"

//------------------------------------------------------------------------------------------------
namespace Video {
//------------------------------------------------------------------------------------------------
	
class Context {
	SDL_Window *window; 
	SDL_GLContext glcontext; 
 
	int screen_width;
	int screen_height;

public:
	Context() {
		window = 0; 
		glcontext = 0;
	} 
	void Destroy() { 
		if( glcontext ) SDL_GL_DeleteContext( glcontext );
		if( window ) SDL_DestroyWindow( window ); 
		window = 0;
		glcontext = 0;
	}
	void Create( int width, int height ) {
		Destroy();
		screen_width = width;
		screen_height = height;
		window = SDL_CreateWindow( "GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,width, height, SDL_WINDOW_OPENGL );
		
		if( !window ) {
			throw std::exception( "Window creation error." );
		}  
		glcontext = SDL_GL_CreateContext(window);
	}

	int ScreenWidth() const {
		return screen_width < 1 ? 1 : screen_width;
	}

	int ScreenHeight() const {
		return screen_height < 1 ? 1 : screen_height;
	} 

	void Swap() {
		SDL_GL_SwapWindow( window );
	}
} context; 


//-------------------------------------------------------------------------------------------------
//  camera
//Eigen::Vector3f camera_position;
//Eigen::Vector3f camera_forward;
//Eigen::Vector3f camera_right;
//Eigen::Vector3f camera_up;
//Eigen::Vector3f camera_punch;

//-------------------------------------------------------------------------------------------------
float camera_shake;
float camera_shake_time;
float camera_shake_power;
float camera_shake_power2;

static const float camera_shake_power_max = 1.0;

//-------------------------------------------------------------------------------------------------
BlendMode blending_mode;
CullingMode culling_mode;

float bg_color[4];
float fog_color[4];

float fog_length;
 
float field_of_view;

//-------------------------------------------------------------------------------------------------
// windows shit
HGLRC hglrc;
HDC hdc;

HWND hwnd;
HINSTANCE hinst;

//-------------------------------------------------------------------------------------------------
  
float view_distance;
int windowtype;
int posx, posy;
  
//-------------------------------------------------------------------------------------------------
Eigen::Matrix4f mat_view; 
Eigen::Matrix4f mat_projection;
Eigen::Matrix4f mat_xp;
Eigen::Matrix4f mat_xpi;
int matxp_serial=0;

Eigen::Vector3f near_plane[4];
Eigen::Vector3f far_plane[4];

//GLint current_vertex_buffer;
  
//-------------------------------------------------------------------------------------------------
float NearPlaneZ() {
	return 0.5f;
}

//-------------------------------------------------------------------------------------------------
float FarPlaneZ() {
	return view_distance;
}

//-------------------------------------------------------------------------------------------------
float ViewPlanesRange() {
	return view_distance - 1.0f;
}

//-------------------------------------------------------------------------------------------------
void SetupProjection( float fovY, float aspect, float fnear, float ffar ) {
	field_of_view = fovY;
	float theta = fovY*0.5f;
	float range = ffar - fnear;
	float invtan = 1.0f/tan(theta);
	 
	mat_projection(0,0) = invtan / aspect;
	mat_projection(1,1) = invtan;
	mat_projection(2,2) = -(fnear + ffar) / range;
	mat_projection(3,2) = -1;
	mat_projection(2,3) = -2 * fnear * ffar / range;
	mat_projection(3,3) = 0; 
}

//-------------------------------------------------------------------------------------------------
void UpdateViewport() { 
	glViewport( 0, 0, context.ScreenWidth(), context.ScreenHeight() );
	SetupProjection( field_of_view, (float)context.ScreenWidth() / (float)context.ScreenHeight(), 1.0, 10000.0 );
}

//-------------------------------------------------------------------------------------------------
void Camera::UpdateVideo() {
	//Eigen::Matrix3f R;
	//R.col(2) = (position-focus).normalized();
	//R.col(0) = orientation.cross(R.col(2)).normalized();
	//R.col(1) = R.col(2).cross(R.col(0));
	
//	mat_view.topLeftCorner<3,3>() = R.transpose();
//	mat_view.topRightCorner<3,1>() = -R.transpose() * position;
//	mat_view(3,3) = 1.0; 

	mat_view.topLeftCorner<3,3>() = rotation.transpose();
	mat_view.topRightCorner<3,1>() = -rotation.transpose() * position;
	mat_view(3,3) = 1.0;

	///mat_view(3,0) = -R.col(0).dot(position);
	/*
	printf( "view matrix dump:\n" );
	printf( "\n\n%8.4f|%8.4f|%8.4f|%8.4f\n%8.4f|%8.4f|%8.4f|%8.4f\n%8.4f|%8.4f|%8.4f|%8.4f\n%8.4f|%8.4f|%8.4f|%8.4f", 
		mat_view(0,0), mat_view(0,1), mat_view(0,2), mat_view(0,3),
		mat_view(1,0), mat_view(1,1), mat_view(1,2), mat_view(1,3),
		mat_view(2,0), mat_view(2,1), mat_view(2,2), mat_view(2,3),
		mat_view(3,0), mat_view(3,1), mat_view(3,2), mat_view(3,3) );*/
		
	if( field_of_view != fov ) {
		SetupProjection( fov, (float)context.ScreenWidth() / (float)context.ScreenHeight(), 1.0, 10000.0 );
	}
	matxp_serial++;
	mat_xp = mat_projection * mat_view;
}

//-------------------------------------------------------------------------------------------------
const Eigen::Matrix4f &GetXPMatrix() {
	return mat_xp;
}
int GetXPMatrixSerial() {
	return matxp_serial;
}

//-------------------------------------------------------------------------------------------------
const Eigen::Matrix4f &GetXPMatrixInverse() {
	return mat_xpi;
}


//-------------------------------------------------------------------------------------------------
void SetBackgroundColor( float r, float g, float b ) {
	bg_color[0] = r;
	bg_color[1] = g;
	bg_color[2] = b;
	fog_color[0] = r;
	fog_color[1] = g;
	fog_color[2] = b;
//	glClearColor( 0.0f,0.0f,1.0f,0.0f );//bgred, bggreen, bgblue, 0.0f );
	glClearColor( bg_color[0], bg_color[1], bg_color[2], 0.0f );
	glFogfv(GL_FOG_COLOR, fog_color);			// Set Fog Color
}

//-------------------------------------------------------------------------------------------------
void UseGlobalSurface() {
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0); 
	glViewport( 0, 0, context.ScreenWidth(), context.ScreenHeight() );
}
  
//------------------------------------------------------------------------------------------------
void Open( int width, int height ) {
	
	static int first_init = 1;
	if( first_init ) {
		SetupConsole(); 

		first_init = 0;
		 
	}
	
	context.Create( width, height );
	
	printf( "OpenGL Version: %s\n", glGetString(GL_VERSION) );

	GLenum err = glewInit(); 
	if( err != GLEW_OK ) {
		throw new std::exception( "Couldn't start GLEW." );
	}
	

	//glEnable( GL_TEXTURE_2D );
	//glShadeModel( GL_SMOOTH );
	glClearDepth( 1.0f );
	glEnable( GL_DEPTH_TEST );
	SetDepthBufferMode( ZBUFFER_ENABLED );
	//glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
	  
	SetCullingMode( CULLMODE_BACK );
	//glEnable(GL_CULL_NONE); 
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	SetBackgroundColor( 255.0f/255.0f, 116.0f/255.0f, 33.0f/255.0f );
	 
	SetBlendMode( BLEND_OPAQUE );

	field_of_view = 50.0; 
	UpdateViewport();
}

void Close() {
	context.Destroy();
}
  
//-------------------------------------------------------------------------------------------------
int ScreenWidth() {
	return context.ScreenWidth();
}

//-------------------------------------------------------------------------------------------------
int ScreenHeight() {
	return context.ScreenHeight();
}

//-------------------------------------------------------------------------------------------------
void Swap() {
	context.Swap();
}

//-------------------------------------------------------------------------------------------------
void BindTextureHandle( GLuint texture ) {
	glBindTexture(GL_TEXTURE_2D, texture );
}

//-------------------------------------------------------------------------------------------------
void BindTextureArrayHandle( GLuint texture ) {
	glBindTexture( GL_TEXTURE_2D_ARRAY, texture );
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/*
cml::vector3f GetCamera() {
	return camera_position;
}*/
/*
//-------------------------------------------------------------------------------------------------
cml::vector3f GetCameraRightVector() {
	return camera_right;
}*/
/*
//-------------------------------------------------------------------------------------------------
cml::vector3f GetCameraForwardVector() {
	return camera_forward;
}*/
/*
//-------------------------------------------------------------------------------------------------
cml::vector3f GetPickRay( cml::vector2f screen_coordinates ) {
	screen_coordinates[0] /= (float)screen_width;
	screen_coordinates[1] /= (float)screen_height;
	cml::vector3f a,b;
	a = far_plane[0] * (1.0-screen_coordinates[0]) + far_plane[3] * screen_coordinates[0];
	b = far_plane[1] * (1.0-screen_coordinates[0]) + far_plane[2] * screen_coordinates[0];
	a = a * (1.0-screen_coordinates[1]) + b * screen_coordinates[1];
	return cml::normalize( a - camera_position );
}*/
/*
//-------------------------------------------------------------------------------------------------
void ViewPunch( cml::vector3f p, float f ) {
	camera_punch = p*f + camera_punch * (1.0f-f);

	//cml::vector3f punch = cameraPunch;
	
	//cameraPosition += cameraRight * punch[0] + cameraUp * punch[1] + cameraForward * punch[2];
}*/
/*
//-------------------------------------------------------------------------------------------------
void SetCameraBob( float offset, float velocity ) {
	
	ViewPunch( cml::vector3f( sin(offset)*velocity/100.0f,sin(offset*2)*velocity/400.0f, 0.0f), 0.4f );
}*/
/*
//-------------------------------------------------------------------------------------------------
void ShakeCamera( float power ) {
	camera_shake_power += power;
	if( camera_shake_power > camera_shake_power_max ) camera_shake_power = camera_shake_power_max;
}

//-------------------------------------------------------------------------------------------------
void FadeCameraShake( float factor ) {
	camera_shake_power2 = camera_shake_power2 * 0.8f + camera_shake_power * 0.2f;
	camera_shake_power *= factor;
	if( camera_shake_power < 0 ) camera_shake_power = 0;
}*/
/*
//-------------------------------------------------------------------------------------------------
float GetCSoffset( float offset ) {
	
	float a = rnd::get_static(offset+(camera_shake_time*0.25f)) * 8.0f;
	a += rnd::get_static(offset+(camera_shake_time*0.5f)) * 4.0f;
	//a += rnd::get_static(offset+(cameraShakeTime*1.0f)) * 2.0f;
	//a += rnd::get_static(offset+(cameraShakeTime*2.0f)) * 1.0f;
	return (a * (1.0f / 12.0f) - 0.5f) * 2.0f;
}*/
/*
//-------------------------------------------------------------------------------------------------
float AddCameraShakeTime( float time ) {
	camera_shake_time += time;
	return camera_shake_time;
}*/
/*
void camera_shit_that_needs_to_be_organized() {
	mat_xp = mat_projection * mat_view;
	
	//glUniformMatrix4fv( Shaders::MS.attribCamera, 1, GL_FALSE, matXP.data() );	
	
	mat_xpi = cml::inverse(mat_xp);

	// compute planes

	cml::vector4f pos[4];
	pos[0].set(-1.0, 1.0, 0.0, 1.0 );
	pos[1].set(-1.0, -1.0, 0.0, 1.0 );
	pos[2].set(1.0, -1.0, 0.0, 1.0 );
	pos[3].set(1.0, 1.0, 0.0, 1.0 );

	// unproject and divide by w

	for( int i = 0; i < 4; i++ ) {
		pos[i] = mat_xpi*pos[i];
		pos[i] = pos[i] / pos[i][3];
	}

	for( int i = 0; i < 4; i++ ) {
		near_plane[i][0] = pos[i][0]; 
		near_plane[i][1] = pos[i][1]; 
		near_plane[i][2] = pos[i][2]; 
	}
	
	pos[0].set(-1.0, 1.0, 1.0, 1.0 );
	pos[1].set(-1.0, -1.0, 1.0, 1.0 );
	pos[2].set(1.0, -1.0, 1.0, 1.0 );
	pos[3].set(1.0, 1.0, 1.0, 1.0 );

	for( int i = 0; i < 4; i++ ) {
		pos[i] = mat_xpi*pos[i];
		pos[i] = pos[i] / pos[i][3];

	}
	
	for( int i = 0; i < 4; i++ ) {
		far_plane[i][0] = pos[i][0]; 
		far_plane[i][1] = pos[i][1]; 
		far_plane[i][2] = pos[i][2]; 
	}
	
}*/
/*
//-------------------------------------------------------------------------------------------------
void SetCamera_LookAt( cml::vector3f position, cml::vector3f target, float roll ) {
	camera_position = position;
	camera_up = cml::vector3f( 0.0f, 1.0f, 0.0f );
	camera_forward = target - position;

	camera_forward.normalize();
	camera_right = cml::cross( camera_forward, camera_up );
	camera_right.normalize();

	cml::matrix_look_at_RH( mat_view, camera_position, target, camera_up );
	camera_shit_that_needs_to_be_organized();
}*/


/*
void CameraLookAt( const Eigen::Vector3f& position, const Eigen::Vector3f& target, const Eigen::Vector3f& up ) {
	Eigen::Matrix3f R;
	R.col(2) = (position-target).normalized();
	R.col(0) = up.cross(R.col(2)).normalized();
	R.col(1) = R.col(2).cross(R.col(0));
	mat_view.topLeftCorner<3,3>() = R.transpose();
	mat_view.topRightCorner<3,1>() = -R.transpose() * position;

	mat_xp = mat_projection * mat_view;
} */
/*
//-------------------------------------------------------------------------------------------------
void SetCamera( float x, float y, float z, float angle, float pitch ) {
	
	camera_position[0] = x;
	camera_position[1] = y;
	camera_position[2] = z;
	
	//cameraForward = 

	//cml::matrix44f_c translate;
	//cml::matrix_translation( translate, -x, -y, -z );
	//cml::identity_transform( matView );
	//cml::matrix_rotate_about_world_y( matView, angle );
	//cml::matrix_rotate_about_world_x( matView, pitch );
	//matView = matView*translate;

	angle += GetCSoffset( 0 ) * camera_shake_power2;
	pitch += GetCSoffset( 100 ) * camera_shake_power2;
	
	camera_forward = cml::vector3f( 0,0,-1);
	camera_forward = cml::rotate_vector( camera_forward, cml::vector3f( 1, 0, 0 ), -pitch );
	camera_forward = cml::rotate_vector( camera_forward, cml::vector3f( 0, 1, 0 ), -angle );

	cml::vector3f rotx = cml::vector3f(camera_forward);
	rotx[1] = 0.0;
	rotx = cml::rotate_vector( rotx, cml::vector3f( 0,1,0), 3.14159/2.0 );
	camera_up = cml::rotate_vector( camera_forward, rotx, -3.14159/2.0 );
	camera_right = cml::cross( camera_forward, camera_up );

	camera_forward.normalize();
	camera_up.normalize();
	camera_right.normalize();

	cml::vector3f poody = camera_position;
	camera_position += camera_punch[0] * camera_right + camera_punch[1] * camera_up + camera_punch[2] * camera_right;
	//cameraUp = cml::vector3f(0,1,0);
	//viewPunch( cml::vector3f( cos(poop)*0.2,-cos(poop*2)*0.05, 0), 1.0 );
	//up = cml::rotate_vector(up,target,poop);

	//sin( gpGlobals->curtime * cl_viewbob_timer.GetFloat() );

	cml::matrix_look_at_RH( mat_view, camera_position, camera_position + camera_forward, camera_up );
	
	camera_shit_that_needs_to_be_organized();
	
}*/

//-------------------------------------------------------------------------------------------------
void SetFogLength( float f ) {
	fog_length = f;
}

//-------------------------------------------------------------------------------------------------
float GetFogLength() {
	return fog_length;
}
 
 
//-------------------------------------------------------------------------------------------------
BlendMode GetBlendMode() {
	return blending_mode;
}

//-------------------------------------------------------------------------------------------------
void SetBlendMode( BlendMode blend_mode ) {
	
	switch( blend_mode ) {
	case BLEND_OPAQUE:
		glDisable( GL_BLEND );
		break;
	case BLEND_ALPHA:
		glEnable( GL_BLEND );
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case BLEND_ADD:
		glEnable( GL_BLEND );
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	case BLEND_SUB:
		
		//glEnable( GL_BLEND );
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// TODO...
		return;
	default:
		return;
	}

	blending_mode = blend_mode;
}

//-------------------------------------------------------------------------------------------------
void SetCullingMode( CullingMode mode ) {
	switch( mode ) {
	case CULLMODE_NONE:
		glDisable( GL_CULL_FACE );
		break;
	case CULLMODE_BACK:
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );
		break;
	case CULLMODE_FRONT:
		glEnable( GL_CULL_FACE );
		glCullFace( GL_FRONT );
	}
	culling_mode = mode;
}

//-------------------------------------------------------------------------------------------------
CullingMode GetCullingMode() {
	return culling_mode;
}
//-------------------------------------------------------------------------------------------------
void SetDepthBufferMode( Video::DepthBufferMode mode ) {
	switch( mode ) {
	case ZBUFFER_DISABLED:
		glDisable( GL_DEPTH_TEST );
		break;
	case ZBUFFER_WRITEONLY:
		//glEnable( GL_DEPTH_TEST );
		// todo
		break;
	case ZBUFFER_READONLY:
		glEnable( GL_DEPTH_TEST );
		glDepthMask(false);
		break;
	case ZBUFFER_ENABLED:
		glEnable( GL_DEPTH_TEST );
		glDepthMask(true);
		break;
	}
	glDepthFunc(  GL_LEQUAL );
}

//-------------------------------------------------------------------------------------------------
void Clear() {
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
}
/*
//-------------------------------------------------------------------------------------------------
float GetCameraDistance( cml::vector3f point ) {
	return sqrt(GetCameraDistance2(point));
}*/
/*
//-------------------------------------------------------------------------------------------------
float GetCameraDistance2( cml::vector3f point ) {
	float x,y,z;
	x = point[0] - camera_position[0];
	y = point[1] - camera_position[1];
	z = point[2] - camera_position[2];
	x = x*x;
	y = y*y;
	z = z*z;
	return x+y+z;
}*/

//-------------------------------------------------------------------------------------------------
void DrawQuads( int start, int size ) {
	glDrawArrays( GL_QUADS, start, size );
}

//-------------------------------------------------------------------------------------------------
void DrawQuadsInstanced( int start, int size, int instances ) {
	glDrawArraysInstanced( GL_QUADS, start, size, instances );
}

//-------------------------------------------------------------------------------------------------
void SetActiveTextureSlot( int slot ) {
	glActiveTexture( GL_TEXTURE0 + slot );
}
/*
//-------------------------------------------------------------------------------------------------
cml::vector3f GetRelativeCameraPoint( float x, float y, float z ) {
	//x,y = screen coordinates
	//z = depth in meters

	z = (z - NearPlaneZ()) / ViewPlanesRange();
	// interpolate z
	cml::vector3f plane[4];
	plane[0] = cml::lerp( near_plane[0], far_plane[0], z );
	plane[1] = cml::lerp( near_plane[1], far_plane[1], z );
	plane[2] = cml::lerp( near_plane[2], far_plane[2], z );
	plane[3] = cml::lerp( near_plane[3], far_plane[3], z );

	plane[0] = cml::lerp( plane[0], plane[1], y );
	plane[3] = cml::lerp( plane[3], plane[2], y );

	plane[0] = cml::lerp( plane[0], plane[3], x );

	return plane[0];

}*/

//-------------------------------------------------------------------------------------------------
/*
void RunWindowLoop( void (*WindowFrameCallback)() ) {
	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );
	while( msg.message != WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        } else {
			WindowFrameCallback();
			Swap();
		}
	}
}*/



void BindArrayBuffer( int id ) {
	// todo: redundancy checks?
	glBindBuffer( GL_ARRAY_BUFFER, id );
}

}

