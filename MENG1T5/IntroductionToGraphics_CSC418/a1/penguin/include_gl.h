/*************************************************************************
    CSC418/2504, Winter 20l5
    Assignment 1

    Instructions:
        See main.cpp for more instructions.

        This file inclueds the necessary files in order to use OpenGL,
	and defines some helpful macros.
**************************************************************************/

#ifndef __include_gl_h__
#define __include_gl_h__

// On Windows machines, windows.h needs to be the first thing included.
#ifdef _WIN32
#include <windows.h>
#endif

// For older OpenGL headers, we need to turn on extensions.
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif

// Include the OpenGL headers.
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

// Define the BUFFER_OFFSET macro that is used in many examples.
#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(offset)  ((void *)(offset))
#endif

// This helper macro will tell you if new OpenGL functions like VAO's are
// supported.
#define supportsNewOpenGL() \
    ((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_3_3) != 0)

// This macro can be helpful to track down the cause of OpenGL errors.
#include <iostream>
#define checkForGLErrors() \
{ \
    GLenum error_code = glGetError(); \
    if (error_code != GL_NO_ERROR) \
        std::cerr << __FILE__ << ":" << __LINE__ << ": " \
            << gluErrorString(error_code) << "\n"; \
}

// Wrap your OpenGL function calls in this macro and #define DEBUG_OPENGL
// to check the result after each call.
#ifdef DEBUG_OPENGL
    #define ENSURE(function_call) \
    { \
        function_call; \
        checkForGLErrors(); \
    }
#else
    #define ENSURE(function_call) function_call;
#endif

#endif
