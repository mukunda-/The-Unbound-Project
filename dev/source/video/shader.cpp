//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "video/shader.h"
#include "video/video.h"
#include "console/console.h"

//-----------------------------------------------------------------------------
namespace Video { 

extern Instance *g_instance;

//-----------------------------------------------------------------------------
Shader::SourceFile::SourceFile( const Stref &filename ) {
 
	// open file, make sure it's there
	std::ifstream file( filename, std::ios::binary );
	if( !file ) { 
		throw std::runtime_error( 
			std::string("Missing shader source file: ") + *filename ); 
	}

	// get file size
	file.seekg( 0, std::ios::end );
	m_length = (int)file.tellg();
	file.seekg( 0, std::ios::beg );

	// allocate buffer, add an extra null terminator
	m_contents.reset( new char[m_length+1] );
	file.read( m_contents.get(), m_length );
	m_contents[m_length] = 0;
}
 
//-----------------------------------------------------------------------------
void Shader::DumpInfoLog( GLuint shader, bool program ) {
	GLint log_length;

	if( program )
		glGetProgramiv( shader, GL_INFO_LOG_LENGTH, &log_length );
	else 
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &log_length );

	std::unique_ptr<char[]> log( new char[log_length] );

	if( program )
		glGetProgramInfoLog( shader, log_length, NULL, log.get() );
	else
		glGetShaderInfoLog( shader, log_length, NULL, log.get() );

	Console::PrintErr( "%s", log.get() );
}

//-----------------------------------------------------------------------------
GLuint Shader::Compile( const Stref &filename, GLenum type ) {

	GLuint shader;
	SourceFile source( filename );
	const GLchar *strings[1] = {source.Contents()}; 
	GLint length = source.Length(); 
	shader = glCreateShader(type);
	glShaderSource( shader, 1, strings, &length ); 
	glCompileShader( shader );

	GLint shader_ok;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
	if( !shader_ok ) {
		Console::PrintErr( "Failed to compile \"%s\"\n", filename ); 
		DumpInfoLog( shader, false ); 
		glDeleteShader( shader );
		return 0;
	}
	return shader;
}

//-----------------------------------------------------------------------------
void Shader::AddShader( const Stref &filename, GLenum type ) {
	GLuint s = Compile( filename, type );
	if( s ) {
		m_shaders.push_back(s);
	}
}

//-----------------------------------------------------------------------------
void Shader::EnableVertexAttributes() {
	for( auto i : m_attributes ) { 
		glEnableVertexAttribArray( i ); 
	}
	
}

//-----------------------------------------------------------------------------
void Shader::DisableVertexAttributes() {
	for( auto i : m_attributes ) { 
		glDisableVertexAttribArray( i ); 
	}
}
 
//-----------------------------------------------------------------------------
void Shader::Link() {

	m_program = glCreateProgram();
	
	glBindFragDataLocation( m_program, 0, "fragment_color" );

	for( auto i : m_shaders ) {
		glAttachShader( m_program, i );
	}
	glLinkProgram( m_program );

	GLint program_ok;
	glGetProgramiv( m_program, GL_LINK_STATUS, &program_ok );
	if( !program_ok ) {
		
		std::cerr << "Failed to link shader program: " << std::endl;
		DumpInfoLog( m_program, true );
	}
	
	m_vertex_state.Create();
	m_vertex_state.Bind();

	// find variables
	for( auto sv : m_variables ) { 
			
		if( sv.type == Variable::UNIFORM ) {
			sv.target = glGetUniformLocation( ProgramID(), sv.name );
			//uniforms.push_back( uniform_id ); not used.
		} else if( sv.type == Variable::ATTRIBUTE ) {
			GLint attribute_id = glGetAttribLocation( ProgramID(), sv.name );
			sv.target = attribute_id;
			for( int i = 0; i < sv.matrix; i++ ) {
			
				m_attributes.push_back( attribute_id+i );
				glEnableVertexAttribArray( attribute_id+i );

				// TODO: disable the rest of the arrays??

		//		glVertexAttribDivisor( attribute_id+i, sv.divisor ); debug bypass
			}
		}
	}
	m_variables.clear();
}

//-----------------------------------------------------------------------------
void Shader::AddAttribute( int &target, const Stref &name, 
	                       int divisor, int matrix ) {

	m_variables.push_back(
		Variable( Variable::ATTRIBUTE, target, name, divisor, matrix )); 
}

//-----------------------------------------------------------------------------
void Shader::AddUniform( GLint &target, const Stref &name ) {

	m_variables.push_back( Variable( Variable::UNIFORM, target, name )); 
}

//-----------------------------------------------------------------------------
void Shader::Use() {
	if( g_instance->m_active_shader != this ) {

		glUseProgram( m_program );
		m_vertex_state.Bind();

		return;
	}

	SetCamera();
	g_instance->m_active_shader = this;
}

//-----------------------------------------------------------------------------
Shader::Shader( const Stref &name ) {
	m_program = -1;
	m_name = name;
}

//-----------------------------------------------------------------------------
Shader::~Shader() {

	// todo: is program 0 valid?
	if( g_instance->m_active_shader == this ) {
		g_instance->m_active_shader = nullptr;
	}

	if( m_program != -1 ) {
		glDeleteProgram( m_program );
	}

	for( auto i : m_shaders ) {
		glDeleteShader( i );
	}
}

//-----------------------------------------------------------------------------
void Shader::KernelMap::AddParam( const Stref &name, Setfunc on_set ) {
	bool result = m_setters.Set( name, on_set, false );

	if( !result ) {
		Console::PrintErr( "Trying to add duplicate kernel parameter \"%s\"",
						   name );
	}
}

//-----------------------------------------------------------------------------
void Shader::Kernel::SetParam( const Stref &name, const Stref &value ) {

	auto func = m_map->Get( name );

	if( func ) {
		//std::bind( func, this, _1 )( value );

		(this->*func)( value );
	} else {
		Console::PrintErr( 
			"Tried to set unknown param \"%s\" -> \"%s\".\n", name, value );
	}
}

//-----------------------------------------------------------------------------
auto Shader::KernelMap::Get( const Stref &name ) const -> Setfunc {
	
	Setfunc func;
	if( m_setters.Get( name, func ) ) {
		return func;
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
Shader::Kernel::Kernel( std::shared_ptr<const KernelMap> &map ) {
	m_map = map;
	ResetToDefault();
}

//-----------------------------------------------------------------------------
Shader::Kernel::~Kernel() {}

//-----------------------------------------------------------------------------



} 
