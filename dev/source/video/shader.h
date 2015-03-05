//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once
 
//-------------------------------------------------------------------------------------------------
#include "util/Trie.h"
#include "util/stringtable2.h"
#include "util/stringles.h"
#include "util/fopen2.h"
#include "util/stringref.h"
#include "video/VertexArrayObject.h"

//-------------------------------------------------------------------------------------------------
namespace Video {  

//-------------------------------------------------------------------------------------------------
class Shader {

public:
	//---------------------------------------------------------------------------
	class Kernel {
		typedef boost::function< void(const char *) > Setfunc;
		Util::Trie<Setfunc> set_trie;

	protected:
		void AddParam( const char *name, Setfunc on_set );

	public:

		void SetParam( const char *name, const char *value );

		virtual void ResetToDefault() {};

		virtual boost::shared_ptr<Kernel> Create();

		Kernel() {
			ResetToDefault();
		} 
	
	};

private:
	//---------------------------------------------------------------------------
	struct Variable {
		// a single 
		enum Type {
			ATTRIBUTE=0,
			UNIFORM=1
		} type;

		char  name[64];
		short divisor; // attribute only
		short matrix;  // attribute only
		GLint *target;
		 
		Variable( Type ptype, GLint &ptarget, const Stref &pname, 
			      short pdivisor=0, short pmatrix=1 ) {

			type = ptype;
			Util::CopyString( name, *pname );
			divisor = pdivisor;
			matrix = pmatrix;
			target = &ptarget;
		} 
	}; 

	//---------------------------------------------------------------------------
	class SourceFile {
		size_t m_length;
		boost::scoped_array<char> m_contents;

	public:
		SourceFile( const Stref &filename );

		size_t Length() const {
			return m_length;
		}
		

		const char *Contents() const {
			return m_contents.get();
		}
	};
	 
	std::vector<GLuint>   shaders;		// list of compiled shaders
	std::vector<Variable> variables; 
	std::vector<GLint>    attributes;	// list of vertex attributes
	//std::vector<GLint>  uniforms;		// list of shader uniform variables (not used)
	GLuint program;						// program ID of shader

	VertexArrayObject vertex_state;		// VAO to hold the state for this shader

	char id[32];
	int  table_index;

	//static std::shared_ptr<char[]> ReadFile( const Stref &filename, 
	//										 int &length );

	static void DumpInfoLog( GLuint shader, bool program );

protected: 
	 
	//---------------------------------------------------------------------------
	// compile a shader file and add it to this program
	//
	void AddShader( const Stref &filename, GLenum type );

	//---------------------------------------------------------------------------
	// link the shader program
	//
	void Link();
	
	//---------------------------------------------------------------------------
	// register a shader attribute
	//
	void AddAttribute( GLint &target, const Stref &name, 
					   int divisor = 0, int matrix = 1 );
	
	//---------------------------------------------------------------------------
	// register a shader uniform
	//
	void AddUniform( GLint &target, const Stref &name );

	//---------------------------------------------------------------------------
	// register this shader
	//
	int Register();
	
	//---------------------------------------------------------------------------
	// enable/disable the vertex attributes for this shader
	// under normal conditions these aren't used; they are used once
	// during initialization and the state is saved in a vao
	void EnableVertexAttributes();
	void DisableVertexAttributes();
	
	//---------------------------------------------------------------------------
	// read the opengl program ID
	GLuint ProgramID();
	 
	//---------------------------------------------------------------------------
	// compile a shader file
	static GLuint Compile( const Stref &filename, GLenum type );

	Shader( const char *name );
	Shader();

public:
	~Shader();

	//---------------------------------------------------------------------------
	// activate this shader program
	void Use();

	//---------------------------------------------------------------------------
	// setup the vertex attribute pointers for the current vertex buffer
	// offset = byte offset into the buffer where the data starts
	// set = shader specific, selection of vertex attributes to affect
	virtual void SetVertexAttributePointers( int offset = 0, int set = 0 );
	
	//---------------------------------------------------------------------------
	// copy camera from video module
	//
	virtual void SetCamera() {};
	//virtual int GetParam( const char *name );

	// is this shader active
	//
	bool Active();
	 
	virtual void LoadKernel( Kernel &kernel ) {}; 
	virtual boost::shared_ptr<Shader::Kernel> CreateKernel();
};


namespace Shaders {
	//-------------------------------------------------------------------------------------------------
	// get the active shader
	//
	Shader *GetActive();

	//-------------------------------------------------------------------------------------------------
	// find a registered shader from a keystring 
	//
	Shader *Find( const Stref &name );
	int FindIndex( const Stref &name );

	// get a shader instance from its registered index
	Shader *Get( int index );
}

} 
 