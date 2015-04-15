//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once
 
//-----------------------------------------------------------------------------
#include "util/trie.h"
#include "util/stringtable2.h"
#include "util/stringles.h"
#include "util/fopen2.h"
#include "util/stref.h"
#include "video/vertexarrayobject.h"

//-----------------------------------------------------------------------------
namespace Video {  

/** ---------------------------------------------------------------------------
 * A Shader controls a gpu "program".
 */
class Shader {

public:
	
	/** -----------------------------------------------------------------------
	 * A kernel is used to save the shader's state.
	 */
	class Kernel {
		friend class Shader;

		typedef boost::function< void(const char *) > Setfunc;
		Util::Trie<Setfunc> set_trie;

		virtual boost::shared_ptr<Kernel> Create();

	protected:
		void AddParam( const char *name, Setfunc on_set );

	public:

		/** -------------------------------------------------------------------
		 * Set a variable.
		 *
		 * @param name  Name of variable.
		 * @param value Value to set the variable to. TODO define syntax for various types.
		 */
		void SetParam( const Stref &name, const Stref &value );

		/** -------------------------------------------------------------------
		 * Reset/erase this kernel's settings, returning all variables to
		 * the values defined by the shader's implementation.
		 */
		virtual void ResetToDefault() {};
		
		Kernel();
		virtual ~Kernel();
	};

private:

	/** -----------------------------------------------------------------------
	 * Variables represent a single uniform variable or attribute in the
	 * shader program.
	 */
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

	/** -----------------------------------------------------------------------
	 * A SourceFile is just a file reader and a buffer, for passing
	 * to the shader compiler.
	 */
	class SourceFile {
		int m_length;
		boost::scoped_array<char> m_contents;

	public:
		/** -------------------------------------------------------------------
		 * Read a file.
		 *
		 * @param filename Path to file.
		 */
		SourceFile( const Stref &filename );

		/** -------------------------------------------------------------------
		 * @returns the length of the contents.
		 */
		int Length() const {
			return m_length;
		}
		
		/** -------------------------------------------------------------------
		 * @returns the text data.
		 */
		const char *Contents() const {
			return m_contents.get();
		}
	};
	 
	// list of compiled shaders.
	std::vector<GLuint>   m_shaders;

	// list of shader variables.
	std::vector<Variable> m_variables; 

	// list of attributes, we keep this as a separate list for
	// easy access when enabling/disabling the arrays.
	std::vector<GLint>    m_attributes;
	
	// program ID of shader
	GLuint m_program;

	// VAO to hold the state for this shader
	VertexArrayObject m_vertex_state;

	// Name of this shader.
	std::string m_name;
	 

	// debug dump function
	static void DumpInfoLog( GLuint shader, bool program );
	
	/** -----------------------------------------------------------------------
	 * Compile a shader file.
	 * 
	 * @param filename Path to shader source.
	 * @param type     Type of shader being compiled.
	 *
	 * @returns OpenGL result.
	 */
	static GLuint Compile( const Stref &filename, GLenum type );

protected: 
	// (used by implementation only)
	 
	/** -----------------------------------------------------------------------
	 * Compile a shader file and add it to this program.
	 *
	 * @param filename Path to shader source file.
	 * @param type     The type of shader being compiled, 
	 *                 may be GL_VERTEX_SHADER, or GL_FRAGMENT_SHADER
	 */
	void AddShader( const Stref &filename, GLenum type );

	/** -----------------------------------------------------------------------
	 * Link the shader program. This is done after all source files have
	 * been compiled successfully.
	 */
	void Link();
	
	/** -----------------------------------------------------------------------
	 * Register a shader "attribute".
	 *
	 * This should only be used during initalization.
	 *
	 * @param target  Member variable to store the index for later access.
	 * @param name    Name of the variable in the shader.
	 * @param divisor Divisor used for instanced rendering.
	 * @param matrix  Number of rows the variable has (if it is a matrix)
	 */
	void AddAttribute( GLint &target, const Stref &name, 
					   int divisor = 0, int matrix = 1 );
	
	/** -----------------------------------------------------------------------
	 * Register a shader uniform variable.
	 *
	 * This should only be used during initalization.
	 *
	 * @param target  Member variable to store the index for later access.
	 * @param name    Name of the variable in the shader.
	 */
	void AddUniform( GLint &target, const Stref &name );

	/** -----------------------------------------------------------------------
	 * Enable or disable the vertex attributes for this shader.
	 *
	 * Under normal conditions these aren't used. They are used
	 * during initialization and the state is saved in a VAO.
	 */
	void EnableVertexAttributes();
	void DisableVertexAttributes();
	
	/** -----------------------------------------------------------------------
	 * @returns the OpenGL program ID.
	 */
	GLuint ProgramID() { return m_program; }
	 
	/** -----------------------------------------------------------------------
	 * Initialize.
	 *
	 * @param name Name of shader used for registration.
	 */
	Shader( const Stref &name );
	Shader();

public:
	virtual ~Shader();

	/** -----------------------------------------------------------------------
	 * Activate this shader program.
	 */
	void Use();

	/** -----------------------------------------------------------------------
	 * Setup the vertex attribute pointers for a vertex buffer that is bound.
	 *
	 * @param offset Byte offset into the buffer where the data starts.
	 * @param set    (shader defined) Selection of vertex attributes to
	 *               affect.
	 */
	virtual void SetVertexAttributePointers( int offset = 0, int set = 0 );
	
	/** -----------------------------------------------------------------------
	 * Copy camera settings from video module. (projection/modelview matrix)
	 */
	virtual void SetCamera() {};

	/** -----------------------------------------------------------------------
	 * @returns true if this shader is active.
	 */
	bool Active();
	 
	/** -----------------------------------------------------------------------
	 * Setup the shader's state from a kernel.
	 */
	virtual void LoadKernel( Kernel &kernel ) {}; 

	/** -----------------------------------------------------------------------
	 * Create a shader kernel. Kernels are used to store the state of
	 * all of the shader's variables.
	 */
	virtual boost::shared_ptr<Shader::Kernel> CreateKernel();
};

} 
 