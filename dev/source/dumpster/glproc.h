//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
/*

#ifndef GLPROC_H
#define GLPROC_H

#ifndef _WIN32
#define GL_GLEXT_PROTOTYPES
#endif
#include "glext.h"

#ifdef _WIN32

//
// VBO
//

#define glGenBuffersARB           pglGenBuffersARB
#define glBindBufferARB           pglBindBufferARB
#define glBufferDataARB           pglBufferDataARB
#define glBufferSubDataARB        pglBufferSubDataARB
#define glDeleteBuffersARB        pglDeleteBuffersARB
#define glGetBufferParameterivARB pglGetBufferParameterivARB
#define glMapBufferARB            pglMapBufferARB
#define glUnmapBufferARB          pglUnmapBufferARB
	
#define glGenRenderbuffersEXT			pglGenRenderbuffersEXT
#define glBindRenderbufferEXT			pglBindRenderbufferEXT
#define glRenderbufferStorageEXT		pglRenderbufferStorageEXT
#define glGenFramebuffersEXT			pglGenFramebuffersEXT
#define glBindFramebufferEXT			pglBindFramebufferEXT
#define glFramebufferTexture2DEXT		pglFramebufferTexture2DEXT
#define glFramebufferRenderbufferEXT	pglFramebufferRenderbufferEXT

extern PFNGLGENBUFFERSARBPROC pglGenBuffersARB ;                     // VBO Name Generation Procedure
extern PFNGLBINDBUFFERARBPROC pglBindBufferARB ;                     // VBO Bind Procedure
extern PFNGLBUFFERDATAARBPROC pglBufferDataARB ;                     // VBO Data Loading Procedure
extern PFNGLBUFFERSUBDATAARBPROC pglBufferSubDataARB ;               // VBO Sub Data Loading Procedure
extern PFNGLDELETEBUFFERSARBPROC pglDeleteBuffersARB ;               // VBO Deletion Procedure
extern PFNGLGETBUFFERPARAMETERIVARBPROC pglGetBufferParameterivARB ; // return various parameters of VBO
extern PFNGLMAPBUFFERARBPROC pglMapBufferARB ;                       // map VBO procedure
extern PFNGLUNMAPBUFFERARBPROC pglUnmapBufferARB ;                   // unmap VBO procedure


extern PFNGLGENRENDERBUFFERSEXTPROC			pglGenRenderbuffersEXT;
extern PFNGLBINDRENDERBUFFEREXTPROC			pglBindRenderbufferEXT;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC		pglRenderbufferStorageEXT;
extern PFNGLGENFRAMEBUFFERSEXTPROC			pglGenFramebuffersEXT;
extern PFNGLBINDFRAMEBUFFEREXTPROC			pglBindFramebufferEXT;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC		pglFramebufferTexture2DEXT;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC	pglFramebufferRenderbufferEXT;

//
// SHADER
//

#define glCompileShader						pglCompileShader
#define glShaderSource						pglShaderSource
#define glCreateShader						pglCreateShader
#define glGetShaderiv						pglGetShaderiv
#define glDeleteShader						pglDeleteShader
#define glGetShaderInfoLog					pglGetShaderInfoLog
#define glCreateProgram						pglCreateProgram
#define glAttachShader						pglAttachShader
#define glLinkProgram						pglLinkProgram
#define glGetProgramiv						pglGetProgramiv
#define glUseProgram						pglUseProgram
#define glVertexAttribPointer				pglVertexAttribPointer
#define glEnableVertexAttribArray			pglEnableVertexAttribArray
#define glDisableVertexAttribArray			pglDisableVertexAttribArray
#define glGetAttribLocation					pglGetAttribLocation
#define glGetProgramInfoLog					pglGetProgramInfoLog
#define glGetUniformLocation				pglGetUniformLocation
#define glUniform1i							pglUniform1i
#define glUniform1f							pglUniform1f
#define glUniform2f							pglUniform2f
#define glUniform3f							pglUniform3f
#define glUniform4f							pglUniform4f
#define glUniformMatrix4fv					pglUniformMatrix4fv

extern PFNGLCOMPILESHADERPROC				pglCompileShader;
extern PFNGLSHADERSOURCEPROC				pglShaderSource;
extern PFNGLCREATESHADERPROC				pglCreateShader;
extern PFNGLGETSHADERIVPROC					pglGetShaderiv;
extern PFNGLDELETESHADERPROC				pglDeleteShader;
extern PFNGLGETSHADERINFOLOGPROC			pglGetShaderInfoLog;
extern PFNGLCREATEPROGRAMPROC				pglCreateProgram;
extern PFNGLATTACHSHADERPROC				pglAttachShader;
extern PFNGLLINKPROGRAMPROC					pglLinkProgram;
extern PFNGLGETPROGRAMIVPROC				pglGetProgramiv;
extern PFNGLUSEPROGRAMPROC					pglUseProgram;
extern PFNGLVERTEXATTRIBPOINTERPROC			pglVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC		pglEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC	pglDisableVertexAttribArray;
extern PFNGLGETATTRIBLOCATIONPROC			pglGetAttribLocation;
extern PFNGLGETPROGRAMINFOLOGPROC			pglGetProgramInfoLog;
extern PFNGLGETUNIFORMLOCATIONPROC			pglGetUniformLocation;
extern PFNGLUNIFORM1IPROC					pglUniform1i;
extern PFNGLUNIFORM1FPROC					pglUniform1f;
extern PFNGLUNIFORM2FPROC					pglUniform2f;
extern PFNGLUNIFORM3FPROC					pglUniform3f;
extern PFNGLUNIFORM4FPROC					pglUniform4f;
extern PFNGLUNIFORMMATRIX4FVPROC			pglUniformMatrix4fv;

#endif

void loadGLfunctions();

#endif
*/
