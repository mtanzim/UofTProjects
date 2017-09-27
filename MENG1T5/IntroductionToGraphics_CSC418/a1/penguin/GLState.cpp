/*************************************************************************
    CSC418/2504, Winter 20l5
    Assignment 1

    Instructions:
        See main.cpp for more instructions.

	This file contains a class to bind values to OpenGL shaders to set
	the OpenGL state.
**************************************************************************/

#include "include_gl.h"
#include "GLState.h"
#include <QtOpenGL/QGLFormat>
#include <cstdlib>
#include <cstdio>
#include <cassert>

void GLState::initializeGL()
{
    if (supportsNewOpenGL())
    {
        // Load our vertex and fragment shaders and make them active.
        ShaderInfo shaders[] =
        {
            { GL_VERTEX_SHADER, "vertex.glsl" },
            { GL_FRAGMENT_SHADER, "fragment.glsl" },
            { GL_NONE, NULL }
        };
        loadShaders(shaders);

	m_color_shader_location = glGetUniformLocation(
	    m_glsl_program, "vertex_color");
	assert(m_color_shader_location >= 0);

	m_transform_shader_location = glGetUniformLocation(
	    m_glsl_program, "vertex_transform");
	assert(m_transform_shader_location >= 0);

	m_projection_shader_location = glGetUniformLocation(
	    m_glsl_program, "projection");
	assert(m_projection_shader_location >= 0);

	// Tell the vertex transform stack the shader input location where it
	// should set transformations.
	m_transform_stack.setShaderLocation(m_transform_shader_location);
    }
}

std::string GLState::readShaderFromFile(const char* file_name)
{
#ifdef WIN32
    FILE* open_file = fopen_s(&open_file, file_name, "rb");
#else
    FILE* open_file = fopen(file_name, "rb");
#endif // WIN32

    if (!open_file)
    {
	std::cerr << "Fatal error: Unable to open shader file: " << file_name
	    << "\n";
	exit(1);
	return NULL;
    }

    fseek(open_file, 0, SEEK_END);
    int len = ftell(open_file);
    fseek(open_file, 0, SEEK_SET);

    std::string source;
    source.resize(len);

    fread(&source[0], 1, len, open_file);
    fclose(open_file);

    return source;
}

void GLState::loadShaders(ShaderInfo* shaders)
{
    // Load the GLSL shaders specified in the ShaderInfo structure and make the
    // program containing them active.
    assert(shaders);

    checkForGLErrors();
    m_glsl_program = glCreateProgram();

    for (ShaderInfo *entry=shaders; entry->shader_type != GL_NONE; ++entry)
    {
	// Create a shader, set the source code, and compile it.
	GLuint shader = glCreateShader(entry->shader_type);
	const char *source = readShaderFromFile(entry->file_name).c_str();
	glShaderSource(
	    shader, 1, &source, /*length=*/NULL);
	glCompileShader(shader);

	// Make sure the shader compiled, and print the compiler errors if
	// it didn't.
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
	    GLsizei len = 0;
	    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

	    GLchar* log = new GLchar[len];
	    glGetShaderInfoLog(shader, len, &len, log);
	    std::cerr << "Fatal Error: Shader compilation failed for "
		<< entry->file_name << ": " << log << "\n";
	    delete [] log;

	    exit(1);
	}

	// Add the shader to this program.
	glAttachShader(m_glsl_program, shader);
    }

    // Link the program, and print errors if it didn't link.
    glLinkProgram(m_glsl_program);

    GLint linked;
    glGetProgramiv(m_glsl_program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
	GLsizei len = 0;
	glGetProgramiv(m_glsl_program, GL_INFO_LOG_LENGTH, &len);

	GLchar* log = new GLchar[len+1];
	glGetProgramInfoLog(m_glsl_program, len, &len, log);
	std::cerr << "Fatal Error: Shader linking failed: " << log << "\n";
	delete [] log;

	exit(1);
    }

    glUseProgram(m_glsl_program);
    checkForGLErrors();
}

void GLState::setColor(GLfloat red, GLfloat green, GLfloat blue)
{
    if (supportsNewOpenGL())
	glUniform4f(
	    m_color_shader_location, red, green, blue, /*alpha=*/1.0);
    else
	glColor3f(red, green, blue);
}

void GLState::setOrthographicProjectionFromWidthAndHeight(
    GLfloat width, GLfloat height)
{
    GLfloat left = -width * 0.5;
    GLfloat right = width * 0.5;
    GLfloat bottom = -height * 0.5;
    GLfloat top = height * 0.5;

    if (supportsNewOpenGL())
    {
	m_projection = Matrix4x4::buildOrtho(
	    left, right, bottom, top, /*near=*/-1.0, /*far=*/1.0);
	updateProjectionTransform();
    }
    else
    {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left, right, bottom, top, /*near=*/-1.0, /*far=*/1.0);
	glMatrixMode(GL_MODELVIEW);
    }
}

void GLState::updateProjectionTransform()
{
    assert(supportsNewOpenGL());

    // Set this matrix into the current shader program.
    glUniformMatrix4fv(
	m_projection_shader_location,
	/*count=*/1,
	/*transpose=*/true,
	m_projection.values());
}
