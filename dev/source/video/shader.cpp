//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "video/shader.h"
#include "video/video.h"

//-----------------------------------------------------------------------------
namespace Video { 

//-----------------------------------------------------------------------------
Shader *Instance::GetActiveShader() {
	return m_active_shader;
}

//-----------------------------------------------------------------------------
Shader *Instance::FindShader( const Stref &name ) {

	try {
		return m_shaders.at( name ).get();
	} catch( const std::out_of_range & ) {}

	return nullptr;
}

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
	boost::scoped_array<char> log( new char[log_length] );
	if( program )
		glGetProgramInfoLog( shader, log_length, NULL, log.get() );
	else
		glGetShaderInfoLog( shader, log_length, NULL, log.get() );
	std::cerr << log.get() << std::endl; 
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
	if (!shader_ok) {
		std::cerr << "Failed to compile " << filename << ":\n"; 
		DumpInfoLog(shader, false); 
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

//-------------------------------------------------------------------------------------------------
void Shader::AddShader( const Stref &filename, GLenum type ) {
	GLuint s = Compile( filename, type );
	if( s ) {
		shaders.push_back(s);
	}
}

//-------------------------------------------------------------------------------------------------
void Shader::EnableVertexAttributes() {
	for( auto i : attributes ) { 
		glEnableVertexAttribArray( i ); 
	}
	
}

//-------------------------------------------------------------------------------------------------
void Shader::DisableVertexAttributes() {
	for( auto i : attributes ) { 
		glDisableVertexAttribArray( i ); 
	}
}

//-------------------------------------------------------------------------------------------------
void Shader::SetVertexAttributePointers( int offset, int set ) {
	// virtual only
}
 
//-------------------------------------------------------------------------------------------------
void Shader::Link() {

	program = glCreateProgram();
	
	glBindFragDataLocation( program, 0, "fragment_color" );

	for( auto i : shaders ) {// boost::uint32_t i = 0; i < shaders.size(); i++ ) {
		glAttachShader( program, i );//shaders[i] );
	}
	glLinkProgram(program);

	GLint program_ok;
	glGetProgramiv( program, GL_LINK_STATUS, &program_ok );
	if( !program_ok ) {
		std::cerr << "Failed to link shader program: " << std::endl;
		DumpInfoLog( program, true );
	}
	
	vertex_state.Create();
	vertex_state.Bind();
	// find variables
	for( auto sv : variables ) { 
			
		if( sv.type == Variable::UNIFORM ) {
			*(sv.target) = glGetUniformLocation( ProgramID(), sv.name );
			//uniforms.push_back( uniform_id ); not used.
		} else if( sv.type == Variable::ATTRIBUTE ) {
			GLint attribute_id = glGetAttribLocation( ProgramID(), sv.name );
			*(sv.target) = attribute_id;
			for( int i = 0; i < sv.matrix; i++ ) {
			
				attributes.push_back( attribute_id+i );
				glEnableVertexAttribArray( attribute_id+i );
		//		glVertexAttribDivisor( attribute_id+i, sv.divisor ); debug bypass
			}
		}
	}
	variables.clear();
	
	//Use();
}

//-------------------------------------------------------------------------------------------------
void Shader::AddAttribute( GLint &target, const Stref &name, int divisor, int matrix ) {

	variables.push_back( Variable( Variable::ATTRIBUTE, target, name, divisor, matrix ) ); 
}

//-------------------------------------------------------------------------------------------------
void Shader::AddUniform( GLint &target, const Stref &name ) {

	variables.push_back( Variable( Variable::UNIFORM, target, name ) ); 
}

//-------------------------------------------------------------------------------------------------
void Shader::Use() {
	if( Shaders::active == this ) {
		SetCamera();
		return;
	}

	glUseProgram(program);
	
	vertex_state.Bind();
	SetCamera();

	Shaders::active = this;
}

//-------------------------------------------------------------------------------------------------
GLuint Shader::ProgramID() {
	return program;
}

//-------------------------------------------------------------------------------------------------
Shader::Shader( const char *name ) {
	program = -1;
	Util::CopyString( id, name ); 
}

//-------------------------------------------------------------------------------------------------
Shader::Shader() {
	throw std::exception( "creating shader without name" );
}

//-------------------------------------------------------------------------------------------------
Shader::~Shader() {
	// todo: is program 0 valid?
	if( Shaders::active == this ) 
		Shaders::active = 0;

	if( program != -1 ) {
		glDeleteProgram( program );
	}
	for( auto i : shaders ) {//boost::uint32_t i = 0; i < shaders.size(); i++ ) {
		glDeleteShader( i ); //shaders[i] );
	}

}

//-------------------------------------------------------------------------------------------------
int Shader::Register() { 
	
	int index = Shaders::table.Find( id );
	if( index != -1 ) return index;
	table_index = Shaders::table.Add( id );
	Shaders::pointers.push_back( this );
	return table_index;
}

//-------------------------------------------------------------------------------------------------
boost::shared_ptr<Shader::Kernel> Shader::Kernel::Create() { 
	// virtual only

	throw std::runtime_error( "Creating kernel from shader base." );
	return nullptr;
}

boost::shared_ptr<Shader::Kernel> Shader::CreateKernel() { 
	
	throw std::runtime_error( "Creating kernel from shader base." );
	return nullptr;
}

//-------------------------------------------------------------------------------------------------
void Shader::Kernel::AddParam( const char *name, Setfunc on_set ) {
	bool result = set_trie.Set( name, on_set, false );
	assert( result ); // false=duplicate parameter key
}

//-------------------------------------------------------------------------------------------------
void Shader::Kernel::SetParam( const char *name, const char *value ) {
	Setfunc func;
	if( !set_trie.Get( name, func ) ) {
		// todo: print error to user
		std::cerr << "Tried to set unknown param \"" << name << "\" -> \""<<value<<"\"." << std::endl;
		return;
	}
	func( value );
}

//-----------------------------------------------------------------------------
Shader::Kernel::Kernel() {
	ResetToDefault();
}

//-----------------------------------------------------------------------------
Shader::Kernel::~Kernel() {}

//-------------------------------------------------------------------------------------------------



} 
