//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "video.h"
#include "console/console.h"
 
//-----------------------------------------------------------------------------
namespace Video {

using Eigen::Matrix4f;

Instance *g_instance = nullptr;

//-----------------------------------------------------------------------------
Instance::Instance() : Module( "video", Levels::SUBSYSTEM ) {
	assert( g_instance == nullptr );
	g_instance = this;
}

//-----------------------------------------------------------------------------
Instance::~Instance() {
	g_instance = nullptr;
}

//-----------------------------------------------------------------------------
void Instance::Open( int width, int height ) {
	Close();

	m_screen_width  = width;
	m_screen_height = height;

	m_window = SDL_CreateWindow( "CLIENT", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		width, height, SDL_WINDOW_OPENGL );

	if( !m_window ) {
		throw std::exception( "Window creation error." );
	}

	m_gl_context = SDL_GL_CreateContext( m_window );
	
	Console::Print( "OpenGL Version: %s\n", glGetString(GL_VERSION) );
	
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

	m_fov = 50.0;
	UpdateViewport();
}

//-----------------------------------------------------------------------------
void Instance::Close() {
	if( m_gl_context ) {
		SDL_GL_DeleteContext( m_gl_context );
		m_gl_context = nullptr;
	}

	if( m_window ) {
		SDL_DestroyWindow( m_window );
		m_window = nullptr;
	}
}

//-----------------------------------------------------------------------------
void Instance::Swap() {
	SDL_GL_SwapWindow( m_window );
}

//-----------------------------------------------------------------------------
void Instance::UseGlobalSurface() {
	glBindFramebuffer( GL_FRAMEBUFFER_EXT, 0 ); 
	glViewport( 0, 0, m_screen_width, m_screen_height );
}

//-----------------------------------------------------------------------------
void Instance::SetBackgroundColor( float r, float g, float b ) {
	m_bg_color[0] = r;
	m_bg_color[1] = g;
	m_bg_color[2] = b;
	m_fog_color[0] = r;
	m_fog_color[1] = g;
	m_fog_color[2] = b;

	glClearColor( m_bg_color[0], m_bg_color[1], m_bg_color[2], 0.0f );

	// i don't think this is used anymore
	glFogfv( GL_FOG_COLOR, m_fog_color );
}

//-----------------------------------------------------------------------------
void Instance::BindTextureHandle( GLuint texture ) {
	glBindTexture( GL_TEXTURE_2D, texture );
}

//-----------------------------------------------------------------------------
void Instance::BindTextureArrayHandle( GLuint texture ) {
	glBindTexture( GL_TEXTURE_2D_ARRAY, texture );
}

//-----------------------------------------------------------------------------
void Instance::SetFogLength( float length ) {
	m_fog_length = length;
}

//-----------------------------------------------------------------------------
void Instance::SetBlendMode( BlendMode mode ) {
	if( m_blendmode == mode ) return;

	switch( mode ) {

	case BLEND_OPAQUE:
		glDisable( GL_BLEND );
		break;

	case BLEND_ALPHA:
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		break;

	case BLEND_ADD:
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
		break;

	case BLEND_SUB:
		//glEnable( GL_BLEND );
		//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		// TODO...
		return;

	default:
		return;
	}

	m_blendmode = mode;
}

//-----------------------------------------------------------------------------
void Instance::SetCullingMode( CullingMode mode ) {
	if( m_cullmode == mode ) return;

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
	m_cullmode = mode;
}

//-----------------------------------------------------------------------------
void Instance::SetDepthBufferMode( DepthBufferMode mode ) {
	if( m_depthmode == mode ) return;

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
		glDepthMask( false );
		break;

	case ZBUFFER_ENABLED:
		glEnable( GL_DEPTH_TEST );
		glDepthMask( true );
		break;
	}
	glDepthFunc(  GL_LEQUAL );

	m_depthmode = mode;
}

//-----------------------------------------------------------------------------
void Instance::Clear() {
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
}

//-----------------------------------------------------------------------------
void Instance::DrawQuads( int start, int size ) {
	glDrawArrays( GL_QUADS, start, size );
}

//-----------------------------------------------------------------------------
void Instance::DrawQuadsInstanced( int start, int size, int instances ) {
	glDrawArraysInstanced( GL_QUADS, start, size, instances );
}

//-----------------------------------------------------------------------------
void Instance::SetActiveTextureSlot( int slot ) {
	glActiveTexture( GL_TEXTURE0 + slot );
}

//-----------------------------------------------------------------------------
void Instance::BindArrayBuffer( GLuint buffer ) {
	// todo: redundancy checks?
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
}

//-----------------------------------------------------------------------------
float Instance::NearPlaneZ() {
	return 0.5f;
}

//-----------------------------------------------------------------------------
float Instance::FarPlaneZ() {
	return m_view_distance;
}

//-----------------------------------------------------------------------------
float Instance::ViewPlanesRange() {
	return m_view_distance - NearPlaneZ();
}

//-----------------------------------------------------------------------------
void Instance::SetupProjection( float fovY, float aspect, 
								float fnear, float ffar ) {
	m_fov = fovY;
	float theta = fovY*0.5f;
	float range = ffar - fnear;
	float invtan = 1.0f/tan(theta);
	
	m_mat_projection(0,0) = invtan / aspect;
	m_mat_projection(1,1) = invtan;
	m_mat_projection(2,2) = -(fnear + ffar) / range;
	m_mat_projection(3,2) = -1;
	m_mat_projection(2,3) = -2 * fnear * ffar / range;
	m_mat_projection(3,3) = 0;
}

//-----------------------------------------------------------------------------
void Instance::UpdateViewport() { 
	glViewport( 0, 0, m_screen_width, m_screen_height );
	SetupProjection( 
		m_fov,
		(float)m_screen_width / (float)m_screen_height,
		1.0, 10000.0 );
}

//-----------------------------------------------------------------------------
void Instance::CopyCamera( Camera &cam ) {

	//Eigen::Matrix3f R;
	//R.col(2) = (position-focus).normalized();
	//R.col(0) = orientation.cross(R.col(2)).normalized();
	//R.col(1) = R.col(2).cross(R.col(0));
	
//	mat_view.topLeftCorner<3,3>() = R.transpose();
//	mat_view.topRightCorner<3,1>() = -R.transpose() * position;
//	mat_view(3,3) = 1.0; 

	m_mat_view.topLeftCorner<3,3>() = cam.m_rotation.transpose();
	m_mat_view.topRightCorner<3,1>() = 
			-cam.m_rotation.transpose() * cam.m_position;

	m_mat_view(3,3) = 1.0;

	///mat_view(3,0) = -R.col(0).dot(position);
	/*
	printf( "view matrix dump:\n" );
	printf( "\n\n%8.4f|%8.4f|%8.4f|%8.4f\n%8.4f|%8.4f|%8.4f|%8.4f\n%8.4f|%8.4f|%8.4f|%8.4f\n%8.4f|%8.4f|%8.4f|%8.4f", 
		mat_view(0,0), mat_view(0,1), mat_view(0,2), mat_view(0,3),
		mat_view(1,0), mat_view(1,1), mat_view(1,2), mat_view(1,3),
		mat_view(2,0), mat_view(2,1), mat_view(2,2), mat_view(2,3),
		mat_view(3,0), mat_view(3,1), mat_view(3,2), mat_view(3,3) );*/
		
	if( m_fov != cam.m_fov ) {
		SetupProjection( cam.m_fov, (float)m_screen_width / (float)m_screen_height, 1.0, 10000.0 );
	}

	m_mat_xp = m_mat_projection * m_mat_view;
	m_matxp_serial++;
}

//-----------------------------------------------------------------------------

 /* TODO move to camera shake dongle
//-------------------------------------------------------------------------------------------------
float camera_shake;
float camera_shake_time;
float camera_shake_power;
float camera_shake_power2;

static const float camera_shake_power_max = 1.0;
*/
//-------------------------------------------------------------------------------------------------
 
//-------------------------------------------------------------------------------------------------
   
  
//-------------------------------------------------------------------------------------------------

Eigen::Matrix4f mat_xpi;


//GLint current_vertex_buffer;
  
//-------------------------------------------------------------------------------------------------
Camera::Camera() {
	m_rotation.setIdentity();
	m_fov = 45.0;
	m_fov_dirty = false;
}

//-------------------------------------------------------------------------------------------------
Camera::~Camera() {}

//-------------------------------------------------------------------------------------------------
void Camera::SetFOV( float fov ) {
	if( m_fov != fov ) {
		m_fov = fov;
		m_fov_dirty = true;
	}
}

//-------------------------------------------------------------------------------------------------
void Camera::SetPosition( const Eigen::Vector3f &vec_position ) {
	m_position = vec_position;
}

//-----------------------------------------------------------------------------
void Camera::Move( const Eigen::Vector3f &add_to_position ) {
	m_position += add_to_position;
}

//-----------------------------------------------------------------------------
void Camera::LookAt( const Eigen::Vector3f &target, 
	                 const Eigen::Vector3f &up ) {
		
	m_rotation.col(2) = (m_position-target).normalized();
	m_rotation.col(0) = up.cross(m_rotation.col(2)).normalized();
	m_rotation.col(1) = m_rotation.col(2).cross(m_rotation.col(0));
}

//-----------------------------------------------------------------------------
void Camera::Fixup() {

	// x axis from cross of forward and up
	m_rotation.col(0) = 
			m_rotation.col(1).cross( m_rotation.col(2) ).normalized();

	// y axis from cross of forward and right
	m_rotation.col(1) = 
			m_rotation.col(2).cross( m_rotation.col(0) ).normalized();

	// and just normalize z axis
	m_rotation.col(2).normalize();
}

//-----------------------------------------------------------------------------
void Camera::Rotate( const Eigen::Vector3f &angles ) {
	if( angles[0] != 0.0f ) {
		Eigen::AngleAxisf rot( angles[0], m_rotation.col(0) );
		m_rotation = rot * m_rotation;
	}
	if( angles[1] != 0.0f ) {
		Eigen::AngleAxisf rot( angles[1], m_rotation.col(1) );
		m_rotation = rot * m_rotation;
	}
	if( angles[2] != 0.0f ) {
		Eigen::AngleAxisf rot( angles[2], m_rotation.col(2) );
		m_rotation = rot * m_rotation;
	}
}

//-------------------------------------------------------------------------------------------------
void Camera::UpdateVideo() {
	g_instance->CopyCamera( *this );
	
}

 
//----------------------------------------------------------------------------------------------------------------------------
// instance bindings
//
void            SetFogLength( float a )                         { g_instance->SetFogLength( a );             }
float           GetFogLength()                                  { return g_instance->GetFogLength();         }
BlendMode       GetBlendMode()                                  { return g_instance->GetBlendMode();         }
void            SetBlendMode( BlendMode a )                     { g_instance->SetBlendMode( a );             }
void            SetCullingMode( CullingMode a )                 { g_instance->SetCullingMode( a );           }
CullingMode     GetCullingMode()                                { return g_instance->GetCullingMode();       }
void            SetDepthBufferMode( DepthBufferMode a )         { g_instance->SetDepthBufferMode( a );       }
DepthBufferMode GetDepthBufferRMode()                           { return g_instance->GetDepthBufferMode();   }
void            Clear()                                         { g_instance->Clear();                       }
void            DrawQuads( int a, int b )                       { g_instance->DrawQuads( a, b );             }
void            DrawQuadsInstanced( int a, int b, int c )       { g_instance->DrawQuadsInstanced( a, b, c ); }
void            SetActiveTextureSlot( int a )                   { g_instance->SetActiveTextureSlot( a );     }
void            BindArrayBuffer( GLuint a )                     { g_instance->BindArrayBuffer( a );          }
void            SetBackgroundColor( float r, float g, float b ) { g_instance->SetBackgroundColor( r, g, b ); }
void            UseGlobalSurface()                              { g_instance->UseGlobalSurface();            }
void            Open( int a, int b )                            { g_instance->Open( a, b );                  }
void            Close()                                         { g_instance->Close();                       }
int             ScreenWidth()                                   { return g_instance->ScreenWidth();          }
int             ScreenHeight()                                  { return g_instance->ScreenHeight();         }
void            Swap()                                          { g_instance->Swap();                        }
void            BindTextureHandle( GLuint a )                   { g_instance->BindTextureHandle( a );        }
void            BindTextureArrayHandle( GLuint a )              { g_instance->BindTextureArrayHandle( a );   }
float           NearPlaneZ()                                    { return g_instance->NearPlaneZ();           }
float           FarPlaneZ()                                     { return g_instance->FarPlaneZ();            }
float           ViewPlanesRange()                               { return g_instance->ViewPlanesRange();      }
const Matrix4f  &GetXPMatrix()                                  { return g_instance->GetXPMatrix();          }
int             GetXPMatrixSerial()                             { return g_instance->GetXPMatrixSerial();    }

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
//-----------------------------------------------------------------------------
float GetCSoffset( float offset ) {
	
	float a = rnd::get_static(offset+(camera_shake_time*0.25f)) * 8.0f;
	a += rnd::get_static(offset+(camera_shake_time*0.5f)) * 4.0f;
	//a += rnd::get_static(offset+(cameraShakeTime*1.0f)) * 2.0f;
	//a += rnd::get_static(offset+(cameraShakeTime*2.0f)) * 1.0f;
	return (a * (1.0f / 12.0f) - 0.5f) * 2.0f;
}*/
/*
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
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


}

