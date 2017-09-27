/*************************************************************************
    CSC418/2504, Winter 20l5
    Assignment 1

    Instructions:
        See main.cpp for more instructions.

	This file contains a class to manage the OpenGL state.  Its main
	purposes are to load GLSL shaders and bind values to them.
**************************************************************************/

#ifndef __GLState_h__
#define __GLState_h__

#include "Matrix4x4.h"
#include "GLTransformStack.h"
#include <string>

class GLState
{
public:
    // These are the locations in the shaders of inputs that will be sent
    // there from this code.  If you change the locations in the shaders,
    // change these values.
    static const int VERTEX_POSITION_SHADER_LOCATION = 0;

    GLState()
	: m_projection(Matrix4x4::buildIdentity())
	, m_glsl_program(-1)
	, m_color_shader_location(-1)
	, m_transform_shader_location(-1)
	, m_projection_shader_location(-1)
    {}

    void initializeGL();

    void setColor(GLfloat red, GLfloat green, GLfloat blue);

    void setOrthographicProjectionFromWidthAndHeight(
	GLfloat width, GLfloat height);

    GLTransformStack &transformStack()
    { return m_transform_stack; }

private:
    typedef struct
    {
	GLenum shader_type;
	const char* file_name;
    } ShaderInfo;

    void updateProjectionTransform();

    void loadShaders(ShaderInfo* shaders);
    std::string readShaderFromFile(const char* filename);

private:
    GLTransformStack m_transform_stack;
    Matrix4x4 m_projection;
    GLuint m_glsl_program;
    int m_color_shader_location;
    int m_transform_shader_location;
    int m_projection_shader_location;
};

#endif
